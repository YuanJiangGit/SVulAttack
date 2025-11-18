# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/16 15:40
# @Function:
import json
import os
import random
import re
import string

from CParser.ParserVisitor import ParserVisitor
from CParser.cGrammer.CLexer import CLexer
from CParser.cGrammer.CParser import CParser
from antlr4 import FileStream, CommonTokenStream, InputStream

from tqdm import tqdm

class ParseAndMutCode_Big_Vul:
    def parse_statement(self, statement, file_name):
        inputs = InputStream(statement)
        lexer = CLexer(inputs)
        stream = CommonTokenStream(lexer)
        parser = CParser(stream)
        tree = parser.compilationUnit()
        mv = ParserVisitor()
        mv.visit(tree)

    def translate_c_replace_const(self, sample_source_path, temp_path, tid = '0', filename = 'filename_replace_const.json'):
        problem_ids = []
        count = 0
        filename_replace_const_dict = {}
        for id in os.listdir(sample_source_path):
            print(id, count)
            if id != tid:
                continue
            count += 1
            files = os.listdir(os.path.join(sample_source_path, id))
            if id in problem_ids:
                continue
            for file in tqdm(files, total=len(files)):
                if not(file.endswith('.c') or file.endswith('.cpp')):
                    continue

                file_name = os.path.join(sample_source_path, id, file)
                with open(file_name, 'r') as f:
                    code_content = f.readlines()
                result = ''
                for statement in code_content:
                    result += statement

                try:
                    inputs = InputStream(result)
                    lexer = CLexer(inputs)
                    lexer.removeErrorListeners()
                    stream = CommonTokenStream(lexer)
                    parser = CParser(stream)
                    parser.removeErrorListeners()
                    tree = parser.compilationUnit()
                    mv = ParserVisitor()
                    mv.visit(tree)
                except Exception as e:
                    continue

                line_replace_dict = {}
                for origin_line in range(len(code_content)):
                    if origin_line+1 not in mv.const_int_dict.keys() and origin_line+1 not in mv.const_string_dict.keys():
                        continue
                    new_code = code_content[origin_line].split('/*')[0]
                    declaration_line1 = ''
                    if origin_line + 1 in mv.const_string_dict.keys() and mv.const_string_dict[origin_line+1] != []:
                        declaration_line1 = 'const char* '
                        for const in mv.const_string_dict[origin_line+1]:
                            new_const_name = ''.join(random.choice(string.ascii_uppercase) for i in range(5))
                            new_code = new_code.replace(const, new_const_name)
                            declaration_line1 = declaration_line1 + new_const_name + ' = ' + str(const) + ','
                        declaration_line1 = declaration_line1[:-1] + ';'

                    declaration_line = ''
                    if origin_line+1 in mv.const_int_dict.keys() and mv.const_int_dict[origin_line+1] != []:
                        declaration_line = 'const int '
                        for const in mv.const_int_dict[origin_line+1]:
                            new_const_name = ''.join(random.choice(string.ascii_uppercase) for i in range(5))
                            new_code = new_code.replace(const, new_const_name)
                            declaration_line = declaration_line + new_const_name + ' = ' + str(const) + ','
                        declaration_line = declaration_line[:-1] + ';'

                    replace_content = []
                    if declaration_line!= '':
                        replace_content.append(declaration_line.strip())
                    if declaration_line1!='':
                        replace_content.append(declaration_line1.strip())
                    replace_content.append(new_code.strip())
                    line_replace_dict[origin_line] = replace_content
                key = id + '/' + file
                filename_replace_const_dict[key] = line_replace_dict

        filename_replace_const_path = os.path.join(temp_path, filename)
        with open(filename_replace_const_path, 'w') as filename_replace_const_jsn:
            json.dump(filename_replace_const_dict, filename_replace_const_jsn)

    def translate_c_merge_function(self, sample_source_path, temp_path):
        problem_ids = []
        count = 0
        filename_merge_function_dict = {}
        for id in os.listdir(sample_source_path):
            print(id, count)
            count += 1
            files = os.listdir(os.path.join(sample_source_path, id))
            if id in problem_ids:
                continue
            func_definition_dict = {}
            line_input_space = {}
            line_output_space = {}
            for file in tqdm(files, total=len(files)):
                if not(file.endswith('.c') or file.endswith('.cpp')):
                    continue

                file_name = os.path.join(sample_source_path, id, file)
                with open(file_name, 'r') as f:
                    code_content = f.readlines()
                result = ''
                for statement in code_content:
                    result += statement

                try:
                    inputs = InputStream(result)
                    lexer = CLexer(inputs)
                    lexer.removeErrorListeners()
                    stream = CommonTokenStream(lexer)
                    parser = CParser(stream)
                    parser.removeErrorListeners()
                    tree = parser.compilationUnit()
                    mv = ParserVisitor()
                    mv.visit(tree)
                except Exception as e:
                    continue

                func_definition_dict[file] = mv.func_definition_dict
                line_input_space[file] = mv.line_input_space
                line_output_space[file] = mv.line_output_space

            for file in tqdm(files, total=len(files)):
                if not(file.endswith('.c') or file.endswith('.cpp')):
                    continue

                file_name = os.path.join(sample_source_path, id, file)
                with open(file_name, 'r') as f:
                    code_content = f.readlines()

                line_fucntion_dict = {}
                for origin_line in range(len(code_content)):
                    if origin_line+1 in line_input_space[file].keys():
                        for variable in line_input_space[file][origin_line+1]:
                            find = False
                            for defin_file in func_definition_dict.keys():
                                for func_definition_line in func_definition_dict[defin_file].keys():
                                    for func_name in func_definition_dict[defin_file][func_definition_line]:
                                        if func_name == variable:
                                            line_fucntion_dict[origin_line] = [func_definition_line-1, defin_file]
                                            find = True
                                            break
                                    if find:
                                        break
                                if find:
                                    break
                    if origin_line+1 in line_output_space[file].keys():
                        for variable in line_output_space[file][origin_line+1]:
                            find = False
                            for defin_file in func_definition_dict.keys():
                                for func_definition_line in func_definition_dict[defin_file].keys():
                                    for func_name in func_definition_dict[defin_file][func_definition_line]:
                                        if func_name == variable:
                                            line_fucntion_dict[origin_line] = [func_definition_line-1, defin_file]
                                            find = True
                                            break
                                    if find:
                                        break
                                if find:
                                    break
                key = id + '/' + file
                filename_merge_function_dict[key] = line_fucntion_dict
        filename_merge_function_path = os.path.join(temp_path, 'filename_merge_function.json')
        with open(filename_merge_function_path, 'w') as filename_merge_function_jsn:
            json.dump(filename_merge_function_dict, filename_merge_function_jsn)

    def translate_c(self, file_name):
        with open(file_name, 'r') as f:
            code_content=f.readlines()
        result = ''
        for statement in code_content:
            result += statement
        self.parse_statement(result, file_name)

    def parser_code_gadget(self,file_name):
        with open(file_name, 'r') as f:
            code_content = f.read()
        inputs = InputStream(code_content)
        lexer = CLexer(inputs)
        stream = CommonTokenStream(lexer)
        parser = CParser(stream)
        tree = parser.compilationUnit()
        mv = ParserVisitor()
        mv.visit(tree)
        for x in mv.prolog_list.keys():
            prolog = mv.prolog_list[x]
            prolog.toString()
        print(mv)

    def parse_the_slice_line(self, slice_file_name, code_position, slice_line, print=False):
        with open(slice_file_name, 'r') as f:
            slice_contet = f.readlines()
        if not print:
            code, code_line, down_code, down_line = '', -1, '', -1
            try:
                code, code_line = slice_contet.__getitem__(slice_line - 1).strip().rsplit(' ', 1)
                if slice_line< len(slice_contet):
                    down_code, down_line = (slice_contet.__getitem__(slice_line)).strip().rsplit(' ', 1)
            except ValueError:
                print("error")
            code_line = int(code_line)
            down_line = int(down_line)
            negative_keywords = ['for', 'while', 'dowhile', 'if', 'else', 'switch', 'case', 'default', 'continue', 'break', 'return']
            if any(s in code for s in negative_keywords) or any(s in down_code for s in negative_keywords):
                print("error")
                return '', -1, -1
            if down_line != code_line + 1:
                print("error")
                return '', -1, -1
            for root, ds, fs in os.walk(code_position):
                for f in fs:
                    fullname = os.path.join(root, f)
                    if fullname == slice_file_name or fullname.__contains__('adversival'):
                        continue
                    with open(fullname, 'r') as f:
                        code_content = f.readlines()
                    if code_line < len(code_content) and down_line < len(code_content):
                        temp1 = re.sub('[\s;+]', '', code_content.__getitem__(code_line-1))
                        temp2 = re.sub('[\s;+]', '', code)
                        temp3 = re.sub('[\s;+]', '', code_content.__getitem__(down_line-1))
                        temp4 = re.sub('[\s;+]', '', down_code)
                        if temp1 == temp2 and temp3 == temp4:
                            return fullname, code_line, down_line
            return '', -1, -1
        else:
            try:
                code, code_line = slice_contet.__getitem__(slice_line - 1).strip().rsplit(' ', 1)
                code_line = int(code_line)
            except ValueError:
                print("error")
            for root, ds, fs in os.walk(code_position):
                for f in fs:
                    fullname = os.path.join(root, f)
                    if fullname == slice_file_name or fullname.__contains__('adversival'):
                        continue
                    with open(fullname, 'r') as f:
                        code_content = f.readlines()
                    if code_line < len(code_content):
                        temp1 = re.sub('[\s;+]', '', code_content.__getitem__(code_line-1))
                        temp2 = re.sub('[\s;+]', '', code)
                        if temp1 == temp2:
                            return fullname, code_line
            return '', -1

    def translate_c_exchange_line(self, sample_source_path, temp_path, tid = '0', filename = 'filename_exchange_line.json'):
        problem_ids = ['']
        count = 0
        filename_exchange_line_dict = {}
        for id in os.listdir(sample_source_path):
            print(id, count)
            if id != tid:
                continue
            count += 1
            files = os.listdir(os.path.join(sample_source_path, id))
            if id in problem_ids:
                for file in files:
                    key = id + '/' + file
                    filename_exchange_line_dict[key] = {}
                continue
            for file in tqdm(files, total=len(files)):
                if not(file.endswith('.c') or file.endswith('.cpp')):
                    continue

                file_name = os.path.join(sample_source_path, id, file)
                with open(file_name, 'r') as f:
                    code_content = f.readlines()
                result = ''
                for statement in code_content:
                    result += statement

                try:
                    inputs = InputStream(result)
                    lexer = CLexer(inputs)
                    lexer.removeErrorListeners()
                    stream = CommonTokenStream(lexer)
                    parser = CParser(stream)
                    parser.removeErrorListeners()
                    tree = parser.compilationUnit()
                    mv = ParserVisitor()
                    mv.visit(tree)
                except Exception as e:
                    continue
                exchange_line_dict = {}

                for origin_line in range(result[0:result.find('{')].count('\n')+1, len(code_content)-1):
                    down_line = origin_line+1

                    negative_keywords = ['for', 'while', 'dowhile', 'if', 'else', 'switch', 'case', 'default',
                                         'continue', 'break', 'return', '{', '}', 'goto']

                    def cannot(s, origin_line):
                        if any(s in word for word in mv.line_input_space.get(origin_line, [])):
                            return False
                        if any(s in word for word in mv.line_output_space.get(origin_line, [])):
                            return False
                        if any(s in word for word in mv.const_string_dict.get(origin_line, [])):
                            return False
                        if any(s in word for word in mv.const_variable.get(origin_line, [])):
                            return False
                        return s in code_content[origin_line]


                    if any(cannot(s, origin_line) for s in negative_keywords) or any(cannot(s, down_line) for s in negative_keywords):
                        exchange_line_dict[origin_line] = -1
                        continue

                    origin_line_input_space = self.preprocess(mv.line_input_space.get(origin_line))
                    origin_line_output_space = self.preprocess(mv.line_output_space.get(origin_line))
                    down_line_input_space = self.preprocess(mv.line_input_space.get(down_line))
                    down_line_output_space = self.preprocess(mv.line_output_space.get(down_line))

                    if (set(origin_line_input_space) & set(down_line_output_space)) or (set(down_line_input_space) & set(origin_line_output_space)) or (
                            set(origin_line_output_space) & set(down_line_output_space)):
                        exchange_line_dict[origin_line] = -1
                        # continue
                    else:
                        exchange_line_dict[origin_line] = down_line
                key = id + '/' + file
                filename_exchange_line_dict[key] = exchange_line_dict

        filename_exchange_line_path = os.path.join(temp_path, filename)
        with open(filename_exchange_line_path, 'w') as filename_exchange_line_jsn:
            json.dump(filename_exchange_line_dict, filename_exchange_line_jsn)

    def preprocess(self, line_space):
        if line_space is not None:
            for space in line_space:
                if re.search(r'[].*&,(.*?)[]', space):
                    line_space.extend(one.strip() for one in re.split(r'[].*&,(.*?)[]', space) if one !='' and one != ' ')
        else:
            return []
        return line_space

    def create_adversival_sample_exchange_line(self, file_name, origin_line, down_line):
        new_file_name = file_name.rsplit('.', 1)[0] + '_adversival_' + str(origin_line) + '.' + file_name.rsplit('.', 1)[1]
        with open(file_name, 'r') as f:
            lines = f.readlines()
        fo = open(new_file_name, 'w')
        for j in range(len(lines)):
            if j == down_line-1:
                fo.write(lines[origin_line-1])
            elif j == origin_line-1:
                fo.write(lines[down_line-1])
            else:
                fo.write(lines[j])
        fo.close()

    def translate_c_add_print(self, sample_source_path, temp_path, tid = '0', filename = 'filename_outspace.json'):
        problem_ids = []
        count = 0
        filename_outspace_dict = {}
        for id in os.listdir(sample_source_path):
            print(id, count)
            if id != tid:
                continue
            count += 1
            files = os.listdir(os.path.join(sample_source_path, id))
            if id in problem_ids:
                continue
            for file in tqdm(files, total=len(files)):
                if not(file.endswith('.c') or file.endswith('.cpp')):
                    continue

                try:
                    file_name = os.path.join(sample_source_path, id, file)

                    with open(file_name, 'r') as f:
                        code_content = f.readlines()
                    result = ''
                    for statement in code_content:
                        result += statement
                    inputs = InputStream(result)
                    lexer = CLexer(inputs)
                    lexer.removeErrorListeners()
                    stream = CommonTokenStream(lexer)
                    parser = CParser(stream)
                    parser.removeErrorListeners()
                    tree = parser.compilationUnit()
                    mv = ParserVisitor()
                    mv.visit(tree)
                except Exception as e:
                    print("Error Parse", file)
                    continue

                origin_line_outspace_dict = {}
                for origin_line in range(len(code_content)):
                    outspace = []
                    if origin_line+1 in mv.line_output_space.keys():
                        outspace = list(set(mv.line_output_space[origin_line+1]))
                    origin_line_outspace_dict[origin_line] = outspace
                key = id + '/' + file
                filename_outspace_dict[key] = origin_line_outspace_dict

        filename_outspace_path = os.path.join(temp_path, filename)
        with open(filename_outspace_path, 'w') as filename_outspace_jsn:
            json.dump(filename_outspace_dict, filename_outspace_jsn)


    def create_adversival_sample_print(self, file_name, origin_line, variable_declaration):
        with open(file_name, 'r') as f:
            lines = f.readlines()
        for i in range(len(variable_declaration)):
            new_file_name = file_name.rsplit('.', 1)[0] + '_adversival_' + str(origin_line) + '_' + str(i) + '.' + file_name.rsplit('.', 1)[1]
            fo = open(new_file_name, 'w')
            for j in range(len(lines)):
                if j == origin_line:
                    print_string = 'printf("%x\\n",' + '&'+list(variable_declaration).__getitem__(i) + ');\n'
                    fo.write(print_string)
                fo.write(lines[j])
            fo.close()

    def create_const_replace(self, const_variable, file_name):
        with open(file_name, 'r') as f:
            code_content = f.readlines()
        origin_code = ''
        new_code = ''
        for statement in code_content:
            origin_code += statement
        for value in const_variable.values():
            for one in value:
                new_const_name = ''.join(random.choice(string.ascii_uppercase) for i in range(5))
                # print(new_const_name)
                new_code = origin_code.replace(one, new_const_name)
                origin_code = new_code
        new_file_name = file_name.rsplit('.', 1)[0] + '_adversival_const_replace.' + \
                        file_name.rsplit('.', 1)[1]
        fo = open(new_file_name, 'w')
        fo.write(new_code)
        fo.close()
        f.close()
