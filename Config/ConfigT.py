# -*- coding: utf-8 -*-
# @Author  : Jiang Yuan
# @Time    : 2021/5/12 13:57
# @Function: configure file

from configparser import ConfigParser


class MyConf:
    def __init__(self, config_file):
        config = ConfigParser()
        config.read(config_file, encoding='utf-8')
        self._config = config
        self.config_file=config_file

        # config.write(open(config_file,'w'))

    @property
    def wordEmbedding(self):
        return self._config.getboolean('data', 'word_embedding')

    @property
    def data_path(self):
        return self._config.get('data', 'data_path')

    @property
    def models_path(self):
        return self._config.get('data', 'models_path')

    @property
    def embedding_path(self):
        return self._config.get('data', 'embedding_path')

    @property
    def learning_rate(self):
        return self._config.getfloat('Optimizer', 'learning_rate')

    @property
    def weight_decay(self):
        return self._config.getfloat('Optimizer', 'weight_decay')

    # Train
    @property
    def epochs(self):
        return self._config.getint("Train", "epochs")

    @property
    def use_gpu(self):
        return self._config.getboolean('use', 'use_gpu')

    @property
    def sample_source_path(self):
        return self._config.get('data', 'sample_source_path')

    @property
    def temp_path(self):
        return self._config.get('data', 'temp_path')

    @property
    def sample_slice_path(self):
        return self._config.get('data', 'sample_slice_path')

    @property
    def result_path(self):
        return self._config.get('data', 'result_path')

    @property
    def root_path(self):
        return self._config.get('data', 'root_path')

    def set_value(self, section, param, value):
        self._config.set(section, param, value)
        self._config.write(open(self.config_file, 'w'))

if __name__ == '__main__':
    conf = MyConf('config.cfg')
    print(conf.wordEmbedding)
    print(conf.charData)
