"""
main_big_vul.py

Main entrypoint for preparing samples and running adversarial attacks on the Big Vul dataset.

This script supports two attack algorithms:
- greedy (Combination_Big_Vul)
- genetic (Genetic_Big_Vul)

It also contains utilities to sample balanced test sets or CWE-specific samples,
and drive the attack runs according to command-line arguments.
"""
import sys
sys.path.append("../")

import json
import warnings
import argparse
import os
import pandas as pd
import random
from gensim.models.word2vec import Word2Vec

from Config.ConfigT import MyConf
from Attack.Combination_Big_Vul import Combination_Big_Vul
from Attack.Genetic_Big_Vul import Genetic_Big_Vul

warnings.filterwarnings("ignore")

class Run_Entry:
    """
    Class that prepares sample sets, and coordinates sample
    selection routines used before launching the attack pipeline.
    """
    def __init__(self, args, config):
        """
        Initialize the runner with parsed arguments and a configuration object.
        Args:
            args: argparse.Namespace, parsed CLI arguments.
            config: configuration object.
        """
        self.args = args
        self.config = config

    def sample_Test(self, test_data):
        """
        Build a balanced test set of 250 positive and 250 negative samples.
        Args:
            test_data: pandas.DataFrame containing the dataset.
        Returns:
            pandas.DataFrame containing the selected samples.
        """
        test_pos = test_data[test_data['label'] == 1]
        test_neg = test_data[test_data['label'] == 0]
        samples_pos = test_pos.sample(n=250, random_state=1)
        samples_neg = test_neg.sample(n=250, random_state=1)
        samples = pd.concat([samples_pos, samples_neg])

        print(samples)
        self.process_samples(samples)
        return samples

    def sample_Test_CWE(self, test_datas, config):
        """
        Construct a CWE-focused sampling of the dataset.
        Args:
            test_datas: pandas.DataFrame containing dataset.
            config: configuration object.
        Returns:
            pandas.DataFrame containing the selected CWE samples.
        """
        if self.args.model == 'LineVul':
            from DataProcess.Append_Data_Big_Vul import LineVulPredict
            t = LineVulPredict(config)
        data = pd.read_csv(self.args.sample_csv)
        # CWE list of interest
        want_list = ['CWE-787', 'CWE-125', 'CWE-20', 'CWE-416', 'CWE-22', 'CWE-190', 'CWE-476', 'CWE-119', 'CWE-200',
                     'CWE-77', 'CWE-399', 'CWE-264', 'CWE-189', 'CWE-362']
        limit_lis = {}
        mp = {}

        for index, w in enumerate(want_list):
            mp[w] = []
        for index, row in data.iterrows():
            if row['CWE ID'] in mp and row['target'] == 1:
                mp[row['CWE ID']].append(index)

        tot = 0
        for i in mp:
            tot += len(mp[i])

        lis = []
        cnt = 0
        for i in want_list:
            lis.append(min(int(500 * len(mp[i]) / tot), limit_lis[i] if i in limit_lis else 500))
            cnt += lis[-1]
            print(i, len(mp[i]), len(mp[i]) / tot, lis[-1])

        print(lis, cnt)

        sorted_indices = sorted(range(len(lis)), key=lambda i: lis[i])
        for i in sorted_indices:
            if want_list[i] in limit_lis and lis[i] >= limit_lis[want_list[i]]:
                continue
            lis[i] += 1
            cnt += 1
            if cnt >= 500:
                break

        print(lis)
        random.seed(12345)
        samples = pd.DataFrame()
        for i in range(0, len(lis)):
            random.shuffle(mp[want_list[i]])
            cnt = 0
            for j in mp[want_list[i]]:
                row = data.iloc[j]
                assert(row['CWE ID'] == want_list[i])
                test_row = test_datas[test_datas['data_id'] == row['index']]
                assert(len(test_row) == 1)
                code = test_row['orig_code'].values[0].copy()
                assert('\n'.join(code) == row['func_before'])
                if len(code) > 150:
                    continue
                code.append('')
                if self.args.model == 'LineVul':
                    if t.predict_adv_program(code) != test_row['label'].values[0]:
                        continue
                cnt += 1
                samples = pd.concat([samples, test_row])
                if cnt >= lis[i]:
                    break
            assert(cnt == lis[i])

        print(samples)
        self.process_samples(samples)
        return samples

    def process_samples(self, samples):
        """
        Prepare sample-related artifacts required by the attack pipeline.
        Creates:
            - individual source files (program_id.cpp)
            - slices saved (indexed by sample index)
            - sample_ids.json
            - index_label.json
        Args:
            samples: pandas.DataFrame containing dataset.
        Returns:
            list of program_ids corresponding to the saved samples.
        """
        tmp_path = self.config.temp_path
        sample_code_path = self.config.sample_code_path
        sample_slices_path = self.config.sample_slice_path
        sample_indexes = []
        samples_program_id = []
        index_label = {}
        for index, row in samples.iterrows():
            index_label[index] = row['label']
            program_id = row['program_id']

            sample_indexes.append(str(index))
            orig_code = row['orig_code']

            # Save the original full program (program_id.cpp)
            with open(os.path.join(sample_code_path, str(program_id) + '.cpp'), 'w') as f:
                f.write('\n'.join(orig_code))

            samples_program_id.append(program_id)

            # Save the slice lines for this sample index
            with open(os.path.join(sample_slices_path, str(index)), 'w') as slice_f:
                for line in orig_code:
                    slice_f.writelines(line+'\n')

        # Save the ordered list of sample ids used in attacks
        sample_ids_json = os.path.join(self.config.root_path, 'sample_ids.json')
        with open(sample_ids_json, 'w') as sjsn:
            json.dump(sample_indexes, sjsn)

        # Save the mapping index -> true label
        index_label_json = 'index_label.json'
        with open(os.path.join(tmp_path, index_label_json), 'w') as label_jsn:
            json.dump(index_label, label_jsn)

        print('Sample Down!')
        return samples_program_id

