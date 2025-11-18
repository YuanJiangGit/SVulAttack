# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/16 12:23
# @Function:


class Prolog(object):

    def __init__(self, name, id, value, father_id, children_id=set(), line=-1):
        self.name = name
        self.id = id
        self.value = value
        self.father_id = father_id
        self.children_id = children_id
        self.line = line

    def toString(self):
        print('%s(%d,%d,[' %
              (self.name, self.id, self.father_id), end='')
        while self.children_id.__len__() > 1:
            print('%d,' % self.children_id.pop(), end='')
        if self.children_id.__len__() == 1:
            print('%d' % self.children_id.pop(), end='')
        print('],\'%s\','% self.value, end='')
        print('line=%d).' % self.line)

    def getChildrenCount(self):
       return self.children_id.__len__()

