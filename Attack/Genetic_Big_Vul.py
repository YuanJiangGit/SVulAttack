"""
Genetic_Big_Vul.py

This module implements a genetic-algorithm-based adversarial attack procedure for the "Big Vul" dataset.
"""
import copy
import json
import os
import csv
import torch
import random
import re
import pickle
import torch.nn.functional as F
import numpy as np
from transformers import RobertaConfig, RobertaForSequenceClassification, RobertaTokenizer
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

from Utils.get_tokens import create_tokens
from Utils.Util import concat_statement
from Target_model.linevul_model import Model
from Target_model.linevul_main import convert_examples_to_features

class Population():
    """
    Container representing the population and intermediate state during GA optimization
    for a single program/sample.
    Attributes:
        config: global configuration object.
        index: sample id.
        ori_label: original label.
        sample_slice: original program lines.
        translit_slice: the program in list-of-lists form.
        members: list of population members in flat statement form.
        translit_members: list of population members in list-of-lists form.
        used_method: list tracking which mutation methods were applied for each member.
        line_replaced: dict mapping line index -> list of available candidate transformation info.
        fitness_scores: fitness score list for the current members.
        macro_names: list of candidate macro names (tokens) for macro replacement.
        result: best resulting program.
        used_method_result: used_method for the best result.
        translit_result: translit version of the best result.
        predicts: predicted labels for members.
        predict: predicted label of the best member.
        fitness_score: fitness score of the best member.
    """
    def __init__(self, config, sample_slice, index, ori_label):
        self.config = config
        self.index = index
        self.ori_label = ori_label
        self.sample_slice = sample_slice
        self.translit_slice = None
        self.members = []
        self.translit_members = []
        self.used_method = []
        self.line_replaced = {}
        self.fitness_scores = None
        self.macro_names = None
        self.result = sample_slice
        self.used_method_result = None
        self.translit_result = None
        self.predicts = []
        self.predict = ori_label
        self.fitness_score = 0


