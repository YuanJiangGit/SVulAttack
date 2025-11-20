🛡️ SVulAttack  
==========================  
  
📄 A replication package for the paper "Shield Broken: Black-Box Adversarial Attacks on LLM-Based Vulnerability Detectors"    
📦 This repository contains data pipelines, attack algorithms, models, and utilities.   
  
🔖 Contents  
--------   
- 🧾 Project overview  
- 🗂️ Directory structure  
- 🔎 Important file descriptions  
- 🚀 Quick start  
- 📝 Statement
- 📚 Citation  
  
📁 Directory structure  
-------------------   
```
.      
├── 🧰 Attack/    
│   ├── __init__.py    
│   ├── Combination_Big_Vul.py         # Greedy attack implementation    
│   ├── Genetic_Big_Vul.py             # Genetic algorithm attack implementation    
│   └── Obfuscation_Big_Vul.py         # Utility used by Greedy attack    
├── ⚙️ Config/    
│   ├── __init__.py    
│   ├── config.cfg                     # Main experiment configuration file    
│   └── ConfigT.py                     # Utility to parse config.cfg    
├── 🧭 CParser/    
│   ├── cGrammer/                      # Grammar files / parser resources    
│   ├── __init__.py    
│   ├── ParseAndMutCode_Big_Vul.py     # Parse code and create intermediate files    
│   ├── ParserVisitor.py               # AST visitor used by the parser    
│   └── Prolog.py                      # Parser data structure    
├── 🧹 DataProcess/    
│   ├── __init__.py    
│   ├── Append_Data_Big_Vul.py         # Utility for sample dataset     
│   └── DataPipline_Big_Vul.py         # Utility for processing Big Vul dataset    
│   └── skidf_preprocess.py            # Preprocess similarity relationships   
├── ▶️ Entry/    
│   ├── __init__.py    
│   └── main_big_vul.py                # Entrypoint for running attacks    
├── 📚 resources/    
│   ├── Dataset/    
│   │   ├── BigVulFile/                # Original / raw samples files    
│   │   ├── embedding/                 # Embedding model     
│   │   ├── data.pkl                   # Big Vul dataset    
│   │   └── sample_ids.json            # Sample index file    
│   │   └── ...
│   └── SavedModels/    
│      └── 12heads_linevul_model.bin   # Pretrained LineVul model    
├── 🧩 Target_model/    
│   ├── __init__.py    
│   ├── linevul_main.py                # Model inference utility for LineVul    
│   └── linevul_model.py               # Model architecture for LineVul    
├── 🛠 Utils/    
│   ├── __init__.py    
│   ├── function.xls                   # Function name list used for identifier normalisation    
│   ├── get_tokens.py                  # Utility for extracting tokens from a sequence  
│   ├── mapping.py                     # Identifier normalization utilities    
│   ├── nope_Big_Vul.py                # Utility for masked-token evaluations    
│   └── Util.py                        # Utility for formatting results     
└── 📜 requirements.txt               # Python dependencies    
└── 📝 readme.md                      # README file    
 ```  
 
🔎 Important file descriptions  
---------------------------   
- 🧰 Attack/Combination_Big_Vul.py — Greedy attack coordinator.    
This file implements the primary greedy attack loop that iterates over sample IDs, computes a per-line statement importance, and tries a prioritized sequence of transformations: dead-code insertion, constant replacement, macro replacement, loop transformations, and variable renaming.   
  
- 🧬 Attack/Genetic_Big_Vul.py — Genetic-algorithm attack orchestrator.    
This module implements a population-based search over program variants: it builds per-line candidate sets, creates and evolves a population, evaluates fitness, and returns the best found variant for each sample.   
  
- 🔧 Attack/Obfuscation_Big_Vul.py — Backend utilities used by greedy attack.    
  This file centralizes target-model loading/wrapping, batch inference, similarity scoring, and sample parsing.   
  
- ▶️ Entry/main_big_vul.py — Command-line entrypoint and sample-preparation utilities.    
  This script provides: argument parsing, sample selection, and the chosen attack.   
  
🚀 Quick start  
-----------------------------------------   
To reproduce experiments on LineVul:   
  
1) Set up the Environment   
   - First of all, clone this repository to your local machine and access the main directory via the following commands:   
     ```bash  
     git clone https://github.com/YuanJiangGit/SVulAttack.git  
     cd SVulAttack  
     ```  
   - Then, install the Python dependencies via the following command:   
     ```bash  
     pip install -r requirements.txt  
     ```  
     Note: Since the Torch version is strongly dependent on the CUDA version installed on your machine, we cannot specify a particular installation version here. Please install based on your specific configuration to make the GPU usable. For installation commands, refer to [this website](https://pytorch.org/).   
  
2) Prepare Detection Model  
    - Run the following commands to download the pretrained model.   
      ```bash
      cd resources  
      cd SavedModels  
      gdown https://drive.google.com/uc?id=1RkIHg6sFnCQatodDHYhSkuTRSBULmayI
      cd ../..   
      ```  
    - For more information on the detection model, refer to [LineVul](https://github.com/awsm-research/LineVul) and [StagedVulBERT](https://github.com/YuanJiangGit/StagedVulBERT).   

3) Prepare DataSet  
    - Run the following commands to download the dataset.   
      ```bash
      cd resources  
      cd Dataset  
      gdown https://drive.google.com/uc?id=16Ud3P--4DaJnNugiIUmcY7fGuVhUSZpz
      cd ../..   
      ```  
    - This file is the Big Vul dataset processed by DataProcess/DataPipline_Big_Vul.py. For more information on the dataset, refer to [this repository](https://github.com/rshariffdeen/Big-Vul/tree/master). 
  
4) Run an attack  
   - Run the greedy attack via the following commands:   
     ```bash  
     cd Entry  
     python main_big_vul.py --algorithm greedy --result_file greedy.csv   
     cd ..  
     ```  
   - Run the genetic attack via the following commands:   
     ```bash  
     cd Entry  
     python main_big_vul.py --algorithm genetic --result_file genetic.csv   
     cd ..  
     ```  
   The result file is saved under SVulAttack/resources/Results.  

   If the default settings are not used, the following parameters can also be modified during the attack process:   
  
   - --limits (int, default: 15): Maximum number of modifications allowed per sample.   
   - --batch_size (int, default: 64): Batch size used during model inference .   
   - --add_tag (bool, default: True): Enable/disable dead-code insertion.   
   - --const_tag (bool, default: True): Enable/disable constant replacement.   
   - --macro_tag (bool, default: True): Enable/disable macro replacement.   
   - --unroll_loop (bool, default: True): Enable/disable loop transformations.   
   - --var_tag (bool, default: True): Enable/disable variable renaming.   
   - --random_tag (bool, default: False): Enable/disable the random algorithm (greedy only).

5) Tutorial video of Quick start  
    [![Tutorial video](https://img.youtube.com/vi/Vpne3Elgyuw/hqdefault.jpg)](https://youtu.be/Vpne3Elgyuw)

📝 Statement
---------------------------  
The code provided in this repository is exclusively for open-source models. To prevent attacks from being misused, we will not publicly release attack code targeting closed-source models at this time, even if it is nearly identical to existing code in this repository. If needed, please contact the author to obtain it.
  
📚 Citation  
-------------------   
Jiang Y, Huang S, Treude C, Su X, Wang T. Shield Broken: Black-Box Adversarial Attacks on LLM-Based Vulnerability Detectors.