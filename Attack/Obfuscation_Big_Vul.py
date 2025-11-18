"""
Obfuscation_Big_Vul.py

Helpers for generating adversarial variants of C/C++ source slices
and querying the target vulnerability detection model.
"""
import json
import os
import pickle
import re
import random
import torch
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
from collections import OrderedDict
from transformers import RobertaConfig, RobertaForSequenceClassification, RobertaTokenizer

from CParser.ParseAndMutCode_Big_Vul import ParseAndMutCode_Big_Vul
from Target_model.linevul_model import Model
from Target_model.linevul_main import convert_examples_to_features

def softmax(x):
    """
    Softmax utility for 1-D numpy array x.
    """
    e_x = np.exp(x)
    return e_x / e_x.sum(axis=0)

class Obfuscation_Big_Vul():
    """
    Backend class for greedy-based attack.
    Responsibilities:
    - load the detection model,
    - provide model batch evaluation functions,
    - compute TF-IDF similarity scores against a corpus of similar files,
    - build auxiliary dictionaries used by obfuscation (add/var/replace),
    - compute per-statement impact by ablation.
    """
    def __init__(self, config, replace_limit_number, sample_ids, test_data, model_name):
        """
        Initialize the obfuscation class.
        Args:
            config: configuration object.
            replace_limit_number: allowed number of modifications per member.
            sample_ids: list of sample ids used for build references set.
            test_data: pandas DataFrame used to collect references code.
            model_name: name of the model.
        """
        self.config = config
        self.model_name = model_name
        self.detect_model = None
        self.detect_model = self.load_trained_model(model_name)
        # vocab used by other utilities
        self.max_token = self.config.vocab_size - 1
        self.vocab = self.config.vocab
        self.replace_limit_number = replace_limit_number
        self.sample_ids = sample_ids
        self.check_model_num = 0

        # Load a precomputed similarity index between files
        with open(os.path.join(config.temp_path, "skidf_sim_between_files.pkl"), 'rb') as f:
            sim_between_files = pickle.load(f)

        # Structures used to store TF-IDF vectors and related data indexed by sample id
        self.compare_file = {}
        self.gen_docs = {}
        self.dictionary = {}
        self.corpus = {}
        self.tf_idf = {}
        self.sims = {}
        self.vectorizer = {}
        self.compare_vecs = {}

        # Build TF-IDF vectors for each sample id
        if sample_ids:
            for times, index in enumerate(sample_ids):
                index = int(index)
                self.compare_file[index] = ['\n'.join(test_data[test_data['data_id'] == source_file[0]].head(1)['orig_code'].values[0])
                                        for source_file in sim_between_files[int(test_data.loc[index, 'data_id'])][0:100]]
                self.vectorizer[index] = TfidfVectorizer()
                self.compare_vecs[index] = self.vectorizer[index].fit_transform(self.compare_file[index])

    def load_trained_model(self, model_name):
        """
        Load the pretrained vulnerability detection model.
        Args:
            model_name: name of the model.
        Returns:
            The loaded model instance or None if loading fails.
        """
        if self.detect_model != None:
            return self.detect_model
        else:
            if model_name == '12heads_linevul_model.bin':
                random.seed(123456)
                np.random.seed(123456)
                torch.manual_seed(123456)
                torch.cuda.manual_seed_all(123456)
                self.block_size = 512
                self.use_word_level_tokenizer = False
                self.best_threshold = 0.5
                self.device = torch.device("cpu")
                config = RobertaConfig.from_pretrained("microsoft/codebert-base")
                config.num_labels = 1
                config.num_attention_heads = 12
                self.tokenizer = RobertaTokenizer.from_pretrained("microsoft/codebert-base")
                detect_model = RobertaForSequenceClassification.from_pretrained("microsoft/codebert-base",
                                                                                config=config,
                                                                                ignore_mismatched_sizes=True)
                detect_model = Model(detect_model, config, self.tokenizer, None)
            else:
                print('Unsupported model name!')
                return None

            # If GPU is enabled in config, move the model to GPU
            if self.config.use_gpu:
                self.device = torch.device("cuda")
                detect_model.to(self.device)
            # load model
            if os.path.exists(self.config.models_path + model_name):
                detect_model.load_state_dict(torch.load(self.config.models_path + model_name, map_location=self.device), strict=False)
                detect_model.to(self.device)
                self.detect_model = detect_model
                return self.detect_model
            else:
                print('No Pretrained Model, Please Train first!')
                return None

    def make_batch(self, map_programs, batch_size):
        """
        Partition a list of programs into batches.
        Args:
            map_programs: list of programs.
            batch_size: batch size.
        Returns:
            List of batches.
        """
        dataset = []
        batch = []
        for i, program in enumerate(map_programs):
            if i % batch_size == 0 and batch != []:
                dataset.append(batch)
                batch = []
            map_program = program
            batch.append(map_program)
        if batch:
            dataset.append(batch)
        return dataset

    def run_model_by_batch(self, dataloader, id, model=None):
        """
        Run the detection model for all programs inside dataloader batches.
        Args:
            dataloader: list of batches.
            id: sample id.
            model: optional model instance to use.
        Returns:
            (predicted_array, probability_list)
            - predicted_array: numpy boolean array of model predictions.
            - probability_list: list of float similarity-based approximate probability.
        """
        if model is None:
            model = self.detect_model
        predicted = []
        probability = []
        for batch in dataloader:
            if self.model_name == '12heads_linevul_model.bin':
                inputs_ids, labels= [], []
                for i in batch:
                    code = ''.join(i)
                    x = convert_examples_to_features(code, 1, self.tokenizer, self)
                    inputs_ids.append(x.input_ids)
                    labels.append(x.label)
                    probability.append(self.Compare(code, id))
                with torch.no_grad():
                    lm_loss, logit = model(input_ids=torch.tensor(inputs_ids).to(self.device), labels=torch.tensor(labels).to(self.device))
                    predicted.append(logit.cpu().numpy())
                continue
        predicted = np.concatenate(predicted, 0)
        predicted = predicted[:, 1] > self.best_threshold
        return predicted, probability

    def Compare(self, adv_program, id):
        """
        Compute a similarity-based approximate probability for adv_program using precomputed TF-IDF vectors.
        The method applies a softmax to cosine similarities with a set of reference files
        and returns a weighted aggregation as the approximate probability.
        Parameters:
            adv_program: list of statements to compare.
            id: sample id.
        Returns:
            A scalar score representing similarity-weighted approximate probability.
        """
        iid = int(id)
        query_vec = self.vectorizer[iid].transform([''.join(adv_program)])
        similarities = cosine_similarity(query_vec, self.compare_vecs[iid])[0]
        probabilities = softmax(similarities)
        tot = sum(sim * prob for sim, prob in zip(similarities, probabilities))
        return tot

    def predict_adv_program(self, adv_program, id):
        """
        Compute the model prediction and an approximate probability for a single program.
        Args:
            adv_program: list of statements representing the program slice.
            id: sample id.
        Returns:
            (predicted_bool, probability_float)
        """
        self.check_model_num += 1
        if self.model_name == '12heads_linevul_model.bin':
            self.detect_model.eval()
            logits = []
            label = 1
            x = convert_examples_to_features(''.join(adv_program), label, self.tokenizer, self)
            inputs_ids = torch.tensor(x.input_ids).unsqueeze(0).to(self.device)
            labels = torch.tensor(x.label).unsqueeze(0).to(self.device)
            with torch.no_grad():
                lm_loss, logit = self.detect_model(input_ids=inputs_ids, labels=labels)
                logits.append(logit.cpu().numpy())
            # calculate scores
            logits = np.concatenate(logits, 0)
            assert(len(logits) == 1)
            predicted = (logits[:, 1] > self.best_threshold)[0]
        probability = self.Compare(adv_program, id)
        return predicted, probability

    def create_add_dict(self):
        """
        Build a mapping from sample index -> per-line variable declarations that can be used for dead-code insertion.
        """
        sample_source_path = self.config.sample_source_path
        temp_path = self.config.temp_path

        # If the intermediate JSON is not present, run the parser to generate it
        if not os.path.exists(os.path.join(temp_path, 'filename_outspace.json')):
            print("processing source code and get filename_outspace.json!")
            pm = ParseAndMutCode_Big_Vul()
            pm.translate_c_add_print(sample_source_path, temp_path)

        filename_outspace_path = os.path.join(temp_path, 'filename_outspace.json')
        with open(filename_outspace_path, 'r') as filename_outspace_jsn:
            filename_outspace_dict = json.load(filename_outspace_jsn)

        index_add_dict = {}
        index_var_dict = {}
        count = 0
        # Iterate over provided sample ids and build per-index maps
        for index in self.sample_ids:
            print(index, count)
            count += 1

            index = str(index)

            add_dict = OrderedDict()
            var_dict = OrderedDict()
            pre = set()

            keyname = '0/' + str(index) + '.cpp'
            if keyname not in filename_outspace_dict:
                continue

            for line in filename_outspace_dict[keyname]:
                variable_declaration = set(filename_outspace_dict[keyname][line])
                if variable_declaration != set():
                    # Remove array/function bracket contents and filter empty results
                    variable_declaration_list = [re.sub(u"\\(.*?\\)|\\[.*?]|\\{.*?}", "", x) for x in variable_declaration]
                    variable_declaration_list = [s for s in variable_declaration_list if s != '']
                    expanded_list = []
                    # Split dotted/arrow accesses and collect valid identifier parts
                    for s in variable_declaration_list:
                        parts = re.split(r'\.|->', s)
                        for part in parts:
                            if bool(re.match(r'^[\w]+$', part)):
                                expanded_list.append(part)
                    var_dict[line] = expanded_list
                    # Keep only first segment for the add-dict (the base variable name)
                    variable_declaration_list = [re.split(r'\.|->', s)[0] for s in variable_declaration_list]
                    variable_declaration = set(variable_declaration_list).union(pre)
                    add_dict[line] = list(variable_declaration)
                    pre = set(variable_declaration)
                else:
                    add_dict[line] = list(pre)
                    var_dict[line] = []

            index_add_dict[index] = add_dict
            index_var_dict[index] = var_dict

        index_add_dict_path = os.path.join(temp_path, 'index_add_dict.json')
        with open(index_add_dict_path, 'w') as index_add_dict_jsn:
            json.dump(index_add_dict, index_add_dict_jsn)

        index_var_dict_path = os.path.join(temp_path, 'index_var_dict.json')
        with open(index_var_dict_path, 'w') as index_var_dict_jsn:
            json.dump(index_var_dict, index_var_dict_jsn)

    def create_replace_dict(self):
        """
        Build a mapping from sample index -> per-line replace templates for constant replacement.
        """
        sample_source_path = self.config.sample_source_path
        temp_path = self.config.temp_path

        # Generate the intermediate file if absent
        if not os.path.exists(os.path.join(temp_path, 'filename_replace_const.json')):
            print("processing source code and get filename_replace_const.json!")
            pm = ParseAndMutCode_Big_Vul()
            pm.translate_c_replace_const(sample_source_path, temp_path)

        filename_replace_const_path = os.path.join(temp_path, 'filename_replace_const.json')
        with open(filename_replace_const_path, 'r') as filename_replace_const_jsn:
            filename_replace_const_dict = json.load(filename_replace_const_jsn)

        index_replace_dict = {}
        count = 0
        for index in self.sample_ids:
            print(index, count)
            count += 1

            index = str(index)

            program_slice_path = os.path.join(self.config.sample_slice_path, index)
            with open(program_slice_path, 'r', encoding='cp1252') as program_slice_f:
                program_slice = program_slice_f.readlines()
            line_replace_dict = {}

            key = '0/' + str(index) + '.cpp'
            if key not in filename_replace_const_dict:
                continue

            for slice_line in range(len(program_slice)):
                if str(slice_line) in filename_replace_const_dict[key].keys():
                    line_replace_dict[slice_line] = filename_replace_const_dict[key][str(slice_line)]
            index_replace_dict[index] = line_replace_dict

        index_replace_dict_path = os.path.join(temp_path, 'index_replace_dict.json')
        with open(index_replace_dict_path, 'w') as index_replace_dict_jsn:
            json.dump(index_replace_dict, index_replace_dict_jsn)

    def statement_impact(self, map_program, ori_label, ori_prob, id):
        """
        Compute the importance of each statement by ablation.
        For each statement index, remove that statement and query the model.
        If the label flips from original label, assign maximal importance 1.
        Otherwise, use the prob difference (prob - ori_prob) as the impact.
        Args:
            map_program: list of statements.
            ori_label: original label.
            ori_prob: original approximate probability.
            id: sample id.
        Returns:
            numpy array of statement indices sorted in decreasing importance.
        """
        state_score_lst = []
        for state_index in range(len(map_program)):
            _temp_map_program = map_program[:state_index] + map_program[state_index + 1:]

            label, prob = self.predict_adv_program(_temp_map_program, id)
            if label != ori_label:
                state_score = 1
            else:
                state_score = prob - ori_prob

            state_score_lst.append(state_score)
        state_score_arr = np.array(state_score_lst)
        sort_index = np.argsort(-state_score_arr)
        return sort_index

    def insert_statement0(self, variable_declaration, best_word):
        """
        Composes a simple dead-code insertion string.
        Args:
            variable_declaration: variable name used as address operand.
            best_word: token included in the printf payload.
        Returns:
            A list containing a single statement string to be inserted.
        """
        line1 = 'printf(\"'  + variable_declaration + '_2 = ' + best_word + ', %p ' + ' \" , & ' + variable_declaration + ');'
        return [line1]