class GeneticAlgorithm():
    """
    Genetic algorithm implementation for generating adversarial program variants.
    Parameters:
        config: global configuration object.
        limits: allowed number of modifications per member.
        sample_ids: list of sample ids used for build references set.
        test_data: pandas DataFrame used to collect references code.
        batch_size: batch size of evaluation.
        model_name: name of the model.
        pop_size: size of the GA population (default 45).
        max_iters: maximum number of GA iterations (default 17).
    """
    def __init__(self, config, limits, sample_ids, test_data, batch_size, model_name, pop_size=45, max_iters=17):
        self.pop_size = pop_size
        self.max_iters = max_iters
        self.check_model_num = 0  # counter tracking number of model queries performed
        self.config = config
        self.batch_size = batch_size
        self.model_name = model_name
        self.limits = limits
        self.ratio = {"select":0.4, "cross":0.6, "mutate":0.6}
        self.load_temp_files()

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
                self.compare_file[index] = [
                    '\n'.join(test_data[test_data['data_id'] == source_file[0]].head(1)['orig_code'].values[0])
                    for source_file in sim_between_files[int(test_data.loc[index, 'data_id'])][0:100]]
                self.vectorizer[index] = TfidfVectorizer()
                self.compare_vecs[index] = self.vectorizer[index].fit_transform(self.compare_file[index])

    def load_temp_files(self):
        """
        Load model, token importance lists and index dictionaries.
        """
        if self.model_name == '12heads_linevul_model.bin':
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

        # If GPU is enabled in config, move the model to GPU
        if self.config.use_gpu:
            self.device = torch.device("cuda")
            detect_model.to(self.device)
        # Load model
        if os.path.exists(self.config.models_path + self.model_name):
            detect_model.load_state_dict(torch.load(self.config.models_path + self.model_name, map_location=self.device),
                                         strict=False)
            detect_model.to(self.device)

        self.detect_model = detect_model

        with open(os.path.join(self.config.temp_path, "index_add_dict.json"), 'r') as index_add_dict_jsn:
            self.index_add_dict = json.load(index_add_dict_jsn)

        with open(os.path.join(self.config.temp_path, "index_replace_dict.json"), 'r') as index_replace_dict_jsn:
            self.index_replace_dict = json.load(index_replace_dict_jsn)

        with open(os.path.join(self.config.temp_path, 'index_var_dict.json'), 'r') as index_var_dict_jsn:
            self.index_var_dict = json.load(index_var_dict_jsn)

        with open(os.path.join(self.config.temp_path, "token_importance_" + self.model_name+ ".json"), 'r') as token_importance_jsn:
            token_importance = json.load(token_importance_jsn)
        self.label_important_tokens = {}
        for label in token_importance.keys():
            tmp = []
            for token in token_importance[label]:
                tmp.append([token, int(token_importance[label][token])])
            tmp.sort(key=lambda x: x[1], reverse=True)
            self.label_important_tokens[int(label)] = tmp

    def keep_leading_whitespace(self, s):
        """
        Return the leading whitespace characters for given string.
        """
        match = re.match(r'^\s*', s)
        return match.group() if match else ''

    def var_replace_word(self, text, target_word, replacement):
        """
        Replace occurrences of target_word in text.
        """
        pattern = r'\b' + re.escape(target_word) + r'\b'
        result = re.sub(pattern, replacement, text)
        return result

    def create_candidates(self, population, add_tag, const_tag, macro_tag, unroll_loop, var_replace):
        """
        Build candidate transformations for each line in the provided Population.
        The produced candidates for each line are stored in population.line_replaced as a list
        of pairs [method_name, method_payload] where method_name is one of:
        - "add": payload is a list of variable declarations eligible for dead-code insertion
        - "const": payload is the replacement templates for constant replacement
        - "macro": payload is None (macro replacements are handled with macro_names)
        - "loop": payload is None
        - "var": payload is a list of variable candidates for rename
        """
        n = len(population.sample_slice)
        # Collect existing tokens in the sample to avoid reusing them
        curr_tokens = []
        for sentence in population.sample_slice:
            curr_tokens.extend(create_tokens(sentence))

        def is_alnum_underscore(string):
            return bool(re.match(r'^[\w]+$', string))

        # C/C++ reserved tokens
        avoid_list = [
            'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor', 'bool', 'break',
            'case', 'catch', 'char', 'char8_t', 'char16_t', 'char32_t', 'class', 'compl', 'const',
            'constexpr', 'const_cast', 'continue', 'co_await', 'co_return', 'co_yield', 'decltype', 'default', 'delete',
            'do', 'NULL', 'null', 'int64', 'bool', 'case', 'long', 'true', 'false', 'uint64_t',
            'double', 'dynamic_cast', 'else', 'enum', 'explicit', 'export', 'extern', 'false', 'float',
            'for', 'friend', 'goto', 'if', 'inline', 'int', 'long', 'mutable', 'namespace', 'new', 'noexcept',
            'not', 'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected', 'public',
            'register', 'reinterpret_cast', 'requires', 'return', 'short', 'signed', 'sizeof',
            'static', 'static_assert', 'static_cast', 'struct', 'switch', 'template', 'this', 'thread_local',
            'throw', 'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using',
            'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq', 'printf'
        ]
        # Build macro names by selecting top important tokens that pass filtering rules
        macro_names = []
        for token in self.label_important_tokens[population.ori_label]:
            tok = token[0]
            if tok in curr_tokens or tok.upper() in curr_tokens:
                continue
            if not is_alnum_underscore(tok) or tok in avoid_list or tok.upper() in avoid_list or tok.isdigit():
                continue
            if len(tok) < 3:
                continue
            macro_names.append(tok)
        population.macro_names = macro_names
        for line in range(n):
            tmp = []
            if str(line-1) in self.index_add_dict[population.index].keys() and self.index_add_dict[population.index][str(line-1)]!=[] and add_tag:
                tmp.append(["add", self.index_add_dict[population.index][str(line-1)]])
            if str(line) in self.index_replace_dict[population.index].keys() and const_tag:
                tmp.append(["const", self.index_replace_dict[population.index][str(line)]])
            if macro_tag:
                tmp.append(["macro", None])
            if unroll_loop:
                tmp.append(["loop", None])
            if str(line) in self.index_var_dict[population.index].keys() and self.index_var_dict[population.index][str(line)]!=[] and var_replace:
                tmp.append(["var", self.index_var_dict[population.index][str(line)]])
            population.line_replaced[line] = tmp

    def create_population_member(self, translit_slice, used_method, candidate_info, line, candidate_words, used_macro_all):
        """
        Generate a single population member by applying a single candidate operation.
        Parameters:
            translit_slice: program in list-of-lists form.
            used_method: dictionary recording counts/sets of methods used on this line.
            candidate_info: pair [method_name, payload] describing the candidate operation.
            line: target line index.
            candidate_words: list of candidate macro/word tokens to use for macro/const/loop flags.
            used_macro_all: set of macros already used in the member.
        Returns:
            (translit_slice_after_modification, used_method_updated)
        """
        # "add" operation: insert a dead-code statement using a chosen variable and word
        if candidate_info[0] == "add":
            variable_declaration = random.choice(candidate_info[1])
            word = random.choice(candidate_words)
            insert_statement_dead_code = ['printf(\"'  + variable_declaration + '_2 = ' + word + ', %p ' + ' \" , & ' + variable_declaration + ');']
            insert_statement_dead_code = [translit_slice[line][0].strip('\n')] + insert_statement_dead_code + \
                                         translit_slice[line][1:] + ['\n' if '\n' in translit_slice[line][0] else '']
            translit_slice = translit_slice[:line] + [insert_statement_dead_code] + translit_slice[line + 1:]
            used_method["add"] += 1
        # "const" operation: fill placeholders in replacement templates once per member
        elif candidate_info[0] == "const":
            if used_method["const"] == 0:
                cnt_var = candidate_info[1][0].count('<unk>') // 10
                candidate_words = sorted(list(set(candidate_words) - used_macro_all))
                word = random.sample(candidate_words, cnt_var)
                var_replace_content = copy.deepcopy(candidate_info[1])
                for j in range(cnt_var):
                    used_method['used'].add(word[j])
                    var_replace_content[0] = var_replace_content[0].replace('<unk>' * 10, word[j], 1)
                    var_replace_content[1] = var_replace_content[1].replace('<unk>' * 10, word[j], 1)
                head = self.keep_leading_whitespace(translit_slice[line][0])
                new_replace_content = [head, var_replace_content[0], var_replace_content[1]]
                translit_slice = translit_slice[0:line] + [new_replace_content + translit_slice[line][1:] + [
                    '\n' if '\n' in translit_slice[line][0] else '']] + translit_slice[line + 1:]
                used_method["const"] = 1
        # "loop" operation: attempt to convert one form of a loop into another equivalent form
        elif candidate_info[0] == "loop":
            if used_method["loop"] == 0 and translit_slice[line]!=[] and line<len(translit_slice)-1:
                candidate_words = sorted(list(set(candidate_words) - used_macro_all))
                flag = random.choice(candidate_words)
                line_content = translit_slice[line][0]
                res = translit_slice[line][1:]
                line_tokens = create_tokens(line_content)
                if "while" in line_tokens:
                    r1 = re.compile(r'[(](.*)[)]', re.S)
                    exp = re.findall(r1, line_content)
                    if exp != []:
                        used_method["used"].add(flag)
                        head = self.keep_leading_whitespace(line_content)
                        statements = ["bool " + flag + " =true;", "while(" + flag + "){"]
                        statements1 = ["if(!(" + exp[0] + "))" + flag + "=false;"]
                        if '{' not in line_tokens and '{' in translit_slice[line + 1][0]:
                            translit_slice = (translit_slice[:line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line_content else '']] +
                                              [[translit_slice[line + 1][0].replace('{', '', 1)] + translit_slice[line + 1][1:]] + translit_slice[
                                                                                                       line + 2:])
                        else:
                            translit_slice = translit_slice[:line] + [[head] + statements + statements1 + res + [
                                '\n' if '\n' in line_content else '']] + translit_slice[line + 1:]
                        used_method["loop"] = 1
                elif "for" in line_tokens:
                    r1 = re.compile(r'[(](.*)[)]', re.S)
                    exp = re.findall(r1, line_content)
                    if exp:
                        i_s = exp[0].split(';')
                        if len(i_s) == 3:
                            used_method["used"].add(flag)
                            statements = ["bool " + flag + " =true;",
                                          "for(" + i_s[0] + ';' + flag + ';' + i_s[2] + "){"]
                            statements1 = ["if(!(" + i_s[1] + "))" + flag + "=false;"]
                            head = self.keep_leading_whitespace(line_content)
                            if '{' not in line_tokens and '{' in translit_slice[line + 1][0]:
                                translit_slice = (translit_slice[:line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line_content else '']] +
                                                  [[translit_slice[line + 1][0].replace('{', '', 1)] + translit_slice[line + 1][1:]] + translit_slice[
                                                                                                  line + 2:])
                            else:
                                translit_slice = translit_slice[:line] + [[head] + statements + statements1 + res + [
                                    '\n' if '\n' in line_content else '']] + translit_slice[line + 1:]
                            used_method["loop"] = 1
        # "var" operation: attempt to rename one variable occurrence across the program
        elif candidate_info[0] == 'var':
            if len(used_method["var"]) < len(candidate_info[1]):
                candidate_index = copy.deepcopy(candidate_info[1])
                random.shuffle(candidate_index)
                candidate_words = sorted(list(set(candidate_words) - used_macro_all))
                word = random.choice(candidate_words)
                replace = False
                for var in candidate_index:
                    for i in range(len(translit_slice)):
                        for j in range(len(translit_slice[i])):
                            r = self.var_replace_word(copy.deepcopy(translit_slice[i][j]), var, word)
                            if r != translit_slice[i][j]:
                                translit_slice[i][j] = r
                                replace = True
                    if replace:
                        used_method["used"].add(word)
                        used_method["var"].add((var, word))
                        break
        # "macro" operation: perform a token-level substitution on the targeted line
        else:
            candidate_words = set(candidate_words) - used_macro_all
            if candidate_words != set() and used_method["macro"] == 0 and line < len(translit_slice) and translit_slice[line] != []:
                word = random.choice(sorted(list(candidate_words)))
                curr_tokens = create_tokens(translit_slice[line][0])
                curr_tokens = [curr_token for curr_token in curr_tokens if curr_token != '\n']
                if len(curr_tokens) > 0:
                    replace_location = random.randint(0, len(curr_tokens)-1)
                    head = self.keep_leading_whitespace(translit_slice[line][0])
                    new_tokens = curr_tokens[:replace_location] + [word] + curr_tokens[replace_location+1:]
                    new_line = [head] + [" ".join(new_tokens)] + translit_slice[line][1:] + ['\n' if '\n' in translit_slice[line][0] else '']
                    translit_slice = translit_slice[:line] + [new_line] + translit_slice[line+1:]
                    used_method["used"].add(word)
                    used_method["macro"] = 1
        return translit_slice, used_method

    def get_limits(self, member_used_method):
        """
        Calculate the total number of modifications.
        """
        curr_modify = 0
        for line_used_method in member_used_method[:-1]:
            for method, count in line_used_method.items():
                if method == "used":
                    curr_modify += 0
                elif method == "var":
                    curr_modify += len(count)
                else:
                    curr_modify += count
        return curr_modify

    def check_limits(self, population):
        """
        Ensure the population respects modification limits.
        This function filters population members and reproduces the population to the requested population size
        by duplicating or fallback to the previous best member if necessary.
        """
        members = []
        translit_members = []
        used_method = []
        for pos, member_used_method in enumerate(population.used_method):
            curr_modify = 0
            for line_used_method in member_used_method[:-1]:
                for method, count in line_used_method.items():
                    if method == "used":
                        curr_modify += 0
                    elif method == "var":
                        curr_modify += len(count)
                    else:
                        curr_modify += count
            # If within limits and length preserved, keep the member
            if curr_modify <= self.limits and len(population.translit_members[pos]) == len(population.translit_slice):
                members.append(population.members[pos])
                translit_members.append(population.translit_members[pos])
                used_method.append(population.used_method[pos])
        # If no valid members remain, fall back to a population of identical copies of the result
        if len(members) == 0:
            for _ in range(self.pop_size):
                members.append(copy.deepcopy(population.result))
                translit_members.append(copy.deepcopy(population.translit_slice))
                used_method.append(copy.deepcopy(population.used_method_result))
        else:
            # If fewer than pop_size, duplicate randomly selected valid members to refill the population
            for _ in range(self.pop_size-len(members)):
                copy_pos = random.randint(0, len(members)-1)
                members.append(copy.deepcopy(members[copy_pos]))
                translit_members.append(copy.deepcopy(translit_members[copy_pos]))
                used_method.append(copy.deepcopy(used_method[copy_pos]))
        population.members = members
        population.translit_members = translit_members
        population.used_method = used_method

    def make_batch(self, map_programs, batch_size):
        """
        Split a list of programs into batches for model evaluation.
        Parameters:
            map_programs: list of programs.
            batch_size: desired batch size.
        Returns:
            A list of batches.
        """
        dataset = []
        batch = []
        for i, program in enumerate(map_programs):
            if i % batch_size == 0 and batch != []:
                dataset.append(batch)
                batch = []
            batch.append(program)
        if batch != []:
            dataset.append(batch)
        return dataset

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
        def softmax(x):
            e_x = np.exp(x)
            return e_x / e_x.sum(axis=0)

        iid = int(id)
        query_vec = self.vectorizer[iid].transform([''.join(adv_program)])
        similarities = cosine_similarity(query_vec, self.compare_vecs[iid])[0]
        probabilities = softmax(similarities)
        tot = sum(sim * prob for sim, prob in zip(similarities, probabilities))
        return tot

    def predict_single(self, program_slice, id):
        """
        Predict label and compute approximate probability for a single program slice.
        Returns:
            (predicted_label, probability) where predicted_label is the model's binary decision
            and probability is the similarity-based approximate probability.
        """
        self.check_model_num += 1
        if self.model_name == '12heads_linevul_model.bin':
            self.detect_model.eval()
            logits = []
            label = 1
            x = convert_examples_to_features(''.join(program_slice), label, self.tokenizer, self)
            inputs_ids = torch.tensor(x.input_ids).unsqueeze(0).to(self.device)
            labels = torch.tensor(x.label).unsqueeze(0).to(self.device)
            with torch.no_grad():
                lm_loss, logit = self.detect_model(input_ids=inputs_ids, labels=labels)
                logits.append(logit.cpu().numpy())
            # calculate scores
            logits = np.concatenate(logits, 0)
            assert(len(logits) == 1)
            predicted = (logits[:, 1] > self.best_threshold)[0]
        probability = self.Compare(program_slice, id)
        return predicted, probability

    def statement_to_list(self, sample_slice):
        """
        Convert a flat list of statements into a list-of-lists format where
        each original statement is wrapped in its own list.
        This representation makes it easier to insert additional statements in the one line.
        """
        l_sample_slice = []
        for statement in sample_slice:
            l_sample_slice.append([statement])
        return l_sample_slice

    def list_to_statement(self, l_sample_slice):
        """
        Convert back from the list-of-lists representation to a flat list of statements.
        """
        sample_slice = []
        for l in l_sample_slice:
            if l == []:
                continue
            for statement in l:
                sample_slice.append(statement)
        return sample_slice

    def fitness(self, dataloader, ori_label, id):
        """
        Evaluate fitness for a set of batches.
        Parameters:
            dataloader: list of batches
            ori_label: original label
            id: sample id
        Returns:
            (predicted_array, probability_list)
        """
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
        if self.model_name == '12heads_linevul_model.bin':
            predicted = np.concatenate(predicted, 0)
            predicted = predicted[:, 1] > self.best_threshold
            for i in range(len(predicted)):
                if predicted[i] == (1 - ori_label):
                    probability[i] = 1
        return predicted, probability

    def initial_population(self, population, id, method_dict):
        """
        Initialize the genetic algorithm population for a given sample.
        This sets up candidate lists, populates translit_members and members arrays,
        and evaluates the initial fitness for the generated members.
        """
        used_method_init = {"add": 0, "const": 0, "macro": 0, "loop": 0,"var": set(), "used":set()}
        # Build candidate lists for the population according to method_dict flags
        self.create_candidates(population, **method_dict)
        # Convert original slice to translit (list-of-lists) representation
        population.translit_slice = self.statement_to_list(population.sample_slice)
        n = len(population.sample_slice)
        # Prepare used_method template per line and append a final set container
        member_used_method = []
        for l in range(n):
            member_used_method.append(copy.deepcopy(used_method_init))
        member_used_method.append(set())
        population.used_method_result = copy.deepcopy(member_used_method)
        # Create initial population members
        for _ in range(self.pop_size):
            population.used_method.append(copy.deepcopy(member_used_method))
            pre_change_times = self.get_limits(population.used_method[-1])
            try_times = 0
            # Attempt to mutate at most 5 times to produce a non-trivial member
            while pre_change_times == self.get_limits(population.used_method[-1]) and try_times < 5:
                line = random.randint(0, n-1)
                if population.line_replaced[line] == []:
                    try_times += 1
                    continue
                candidate_info = random.choice(population.line_replaced[line])
                used_macro_all = population.used_method[-1][-1]
                translit_slice_tmp = copy.deepcopy(population.translit_slice)
                used_method_tmp = copy.deepcopy(population.used_method[-1][line])
                population_member, update_used_method = self.create_population_member(translit_slice_tmp, used_method_tmp, candidate_info, line, population.macro_names, used_macro_all)
                population.used_method[-1][-1] = population.used_method[-1][-1].union(update_used_method["used"])
                population.used_method[-1][line] = update_used_method
                try_times += 1
            if pre_change_times != self.get_limits(population.used_method[-1]):
                population.translit_members.append(population_member)
                population.members.append(self.list_to_statement(population_member))
            else:
                population.translit_members.append(copy.deepcopy(population.translit_slice))
                population.members.append(copy.deepcopy(population.sample_slice))
        # Evaluate fitness for the initial population
        dataloader = self.make_batch(population.members, self.batch_size)
        population.predicts, population.fitness_scores = self.fitness(dataloader, population.ori_label, id)
        self.check_model_num += len(population.members)
        # Keep best member as the initial result
        state_score_arr = np.array(population.fitness_scores)
        sort_index = np.argsort(-state_score_arr)
        population.predict = population.predicts[sort_index[0]]
        population.fitness_score = population.fitness_scores[sort_index[0]]
        population.result = copy.deepcopy(population.members[sort_index[0]])
        population.translit_result = copy.deepcopy(population.translit_members[sort_index[0]])
        population.used_method_result = copy.deepcopy(population.used_method[sort_index[0]])

    def select(self, population):
        """
        Select a subset of population members using a roulette-wheel selection
        according to softmax-normalized fitness scores.
        Returns:
            (selected_translit_members, selected_used_method)
        """
        probilities = F.softmax(torch.tensor(population.fitness_scores)).tolist()
        probilities[0] += 1-sum(probilities)
        index_list = [i for i in range(self.pop_size)]
        selected_index_list = np.random.choice(index_list, size=int(self.pop_size*self.ratio["select"]), p=probilities)
        selected_members = [copy.deepcopy(population.translit_members[i]) for i in selected_index_list]
        selected_used_method = [copy.deepcopy(population.used_method[i]) for i in selected_index_list]
        return selected_members, selected_used_method

    def cross(self, population):
        """
        Perform crossover between randomly chosen parents.
        Pairs are formed by sampling indices and pairing them sequentially. For each
        pair, a random cross location is selected and segments are swapped.
        Returns:
            (crossed_members, crossed_used_method)
        """
        index_list = [i for i in range(2*self.pop_size)]
        selected_index_list = np.random.choice(index_list, size=2*int(self.pop_size * self.ratio["cross"]))
        i = 0
        crossed_members, crossed_used_method = [], []
        while i < len(selected_index_list):
            p1 = selected_index_list[i]%self.pop_size
            p2 = selected_index_list[i+1]%self.pop_size
            parent_1 = population.translit_members[p1]
            parent_2 = population.translit_members[p2]
            parent_1_used_method = population.used_method[p1]
            parent_2_used_method = population.used_method[p2]
            cross_location = random.randint(0, len(population.sample_slice)-1)
            # if either parent has a constant replacement on the crossing line, skip swapping
            if parent_2_used_method[cross_location]['const'] > 0 or parent_1_used_method[cross_location]['const'] > 0:
                crossed_members.append(copy.deepcopy(parent_1))
                crossed_used_method.append(parent_1_used_method[:-1])
            else:
                # build new child by concatenating pre- and post- segments from both parents
                crossed_members.append(copy.deepcopy(parent_1[:cross_location] + parent_2[cross_location:]))
                crossed_used_method.append(copy.deepcopy(parent_1_used_method[:cross_location] + parent_2_used_method[cross_location:-1]))
                # Reconcile variable renames that might conflict after crossover:
                del_used_method = copy.deepcopy(parent_1_used_method[cross_location:-1] + parent_2_used_method[:cross_location])
                var_lis = []
                for x in parent_1_used_method[:cross_location]:
                    for var, word in x["var"]:
                        var_lis.append(var)
                for idx, x in enumerate(parent_2_used_method[cross_location:-1]):
                    for var, word in x["var"]:
                        if var in var_lis:
                            del_used_method.append({"var": [(var, word)]})
                            crossed_used_method[-1][cross_location + idx]["var"].remove((var, word))
                            crossed_used_method[-1][cross_location + idx]["used"].remove(word)
                # Revert renames from del_used_method in the crossed member
                for x in del_used_method:
                    for var, word in x["var"]:
                        for pos1 in range(len(crossed_members[-1])):
                            for pos2 in range(len(crossed_members[-1][pos1])):
                                crossed_members[-1][pos1][pos2] = self.var_replace_word(crossed_members[-1][pos1][pos2], word, var)
                # Apply renames present in the kept used_method of the child member
                for x in crossed_used_method[-1]:
                    for var, word in x["var"]:
                        for pos1 in range(len(crossed_members[-1])):
                            for pos2 in range(len(crossed_members[-1][pos1])):
                                crossed_members[-1][pos1][pos2] = self.var_replace_word(crossed_members[-1][pos1][pos2], var, word)
            # collect used macro tokens into the final set for this child
            used_macro = set()
            for x in crossed_used_method[-1]:
                used_macro = used_macro.union(x["used"])
            crossed_used_method[-1].append(copy.deepcopy(used_macro))
            i += 2
        return crossed_members, crossed_used_method

    def mutate(self, population):
        """
        Apply random mutations to a subset of population members.
        Each selected member gets a random candidate operation applied at a random line.
        """
        index_list = [i for i in range(self.pop_size)]
        selected_index_list = np.random.choice(index_list, size=int(self.pop_size * self.ratio["mutate"]))
        n = len(population.sample_slice)
        for i in selected_index_list:
            pre_change_times = self.get_limits(population.used_method[i])
            try_times = 0
            while pre_change_times == self.get_limits(population.used_method[i]) and try_times < 5:
                line = random.randint(0, n-1)

                if population.line_replaced[line] == []:
                    try_times += 1
                    continue

                candidate_info = random.choice(population.line_replaced[line])
                used_macro_all = population.used_method[i][-1]
                translit_member_tmp = copy.deepcopy(population.translit_members[i])
                used_method_tmp = copy.deepcopy(population.used_method[i][line])
                population_member, update_used_method = self.create_population_member(translit_member_tmp, used_method_tmp, candidate_info, line, population.macro_names, used_macro_all)
                population.translit_members[i] = population_member
                population.used_method[i][-1] = population.used_method[i][-1].union(update_used_method["used"])
                population.used_method[i][line] = update_used_method
                try_times += 1

    def run(self, sample_slice, ori_label, index, method_dict):
        """
        Execute the genetic algorithm for a single sample.
        Parameters:
            sample_slice: the program lines.
            ori_label: original label.
            index: sample id.
            method_dict: dictionary of boolean flags controlling which operations are allowed.
        Returns:
            (is_successful, best_result, best_label, best_score, modify_count)
        """
        self.check_model_num = 0
        population = Population(self.config, sample_slice, index, ori_label)
        # Initialize population and perform initial fitness evaluation
        self.initial_population(population, index, method_dict)
        label = population.predict
        prob = population.fitness_score
        if label != ori_label:
            return True, population.result, label, prob, self.get_limits(population.used_method_result)
        for _ in range(self.max_iters):
            selected_members, selected_used_method = self.select(population)
            crossed_members, crossed_used_method = self.cross(population)
            population.translit_members = selected_members + crossed_members
            population.used_method = selected_used_method + crossed_used_method
            # Apply mutation in-place
            self.mutate(population)
            # Convert translit representation back to flat statements for evaluation
            population.members = []
            for translit_member in population.translit_members:
                population.members.append(self.list_to_statement(translit_member))
            # Enforce limits and evaluate fitness
            self.check_limits(population)
            dataloader = self.make_batch(population.members, self.batch_size)
            population.predicts, population.fitness_scores = self.fitness(dataloader, population.ori_label, index)
            self.check_model_num += len(population.members)
            # Update best member information
            state_score_arr = np.array(population.fitness_scores)
            sort_index = np.argsort(-state_score_arr)
            population.predict = population.predicts[sort_index[0]]
            population.fitness_score = population.fitness_scores[sort_index[0]]
            population.result = copy.deepcopy(population.members[sort_index[0]])
            population.translit_result = copy.deepcopy(population.translit_members[sort_index[0]])
            population.used_method_result = copy.deepcopy(population.used_method[sort_index[0]])
            label = population.predict
            prob = population.fitness_score
            if label != ori_label:
                return True, population.result, label, prob, self.get_limits(population.used_method_result)
        # Return best found after exhausting iterations
        label = population.predict
        prob = population.fitness_score
        return False, population.result, label, prob, self.get_limits(population.used_method_result)


