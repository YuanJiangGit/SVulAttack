"""
skidf_preprocess.py

Preprocess a dataset of source code snippets to build a TF-IDF similarity index
between vulnerable and fixed files.
"""
import pandas as pd
from tqdm import tqdm
import pickle
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

def main(data, top_n=100):
    """
    Build a dictionary mapping each file id to its top similar files using TF-IDF.
    The function separates the dataset rows into "vulnerable" and "fixed" groups
    according to the 'vul' column, computes TF-IDF vectors for all snippets,
    and for each file computes up to 250 most similar files from the opposite group.
    Args:
        data: Input dataset:
    """
    # Lists to keep file ids and their corresponding code bodies, split by vulnerability
    total = len(data)
    vul_names = []
    vul_codes = []
    fix_names = []
    fix_codes = []

    def process_row(row):
        if row['vul'] != 0:
            vul_names.append(row['Unnamed: 0'])
            vul_codes.append(row['func_before'])
        else:
            fix_names.append(row['Unnamed: 0'])
            fix_codes.append(row['func_before'])

    data.apply(process_row, axis=1)
    print(str(len(vul_names) + len(fix_names)) + '/' + str(total))

    vectorizer = TfidfVectorizer()

    vectorizer.fit(vul_codes + fix_codes)

    vul_vecs = vectorizer.transform(vul_codes)
    fix_vecs = vectorizer.transform(fix_codes)

    sim_high_pairs = {}

    # For each vulnerable file, compute similarities to all fixed files and keep the top-N
    for i, (vul_name, vul_vec) in tqdm(enumerate(zip(vul_names, vul_vecs)), total=len(vul_names), desc="Processing vulnerable files"):
        if int(vul_name) in sim_high_pairs:
            continue
        # Cosine similarities between this vulnerable file and all fixed files
        similarities = cosine_similarity(vul_vec, fix_vecs)
        # Pair each fixed file id with its similarity score and convert to percent-scale
        pair_sim = [(sim * 100, fix_name) for (sim, fix_name) in zip(similarities[0], fix_names)]
        # Sort by similarity descending
        pair_sim = sorted(pair_sim, key=lambda x: x[0], reverse=True)

        # Keep up to top N similar fixed files for this vulnerable file
        sim_high_pairs[int(vul_name)] = []
        for j, (sim, fix_name) in enumerate(pair_sim):
            sim_high_pairs[int(vul_name)].append((fix_name, sim))
            if j >= top_n:
                break

        if i % 100 == 0:
            with open(f'./save/skidf_sim_between_files_vul.pkl', 'wb') as f:
                pickle.dump(sim_high_pairs, f)

    # For each fixed file, compute similarities to all vulnerable files and keep the top-N
    for i, (fix_name, fix_vec) in tqdm(enumerate(zip(fix_names, fix_vecs)), total=len(fix_names), desc="Processing fixed files"):
        if int(fix_name) in sim_high_pairs:
            continue

        similarities = cosine_similarity(fix_vec, vul_vecs)
        pair_sim = [(sim * 100, vul_name) for (sim, vul_name) in zip(similarities[0], vul_names)]
        pair_sim = sorted(pair_sim, key=lambda x: x[0], reverse=True)

        sim_high_pairs[int(fix_name)] = []
        for j, (sim, vul_name) in enumerate(pair_sim):
            sim_high_pairs[int(fix_name)].append((vul_name, sim))
            if j >= top_n:
                break

        if i % 1000 == 0:
            with open(f'./save/skidf_sim_between_files_fix.pkl', 'wb') as f:
                pickle.dump(sim_high_pairs, f)

    with open(f'./skidf_sim_between_files.pkl', 'wb') as f:
        pickle.dump(sim_high_pairs, f)


if __name__ == "__main__":
    data = pd.read_csv('')
    main(data)