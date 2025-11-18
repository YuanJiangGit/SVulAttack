# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/12 8:57
# @Function: data process


import pandas as pd
from tqdm import tqdm

from Config.ConfigT import MyConf
from Utils.mapping import *


class DataPipline_Big_Vul:
    def __init__(self, config):
        self.config = config
        self.columns = ['data_id', 'SyVCs', 'file_fun', 'program_id', 'types', 'map_code', 'orig_code', 'label']

    def load_all(self, data_dir):
        '''
        load all samples
        :param data_dir:
        :return:
        '''
        data = pd.DataFrame(columns=self.columns)
        df = pd.read_csv(data_dir)
        for index, row in tqdm(df.iterrows(), total = len(df)):
            temp_list = []
            for line in row['func_before'].split('\n'):
                tokens = create_tokens(line)  # tokens: list
                temp_list.append(tokens)
            map_program1, _ = mapping(temp_list)
            data_of_sub = {'data_id': row['Unnamed: 0'], 'SyVCs': "", 'file_fun': "", 'program_id': row['Unnamed: 0'], 'types': "", 'map_code': map_program1,
                'orig_code': row['func_before'].split('\n'), 'label': row['vul']}
            data_of_sub = pd.DataFrame(data_of_sub, columns=self.columns)
            data = pd.concat([data, data_of_sub]).reset_index(drop=True)  # 重置索引
        data.to_pickle(os.path.join(self.config.root_path, 'data.pkl'))

    @ staticmethod
    def states2idseq(statement_lst,vocab,max_token):
        '''
        convert code symbols in statement_lst into symbols index in vocab
        :param statement_lst: list, a sequence of statements
        :param vocab:
        :param max_token: the maximum integer index of symbol in vocab +1
         :return:
        '''
        sequence = []
        s_lengths = []
        for index, statement in enumerate(statement_lst):
            s_split = create_tokens(statement.lower())
            sequence.extend([vocab[token] if token in vocab else max_token for token in s_split])
            s_lengths.append(len(s_split))
        return [sequence, s_lengths]


def main():
    config = MyConf('./../Config/config.cfg')
    pipline = DataPipline_Big_Vul(config)

    # CSV -> pkl
    # MSR_data_cleaned.csv from https://github.com/rshariffdeen/Big-Vul/tree/master
    # download link https://drive.google.com/file/d/1-0VhnHBp9IGh90s2wCNjeCMuy70HPl8X/view?usp=sharing
    print('load data...')
    data_dir = './../resources/Dataset/MSR_data_cleaned.csv'
    pipline.load_all(data_dir)


if __name__ == '__main__':
    main()
