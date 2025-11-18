"""
Combination_Big_Vul.py

This module implements a greedy-based adversarial attack procedure for the "Big Vul" dataset.
"""
import copy
import json
import os
import csv
import random
import string
import re
import numpy as np
import pandas as pd
from tqdm import tqdm

from Attack.Obfuscation_Big_Vul import Obfuscation_Big_Vul
from Utils.get_tokens import create_tokens
from DataProcess.DataPipline_Big_Vul import DataPipline_Big_Vul
from Utils.nope_Big_Vul import NopeTool_Big_Vul
from Utils.Util import concat_statement

class Combination_Big_Vul():
    """
    Orchestrates combination attacks for the Big Vul dataset.
    Methods apply five transformations (add dead code, replace constants/macros/vars,
    unroll loops) and query a target model to find the best adversarial sample.
    """
    def __init__(self, config, replace_limit_number, sample_ids, batch_size, model_name):
        """
        Initialize attack coordinator.
        Args:
            config: configuration object.
            replace_limit_number: maximum number of modifications permitted per sample.
            sample_ids: sample ids to attack.
            batch_size: batch size of evaluation.
            model_name: name of the model.
        """
        self.config = config
        self.check_model_num = 0  # number of model queries made for the current sample
        self.replace_limit_number = replace_limit_number
        self.batch_size = batch_size
        self.sample_ids = sample_ids
        # load test data
        test_data = pd.read_pickle(config.root_path + '/data.pkl')
        self.obfuscation = Obfuscation_Big_Vul(config, replace_limit_number, sample_ids, test_data, model_name)
        # load mapping files
        self.load_temp_files()
        self.token_list = None
        self.best_token_dict = None
        self.best_line_dict = None
        # tools that performs masked-token impact checks
        self.nope_tool = NopeTool_Big_Vul(self.config, self.obfuscation)
        self.model_name = model_name

    def load_temp_files(self):
        """
        Load mapping JSON files and  create them if not exist.
        The files include:
            - index_add_dict.json: insertion candidates by line
            - index_var_dict.json: variable rename candidates by line
            - index_replace_dict.json: replacement (constant) candidates by line
            - index_label.json: ground truth labels
        """
        # create/load add dictionary
        index_add_path = os.path.join(self.config.temp_path, "index_add_dict.json")
        if not os.path.exists(index_add_path):
            self.obfuscation.create_add_dict()
        with open(index_add_path, 'r') as index_add_dict_jsn:
            self.index_add_dict = json.load(index_add_dict_jsn)

        # create/load variable dictionary
        index_var_path = os.path.join(self.config.temp_path, 'index_var_dict.json')
        if not os.path.exists(index_var_path):
            self.obfuscation.create_add_dict()
        with open(index_var_path, 'r') as index_add_dict_jsn:
            self.index_var_dict = json.load(index_add_dict_jsn)

        # create/load replace dictionary
        index_replace_path = os.path.join(self.config.temp_path, "index_replace_dict.json")
        if not os.path.exists(index_replace_path):
            self.obfuscation.create_replace_dict()
        with open(index_replace_path, 'r') as index_replace_dict_jsn:
            self.index_replace_dict = json.load(index_replace_dict_jsn)

        # load index to label mapping
        with open(os.path.join(self.config.temp_path, "index_label.json"), 'r') as index_label_jsn:
            self.index_label_dict = json.load(index_label_jsn)

    def get_best_adv(self, adv_slice_list, ori_label, id):
        """
        Evaluate a list of candidate adversarial program slices and return the best one.
        The best candidate is selected by approximate probability or label change:
            - If a candidate changes the label (pred != ori_label) it gets maximum score 1.
            - Otherwise its score is the approximate probability.
        Args:
            adv_slice_list: list of candidate program slices in list-of-lists format.
            ori_label: original label.
            id: sample id.
        Returns:
            tuple(best_adv_slice, best_pred_label, best_pred_prob) or (None, None, None)
            if adv_slice_list is empty.
        """
        if not adv_slice_list:
            return None, None, None
        adv_slice_list_trans = []
        # convert each list-of-statements representation into the flattened statement list
        for adv in adv_slice_list:
            adv_slice_list_trans.append(self.list_to_statement(adv))
        # make batch and query the model for predictions
        dataloader = self.obfuscation.make_batch(adv_slice_list_trans, self.batch_size)
        self.check_model_num += len(adv_slice_list_trans)
        preds, probs = self.obfuscation.run_model_by_batch(dataloader, id)
        # compute score: 1 if prediction differs from original label, else use approximate probability
        adv_score_list = []
        i = 0
        while i < len(preds):
            label = preds[i]
            score = probs[i]
            if label != ori_label:
                state_score = 1
            else:
                state_score = score
            adv_score_list.append(state_score)
            i += 1
        # select the candidate with the highest score
        state_score_arr = np.array(adv_score_list)
        sort_index = np.argsort(-state_score_arr)
        return adv_slice_list[sort_index[0]], preds[sort_index[0]], probs[sort_index[0]]

    def keep_leading_whitespace(self, s):
        """
        Return the leading whitespace characters for given string.
        """
        match = re.match(r'^\s*', s)
        return match.group() if match else ''

    def add_dead_code(self, sample_slice, line, variable_declaration_list, ori_label, best_word, id,
                      method=1):
        """
        Insert dead-code statements at a specified line.
        Strategies:
            method == 1: inject candidate tokens computed by token importance.
            method == 2: inject random uppercase tokens.
            else: use provided best_word only.
        The function generates various candidate slices with inserted statements and picks
        the best one.
        Args:
            sample_slice: list-of-lists representation of the program.
            line: target line index.
            variable_declaration_list: list of possible variable declarations for insertion.
            ori_label: original label.
            best_word: fallback word to use for insertion (when method != 1,2).
            id: sample id.
            method: insertion method as described above.
        Returns:
            tuple(best_adv, adv_label, adv_prob) as returned by get_best_adv.
        """
        if method == 1:
            best_words = self.get_candidates_by_token_importance(sample_slice, ori_label, need_clear = False)
        elif method == 2:
            best_words = [((''.join(random.choice(string.ascii_uppercase) for i in range(5))), 0) for cnt in range(5)]
        else:
            best_words = [best_word]

        adv_slice_list = []
        # For each variable declaration possibility, create insertions using best_words
        for variable_declaration in variable_declaration_list:
            for b in best_words:
                best_word = b[0]
                insert_statement_dead_code = self.obfuscation.insert_statement0(variable_declaration, best_word)
                # preserve the original leading/trailing content of the line while inserting
                insert_statement_dead_code = [sample_slice[line][0].strip('\n')] + insert_statement_dead_code + sample_slice[line][1:] + ['\n' if '\n' in sample_slice[line][0] else '']
                adv_slice = sample_slice[:line] + [insert_statement_dead_code] + sample_slice[line + 1:]
                adv_slice_list.append(adv_slice)

        # # If there are fewer than 3 variable_declaration_list entries, try another insertions
        if len(variable_declaration_list) < 3:
           best_words = self.get_candidates_by_token_importance(sample_slice, ori_label, need_clear = False)
           for b in best_words:
                best_word = b[0]
                insert_statement_dead_code = ['printf(\" ' + best_word + '\");']
                insert_statement_dead_code = [sample_slice[line][0].strip('\n')] + insert_statement_dead_code + sample_slice[line][1:] + ['\n' if '\n' in sample_slice[line][0] else '']
                adv_slice = sample_slice[:line] + [insert_statement_dead_code] + sample_slice[line + 1:]
                adv_slice_list.append(adv_slice)
        # evaluate candidates and return the best
        return self.get_best_adv(adv_slice_list, ori_label, id)

    def const_replace(self, sample_slice, line, replace_content, method, ori_label, id):
        """
        Replace constant content in a given line using token candidates.
        Args:
            sample_slice: list-of-lists representation of the program.
            line: target line index.
            replace_content: object containing the replacement template.
            method: if 1, use importance-based candidates; otherwise use random tokens.
            ori_label: original label.
            id: sample id.
        Returns:
            tuple(best_adv, adv_label, adv_prob) as returned by get_best_adv.
        """
        if method == 1:
            candidates = self.get_candidates_by_token_importance(sample_slice, ori_label)
        else:
            candidates = [((''.join(random.choice(string.ascii_uppercase) for i in range(5))), 0) for cnt in range(5)]

        adv_slice_list = []
        # compute how many placeholders are present (each '<unk>' * 10 counted)
        cnt_var = replace_content[0].count('<unk>') // 10
        # iterate through candidate tokens and fill placeholders sequentially
        for i in range(len(candidates) - cnt_var + 1):
            var_replace_content = copy.deepcopy(replace_content)
            for j in range(cnt_var):
                var_replace_content[0] = var_replace_content[0].replace('<unk>' * 10, candidates[i + j][0], 1)
                var_replace_content[1] = var_replace_content[1].replace('<unk>' * 10, candidates[i + j][0], 1)
            # preserve original leading whitespace for the line
            head = self.keep_leading_whitespace(sample_slice[line][0])
            new_replace_content = [head, var_replace_content[0], var_replace_content[1]]
            adv_slice = sample_slice[0:line] + [new_replace_content + sample_slice[line][1:] + ['\n' if '\n' in sample_slice[line][0] else '']] + sample_slice[line + 1:]
            adv_slice_list.append(adv_slice)
        # pick the best candidate via model queries
        return self.get_best_adv(adv_slice_list, ori_label, id)

    def token_impact(self, ori_label, s_lengths, token_symbol_sequence, id):
        """
        Measure the impact of each token by blanking it out and querying the model.
        For each token index, construct a program where that single token is removed
        (replaced with an empty string) and query the model. Returns the ordering of
        indices by their impact and the corresponding approximate probabilities.
        Args:
            ori_label: original label.
            s_lengths: list of statement token lengths.
            token_symbol_sequence: flattened list of tokens.
            id: sample id.
        Returns:
            (sort_index, probs_sorted_by_increasing_impact)
            sort_index: numpy array of indices sorted by increasing approximate probability.
            probs: list of approximate probabilities corresponding to that sorted order.
        """
        map_programs = []
        # For each token, create a variant of the program with that token blanked out
        for token_index in range(len(token_symbol_sequence)):
            tmp_token_list = []
            for i in range(len(token_symbol_sequence)):
                if i != token_index:
                    tmp_token_list.append(token_symbol_sequence[i])
                else:
                    tmp_token_list.append("")
            tmp_map_program = []
            start = 0
            # reassemble flattened tokens into statements using s_lengths
            for l in s_lengths:
                tmp_map_program.append(" ".join(tmp_token_list[start:start + l]))
                start += l
            map_programs.append(tmp_map_program)
        # batch and query the model via the tool
        dataloader = self.obfuscation.make_batch(map_programs, self.batch_size)
        preds, probs = self.nope_tool.run_model_by_batch(dataloader, id)
        # return indices sorted by approximate probability, and the list of approximate probabilities in that order
        state_score_arr = np.array(probs)
        sort_index = np.argsort(state_score_arr)
        p = []
        for i in sort_index:
            p.append(probs[i])
        return sort_index, p

    def get_candidates_by_token_importance(self, sample_slice, ori_label, need_clear = True):
        """
        Return a list of candidate tokens sorted by token-importance for substitution.
        Filtering rules include:
            - skip tokens that are short (<3 characters)
            - skip tokens that appear in the current sample
            - skip C/C++ reserved keywords in an avoid list and tokens that are not alphanumeric/underscore
                when need_clear is True
        Args:
            sample_slice: list-of-lists representation of the program.
            ori_label: original label.
            need_clear: whether to apply strict filtering.
        Returns:
            list of lists like [[token1], [token2], ...]
        """
        sample_slice = self.list_to_statement(sample_slice)
        token_imp_path = os.path.join(self.config.temp_path, "token_importance_" + self.model_name + ".json")
        # Build token importance cache if missing
        if not os.path.exists(token_imp_path):
            token_importance_dict = {0: {}, 1: {}}
            count_dict = {0: 0, 1: 0}
            count = 0
            for index in tqdm(self.sample_ids, total=len(self.sample_ids)):
                count += 1

                with open(os.path.join(self.config.sample_slice_path, str(index)), 'r') as f:
                    slice = f.readlines()

                ori_label_new, ori_prob = self.obfuscation.predict_adv_program(slice, index)

                # skip overly long samples or if we already have enough samples for this label
                if len(slice) > 150 or count_dict[ori_label_new] >= 250:
                    continue

                count_dict[ori_label_new] += 1

                token_symbol_sequence = []
                # build flattened token list from all statements in the sample
                [token_symbol_sequence.extend(create_tokens(statement)) for statement in slice]
                _, s_lengths = DataPipline_Big_Vul.states2idseq(slice, self.config.vocab, self.config.vocab_size - 1)
                sort_index, probs = self.token_impact(ori_label_new, s_lengths, token_symbol_sequence, index)
                # accumulate importance values for tokens; the token's importance is max(prob - ori_prob)
                for pos in range(len(sort_index)):
                    i = sort_index[pos]
                    prob = probs[pos]
                    tok = token_symbol_sequence[i]
                    if tok in token_importance_dict[ori_label_new].keys():
                        token_importance_dict[ori_label_new][tok] = max(
                            token_importance_dict[ori_label_new][tok], prob - ori_prob)
                    else:
                        token_importance_dict[ori_label_new][tok] = prob - ori_prob
            with open(os.path.join(self.config.temp_path, "token_importance_" + self.model_name + ".json"),
                      'w') as token_importance_jsn:
                json.dump(token_importance_dict, token_importance_jsn)
        if self.token_list is None:
            with open(token_imp_path, 'r') as token_importance_jsn:
                token_importance_dict = json.load(token_importance_jsn)
            self.token_list = token_importance_dict
        token_importance_dict = self.token_list
        curr_tokens = []
        # collect tokens that appear in current sample, so we can avoid reusing them
        for sentence in sample_slice:
            curr_tokens.extend(create_tokens(sentence))
        tmp = token_importance_dict[str(1 - ori_label)]
        tmp_list = []
        for token in tmp.keys():
            tmp_list.append([token, tmp[token]])
        # sort tokens by decreasing importance value
        tmp_list.sort(key=lambda x: x[1], reverse=True)
        ret = []
        count = 5

        def is_alnum_underscore(string):
            return bool(re.match(r'^[\w]+$', string))

        # C/C++ reserved tokens
        avoid_list = [
            'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor', 'bool', 'break',
            'case', 'catch', 'char', 'char8_t', 'char16_t', 'char32_t', 'class', 'compl', 'const',
            'constexpr', 'const_cast', 'continue', 'co_await', 'co_return', 'co_yield', 'decltype', 'default', 'delete',
            'do', 'NULL', 'null', 'int64',
            'double', 'dynamic_cast', 'else', 'enum', 'explicit', 'export', 'extern', 'false', 'float',
            'for', 'friend', 'goto', 'if', 'inline', 'int', 'long', 'mutable', 'namespace', 'new', 'noexcept',
            'not', 'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected', 'public',
            'register', 'reinterpret_cast', 'requires', 'return', 'short', 'signed', 'sizeof',
            'static', 'static_assert', 'static_cast', 'struct', 'switch', 'template', 'this', 'thread_local',
            'throw', 'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using',
            'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq'
        ]

        for t in tmp_list:
            if t[0] in curr_tokens or t[0].upper() in curr_tokens:
                continue
            if need_clear and (not is_alnum_underscore(t[0]) or t[0] in avoid_list or t[0].upper() in avoid_list or t[0].isdigit()):
                continue
            if len(t[0]) < 3:
                continue
            ret.append([t[0]])
            count -= 1
            if count <= 0:
                break
        return ret

    def macro_replace(self, sample_slice, line, ori_label, id, method=1):
        """
        Try macro replacements on tokens within a single line.
        For every token position on the line, attempt to substitute with candidate tokens
        and evaluate the resulting programs.
        Args:
            sample_slice: list-of-lists representation of the program.
            line: target line index.
            ori_label: original label.
            id: sample id.
            method: if 1 use importance-based tokens, otherwise random tokens.
        Returns:
            the best candidate (adv_slice, adv_label, adv_prob) via get_best_adv. If the given
            line is out of range or invalid, returns the original sample_slice directly.
        """
        if method == 1:
            candidates = self.get_candidates_by_token_importance(sample_slice, ori_label)
        else:
            candidates = [((''.join(random.choice(string.ascii_uppercase) for i in range(5))), 0) for cnt in range(5)]
        # Ensure line exists and has tokens
        if line >= len(sample_slice) or len(sample_slice[line]) < 1:
            return sample_slice
        # tokenize the primary code string at the requested line (ignore '\n' tokens)
        s_split = create_tokens(sample_slice[line][0])
        s_split = [t for t in s_split if t != '\n']
        res = sample_slice[line][1:]
        adv_slice_list = []
        # try replacing each token position with each candidate token
        for l in range(len(s_split)):
            for candidate in candidates:
                head = self.keep_leading_whitespace(sample_slice[line][0])
                macro_replace_line = s_split[:l] + [candidate[0]] + s_split[l + 1:]
                adv_slice = sample_slice[0:line] + [[head] + [" ".join(macro_replace_line)] + res + ['\n' if '\n' in sample_slice[line][0] else '']] + sample_slice[line + 1:]
                adv_slice_list.append(adv_slice)

        return self.get_best_adv(adv_slice_list, ori_label, id)

    def var_replace_word(self, text, target_word, replacement):
        """
        Replace occurrences of target_word in text.
        """
        pattern = r'\b' + re.escape(target_word) + r'\b'
        result = re.sub(pattern, replacement, text)
        return result

    def var_replace(self, sample_slice, variable_list, ori_label, best_word, id, method=1):
        """
        Perform variable renaming across the whole sample, for each variable in variable_list.
        Args:
            sample_slice: list-of-lists representation of the program.
            variable_list: list of variable names to try renaming.
            ori_label: original label.
            best_word: fallback token when method != 1,2.
            id: sample id.
            method: if 1 use token-importance candidates, if 2 use random uppercase tokens, else use best_word.
        Returns:
            the best candidate (adv_slice, adv_label, adv_prob) via get_best_adv, or None triple if none.
        """
        if method == 1:
            best_words = self.get_candidates_by_token_importance(sample_slice, ori_label)
        elif method == 2:
            best_words = [((''.join(random.choice(string.ascii_uppercase) for i in range(5))), 0) for cnt in range(5)]
        else:
            best_words = [best_word]

        adv_slice_list = []
        for var in variable_list:
            for b in best_words:
                best_word = b[0]
                replace = False
                adv_slice = copy.deepcopy(sample_slice)
                for i in range(len(adv_slice)):
                    for j in range(len(adv_slice[i])):
                        r = self.var_replace_word(copy.deepcopy(adv_slice[i][j]), var, best_word)
                        if r != adv_slice[i][j]:
                            adv_slice[i][j] = r
                            replace = True
                if not replace:
                    continue
                adv_slice_list.append(adv_slice)
        return self.get_best_adv(adv_slice_list, ori_label, id)

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

    def combination_attack(self, save_name, add_tag=False, const_tag=False, macro_tag=False,
                           unroll_loop=False, var_tag=False, random_tag=False):
        """
        Run the combination attack across the configured samples and save results.
        This method attempts different transformations in a prioritized order on each sample:
            - add dead code (if add_tag)
            - constant replacement (if const_tag)
            - macro replacement (if macro_tag)
            - loop unrolling (if unroll_loop)
            - variable renaming (if var_tag)
        Each sample is limited by replace_limit_number total modifications.
        Args:
            save_name: CSV filename to write results.
            add_tag, const_tag, macro_tag, unroll_loop, var_tag: booleans to enable each transformation.
            random_tag: if True, use random candidate tokens instead of token-importance-based ones.
        """
        count = 0
        success = 0
        method = 2 if random_tag else 1
        failed = []

        with open(os.path.join(self.config.result_path, save_name), 'w', encoding='utf-8', newline='') as r_csv_f:
            csv_writer = csv.writer(r_csv_f)
            csv_writer.writerow(['index', 'true_label', 'ori_label', 'ori_prob', 'ori_code', 'adv_label', 'adv_prob', 'adv_code', 'query_times', 'modify_times', 'macro_times', 'dif_lis'])
            for index in self.sample_ids:
                index = str(index)
                self.check_model_num = 0
                is_success = False
                diff = 0
                dif_lis = []
                macro = 0
                try:
                    ground_label = int(self.index_label_dict[index])
                    add_dict = self.index_add_dict[index]
                    replace_dict = self.index_replace_dict[index]
                    var_dict = self.index_var_dict[index]
                except Exception as e:
                    continue

                # read sample and skip overly long ones
                with open(os.path.join(self.config.sample_slice_path, index), 'r', encoding='cp1252') as sample_slice_f:
                    sample_slice_ori = sample_slice_f.readlines()
                if len(sample_slice_ori) > 150:
                    continue
                # work in list-of-lists representation
                sample_slice = self.statement_to_list(sample_slice_ori)
                # initial prediction and its approximate probability
                ori_label, ori_prob = self.obfuscation.predict_adv_program(sample_slice_ori, index)

                save_ori_prob = ori_prob
                self.check_model_num += 1
                if ori_label != ground_label:
                    sample_slice_ori = concat_statement(sample_slice_ori)
                    csv_writer.writerow(
                        [index, ground_label, ori_label, ori_prob, sample_slice_ori, ori_label, ori_prob,
                         sample_slice_ori, self.check_model_num, diff, macro, dif_lis])
                    continue
                count += 1
                if not random_tag:
                    # compute a line ordering by importance
                    sorted_lines = self.obfuscation.statement_impact(sample_slice_ori, ori_label, ori_prob, index)
                else:
                    # if random_tag is set, use a deterministic random permutation for line order
                    lines_number = [i for i in range(len(sample_slice_ori))]
                    random.seed(123456)
                    np.random.seed(123456)
                    sorted_lines = np.random.choice(lines_number, size=len(sample_slice_ori))
                try:
                    for important_line in sorted_lines:
                        if diff >= self.replace_limit_number:
                            break
                        best_word = sample_slice_ori[sorted_lines[-1]]
                        candidate_adv = []
                        # try adding dead code
                        if not is_success and str(
                                important_line - 1) in add_dict.keys() and diff < self.replace_limit_number and add_tag:
                            variable_declaration_list = []
                            if important_line > 0:
                                variable_declaration_list = add_dict[str(important_line - 1)]
                            adv, adv_label, adv_prob = self.add_dead_code(sample_slice, important_line, variable_declaration_list, ori_label,
                                                     best_word, int(index), method=method)
                            if adv_label != ground_label:
                                sample_slice = adv
                                is_success = True
                                diff += 1
                                dif_lis.append(f"add {important_line}")
                            else:
                                candidate_adv.append((adv_prob, adv, f"add {important_line}"))
                        # try constant replacement
                        if not is_success and str(
                                important_line) in replace_dict.keys() and diff < self.replace_limit_number and const_tag:
                            replace_content = replace_dict[str(important_line)]
                            adv, adv_label, adv_prob = self.const_replace(sample_slice, important_line, replace_content, method, ori_label, index)
                            if adv:
                                if adv_label != ground_label:
                                    sample_slice = adv
                                    is_success = True
                                    diff += 1
                                    dif_lis.append(f"const {important_line}")
                                else:
                                    candidate_adv.append((adv_prob, adv, f"const {important_line}"))
                        # try macro replacement
                        if macro_tag and not is_success and diff < self.replace_limit_number:
                            adv, adv_label, adv_prob = self.macro_replace(sample_slice, important_line, ori_label, index,
                                                     method=method)
                            if adv:
                                if adv_label != ground_label:
                                    sample_slice = adv
                                    is_success = True
                                    diff += 1
                                    macro += 1
                                    dif_lis.append(f"macro {important_line}")
                                else:
                                    candidate_adv.append((adv_prob, adv, f"macro {important_line}"))
                        # try loop unrolling transformation
                        if unroll_loop and not is_success and diff < self.replace_limit_number:
                            adv, adv_label, adv_prob = self.unroll_loop(sample_slice, important_line, ori_label, index, method)
                            if adv != sample_slice:
                                if adv_label != ground_label:
                                    sample_slice = adv
                                    is_success = True
                                    diff += 1
                                    dif_lis.append(f"unroll {important_line}")
                                else:
                                    candidate_adv.append((adv_prob, adv, f"unroll {important_line}"))
                        # try variable renaming
                        if not is_success and str(important_line) in var_dict.keys() and diff < self.replace_limit_number and var_tag:
                            var_list = var_dict[str(important_line)]
                            if var_list != []:
                                adv, adv_label, adv_prob = self.var_replace(sample_slice, var_list, ori_label, best_word, int(index), method=method)
                                if adv:
                                    if adv_label != ground_label:
                                        sample_slice = adv
                                        is_success = True
                                        diff += 1
                                        dif_lis.append(f"var {important_line}")
                                    else:
                                        candidate_adv.append((adv_prob, adv, f"var {important_line}"))
                        if is_success:
                            break
                        # choose the best candidate among tried transformations for this line,
                        # if any improves the approximate probability
                        candidate_adv = sorted(candidate_adv, key=lambda x: x[0], reverse=True)
                        if candidate_adv and candidate_adv[0][0] > ori_prob:
                            ori_prob = candidate_adv[0][0]
                            sample_slice = candidate_adv[0][1]
                            diff += 1
                            dif_lis.append(candidate_adv[0][2])
                    # record success/failure counts
                    if is_success:
                        success += 1
                    else:
                        failed.append(index)
                    adv_tans = self.list_to_statement(sample_slice)
                    adv_prob = self.obfuscation.Compare(adv_tans, index)
                    sample_slice_ori = concat_statement(sample_slice_ori)
                    adv_tans = concat_statement(adv_tans)
                    csv_writer.writerow(
                        [index, ground_label, ori_label, save_ori_prob, sample_slice_ori, adv_label, adv_prob, adv_tans,
                         self.check_model_num, diff, macro, dif_lis])
                    print(f"success count {success}, total count {count}, AS {success / count}")
                except Exception as e:
                    print(f"Error {index}")
                    print(e)
            # summary statistics at end of file
            csv_writer.writerow([success, count, success / count])
            csv_writer.writerow(failed)

    def unroll_loop(self, sample_slice, important_line, ori_label, id, method):
        """
        Attempt to convert one form of a loop into another equivalent form.
        Args:
            sample_slice: list-of-lists representation of the program.
            important_line: target line index.
            ori_label: original label.
            id: sample id.
            method: if 1 use token-importance derived flags, otherwise random flags.
        Returns:
            tuple(best_adv_slice_or_original, adv_label, adv_prob)
            If the line does not contain a loop or no valid transformation is possible,
            returns the original sample_slice and two Nones.
        """
        if method == 1:
            flags = self.get_candidates_by_token_importance(sample_slice, ori_label)
        else:
            flags = [((''.join(random.choice(string.ascii_uppercase) for i in range(5))), 0) for cnt in range(5)]
        if sample_slice[important_line] == []:
            return sample_slice, None, None
        line = sample_slice[important_line][0]
        res = sample_slice[important_line][1:]
        line_tokens = create_tokens(line)
        adv_lis = []
        # handle while(...) constructs
        if "while" in line_tokens:
            # extract the parenthesized expression inside while(...)
            r1 = re.compile(r'[(](.*)[)]', re.S)
            exp = re.findall(r1, line)
            if exp == []:
                return sample_slice, None, None
            for flag in flags:
                flag = flag[0]
                head = self.keep_leading_whitespace(line)
                # create a flag variable and reframe the while loop to use it
                statements = ["bool " + flag + " =true;", "while(" + flag + "){"]
                statements1 = ["if(!(" + exp[0] + "))" + flag + "=false;"]
                # if original line lacked '{' but the following line contains '{', preserve structure
                if '{' not in line_tokens and '{' in sample_slice[important_line + 1][0]:
                    adv = (sample_slice[:important_line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line else '']] +
                           [[sample_slice[important_line + 1][0].replace('{', '', 1)] + sample_slice[important_line + 1][1:]] + sample_slice[
                                                                                            important_line + 2:])
                else:
                    adv = sample_slice[:important_line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line else '']] + sample_slice[
                                                                                             important_line + 1:]
                adv_lis.append(adv)
            return self.get_best_adv(adv_lis, ori_label, id)
        # handle for(init; cond; step) constructs
        elif "for" in line_tokens:
            r1 = re.compile(r'[(](.*)[)]', re.S)
            exp = re.findall(r1, line)
            if exp == []:
                return sample_slice, None, None
            i_s = exp[0].split(';')
            if len(i_s) != 3:
                # not a standard for(init;cond;step) format we can handle
                return sample_slice, None, None
            for flag in flags:
                flag = flag[0]
                statements = ["bool " + flag + " =true;", "for(" + i_s[0] + ';' + flag + ';' + i_s[2] + "){"]
                statements1 = ["if(!(" + i_s[1] + "))" + flag + "=false;"]
                head = self.keep_leading_whitespace(line)
                if '{' not in line_tokens and '{' in sample_slice[important_line + 1][0]:
                    adv = (sample_slice[:important_line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line else '']] +
                           [[sample_slice[important_line + 1][0].replace('{', '', 1)] + sample_slice[important_line + 1][1:]] + sample_slice[
                                                                                            important_line + 2:])
                else:
                    adv = sample_slice[:important_line] + [[head] + statements + statements1 + res + ['\n' if '\n' in line else '']] + sample_slice[
                                                                                             important_line + 1:]
                adv_lis.append(adv)
            return self.get_best_adv(adv_lis, ori_label, id)
        else:
            # if no loop detected, return original unchanged
            return sample_slice, None, None