class Genetic_Big_Vul():
    """
    Orchestrator class that runs the genetic attack across multiple program indices
    and writes results to a CSV file.
    Parameters:
        config: global configuration object.
        limits: allowed number of modifications.
        programs_indexes: list of program indices to attack.
        test_data: pandas DataFrame used for build reference set.
        batch_size: batch size of evaluation.
        model_name: name of the model.
    """
    def __init__(self, config, limits, programs_indexes, test_data, batch_size, model_name):
        self.config = config
        self.programs_indexes = programs_indexes
        self.load_temp_files()
        self.genetic_algorithm = GeneticAlgorithm(config, limits, programs_indexes, test_data, batch_size, model_name=model_name)
        self.model_name = model_name
        self.limits = limits

    def load_temp_files(self):
        """
       Load mapping from sample index to ground truth.
       """
        with open(os.path.join(self.config.temp_path, 'index_label.json'), 'r') as index_label_jsn:
            self.index_label_dict = json.load(index_label_jsn)

    def attack(self, file_name, method_dict):
        """
        Run the genetic attack across program indices and save results.
        Parameters:
            file_name: output CSV filename.
            method_dict: dictionary of boolean flags controlling enabled transformations.
        """
        random.seed(123456)
        np.random.seed(123456)
        count, success = 0, 0

        failed = []
        with open(os.path.join(self.config.result_path, file_name), 'w', encoding='utf-8', newline='') as r_csv_f:
            csv_writer = csv.writer(r_csv_f)
            csv_writer.writerow(['index', 'true_label', 'ori_label', 'ori_prob', 'ori_code', 'adv_label', 'adv_prob', 'adv_code', 'query_times', 'modify_times'])
            for index in self.programs_indexes:
                index = str(index)

                if index not in self.genetic_algorithm.index_add_dict or \
                    index not in self.genetic_algorithm.index_replace_dict or \
                    index not in self.genetic_algorithm.index_var_dict:
                    continue

                ground_label = int(self.index_label_dict[index])

                with open(os.path.join(self.config.sample_slice_path, index), 'r', encoding='cp1252') as slice_f:
                    sample_slice = slice_f.readlines()
                # Get original model prediction and similarity-based approximate probability
                ori_label, ori_prob = self.genetic_algorithm.predict_single(sample_slice, index)
                if ori_label != ground_label:
                    sample_slice = concat_statement(sample_slice)
                    csv_writer.writerow([index, ground_label, ori_label, ori_prob, sample_slice, ori_label, ori_prob, sample_slice, self.genetic_algorithm.check_model_num])
                    continue
                count += 1
                # Run GA-based attack for the sample
                is_success, adv_slice, adv_label, adv_prob, diff = self.genetic_algorithm.run(sample_slice, ori_label, index, method_dict)
                if is_success:
                    success += 1
                else:
                    failed.append(index)
                sample_slice = concat_statement(sample_slice)
                adv_slice = concat_statement(adv_slice)
                csv_writer.writerow([index, ground_label, ori_label, ori_prob, sample_slice, adv_label, adv_prob, adv_slice, self.genetic_algorithm.check_model_num, diff])
                print(f"success count {success}, total count {count}, AS {success / count}")
            # Write summary statistics and failed list at the end of the CSV
            csv_writer.writerow([success, count, success / count])
            csv_writer.writerow(failed)

