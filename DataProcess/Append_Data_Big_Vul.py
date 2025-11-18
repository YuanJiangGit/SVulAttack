"""
Append_Data_Big_Vul.py

Load a pretrained vulnerability detection model (LineVul)
and provide a convenience API to predict the label for a single program slice.
"""
import os
import random
import torch
import numpy as np
from Target_model.linevul_model import Model
from Target_model.linevul_main import convert_examples_to_features
from transformers import RobertaConfig, RobertaForSequenceClassification, RobertaTokenizer
class LineVulPredict():
    """
    Class for loading a pretrained LineVul detection model and predicting
    single program slices.
    """

    def __init__(self, config):
        self.detect_model = None
        self.config = config
        self.detect_model = self.load_trained_model()

    def load_trained_model(self):
        """
        Load the pretrained target model and return the model instance.
        """
        # If already loaded, return cached instance
        if self.detect_model != None:
            return self.detect_model
        else:
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

            if self.config.use_gpu:
                self.device = torch.device("cuda")
                detect_model.to(self.device)
            # load model
            if os.path.exists(self.config.models_path + '12heads_linevul_model.bin'):
                detect_model.load_state_dict(torch.load(self.config.models_path + '12heads_linevul_model.bin', map_location=self.device), strict=False)
                detect_model.to(self.device)
                self.detect_model = detect_model
                return self.detect_model
            else:
                print('No Pretrained Model, Please Train first!')
                return None

    def predict_adv_program(self, adv_program):
        """
        Predict the binary label for a single program slice.
        Args:
           adv_program: program slice
        """
        self.detect_model.eval()
        logits = []
        label = 1
        x = convert_examples_to_features('\n'.join(adv_program), label, self.tokenizer, self)
        inputs_ids = torch.tensor(x.input_ids).unsqueeze(0).to(self.device)
        labels = torch.tensor(x.label).unsqueeze(0).to(self.device)
        with torch.no_grad():
            lm_loss, logit = self.detect_model(input_ids=inputs_ids, labels=labels)
            logits.append(logit.cpu().numpy())
        # calculate scores
        logits = np.concatenate(logits, 0)
        assert (len(logits) == 1)
        predicted = (logits[:, 1] > self.best_threshold)[0]
        return predicted
