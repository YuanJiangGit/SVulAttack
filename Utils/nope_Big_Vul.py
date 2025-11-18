"""
nope_Big_Vul.py

Small helper that performs masked-token style model evaluations for the Big Vul dataset.
"""
from Target_model.linevul_main import convert_examples_to_features
class NopeTool_Big_Vul():
    """
    Class to run batched model calls for masked/ablated programs.
    Attributes:
        config: configuration object.
        obfuscation: an instance of Obfuscation_Big_Vul class to provide approximate probability.
    """
    def __init__(self, config, obfuscation):
        self.config = config
        self.obfuscation = obfuscation

    def run_model_by_batch(self, dataloader, id):
        """
        Run the model for a list of batches and return probability scores.
        Args:
            dataloader: iterable of batches.
            ori_label: original label.
        """
        probability = []
        for batch in dataloader:
            if self.obfuscation.model_name == '12heads_linevul_model.bin':
                inputs_ids, labels= [], []
                for i in batch:
                    code = ''.join(i)
                    x = convert_examples_to_features(code, 1, self.obfuscation.tokenizer, self.obfuscation)
                    inputs_ids.append(x.input_ids)
                    labels.append(x.label)
                    probability.append(self.obfuscation.Compare(code, id))
                continue
        return None, probability