# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/12 8:57
# @Function: other tool functions

def concat_statement(slice_list):
    r = ''
    for s in slice_list:
        r += s + '<EOL>'
    return r