def main():
    """
    Parse CLI arguments, prepare dataset artifacts, and run the chosen attack algorithm.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', type=str, default='LineVul', choices=['LineVul'])
    parser.add_argument('--sample_csv', type=str, default=None)
    parser.add_argument('--average_sample', type=bool, default=False)
    parser.add_argument('--CWE_sample', type=bool, default=False)
    parser.add_argument('--limits', type=int, default=15)
    parser.add_argument('--batch_size', type=int, default=64)
    parser.add_argument('--result_file', type=str, default='greedy.csv')
    parser.add_argument('--add_tag', type=bool, default=True)
    parser.add_argument('--const_tag', type=bool, default=True)
    parser.add_argument('--macro_tag', type=bool, default=True)
    parser.add_argument('--unroll_loop', type=bool, default=True)
    parser.add_argument('--var_tag', type=bool, default=True)
    parser.add_argument('--random_tag', type=bool, default=False)
    parser.add_argument('--algorithm', type=str, default='greedy', choices=['greedy', 'genetic'])
    args = parser.parse_args()

    # path to the persisted sample-ids file
    sample_ids_json = os.path.join(config.root_path, 'sample_ids.json')

    # Load the dataset
    test_data = pd.read_pickle(config.root_path + '/data.pkl')

    word2vec = Word2Vec.load(config.embedding_path + "/node_w2v_60")

    # Set vocabulary size and mapping on the shared config object
    vectors = word2vec.wv.vectors
    vocab = word2vec.wv.key_to_index

    config.vocab_size = vectors.shape[0] + 1
    config.vocab = vocab

    entry = Run_Entry(args, config)

    if args.CWE_sample:
        entry.sample_Test_CWE(test_data, config)

    if args.average_sample:
        entry.sample_Test(test_data)

    if not os.path.isfile(sample_ids_json):
        print("Sample first!")
        return

    with open(sample_ids_json, 'r') as sample_ids_file:
        programs_indexes = json.load(sample_ids_file)

    # Run the selected algorithm (greedy or genetic)
    if args.algorithm == 'greedy':
        if args.model == 'LineVul':
            combination = Combination_Big_Vul(config,
                                              args.limits,
                                              programs_indexes,
                                              args.batch_size,
                                              model_name='12heads_linevul_model.bin')

        combination.combination_attack(save_name=args.result_file,
                                       add_tag=args.add_tag,
                                       const_tag=args.const_tag,
                                       macro_tag=args.macro_tag,
                                       unroll_loop=args.unroll_loop,
                                       var_tag=args.var_tag,
                                       random_tag=args.random_tag)

    elif args.algorithm == 'genetic':
        if args.model == 'LineVul':
            genetic = Genetic_Big_Vul(config,
                                      args.limits,
                                      programs_indexes,
                                      test_data,
                                      args.batch_size,
                                      model_name='12heads_linevul_model.bin')

        method_dict = {"add_tag": args.add_tag,
                       "const_tag": args.const_tag,
                       "macro_tag": args.macro_tag,
                       "unroll_loop": args.unroll_loop,
                       "var_replace": args.var_tag}

        genetic.attack(args.result_file, method_dict)

if __name__ == '__main__':
    # Instantiate configuration and run main function
    config = MyConf('./../Config/config.cfg')
    main()