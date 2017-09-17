#ifndef SCORER_H_
#define SCORER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "lm/enumerate_vocab.hh"
#include "lm/virtual_interface.hh"
#include "lm/word_index.hh"
#include "util/string_piece.hh"

#include "path_trie.h"

const double OOV_SCORE = -1000.0;
const std::string START_TOKEN = "<s>";
const std::string UNK_TOKEN = "<unk>";
const std::string END_TOKEN = "</s>";

// Implement a callback to retrive string vocabulary.
class RetriveStrEnumerateVocab : public lm::EnumerateVocab {
public:
  RetriveStrEnumerateVocab() {}

  void Add(lm::WordIndex index, const StringPiece &str) {
    vocabulary.push_back(std::string(str.data(), str.length()));
  }

  std::vector<std::string> vocabulary;
};

/* External scorer to query score for n-gram or sentence, including language
 * model scoring and word insertion.
 *
 * Example:
 *     Scorer scorer(alpha, beta, "path_of_language_model");
 *     scorer.get_log_cond_prob({ "WORD1", "WORD2", "WORD3" });
 *     scorer.get_sent_log_prob({ "WORD1", "WORD2", "WORD3" });
 */
class Scorer {
public:
  Scorer(double alpha,
         double beta,
         const std::string &lm_path,
         const std::vector<std::string> &vocabulary);
  ~Scorer();

  double get_log_cond_prob(const std::vector<std::string> &words);

  double get_sent_log_prob(const std::vector<std::string> &words);

  size_t get_max_order() const { return _max_order; }

  size_t get_dict_size() const { return _dict_size; }

  bool is_char_map_empty() const { return _char_map.size() == 0; }

  bool is_character_based() const { return _is_character_based; }

  // reset params alpha & beta
  void reset_params(float alpha, float beta);

  // make ngram for a given prefix
  std::vector<std::string> make_ngram(PathTrie *prefix);

  // trransform the labels in index to the vector of words (word based lm) or
  // the vector of characters (character based lm)
  std::vector<std::string> split_labels(const std::vector<int> &labels);

  // expose to decoder
  double alpha;
  double beta;

  // fst dictionary
  void *dictionary;

protected:
  void setup(const std::string &lm_path,
             const std::vector<std::string> &vocab_list);

  void load_lm(const std::string &lm_path);

  // fill dictionary for fst
  void fill_dictionary(bool add_space);

  // set char map
  void set_char_map(const std::vector<std::string> &char_list);

  double get_log_prob(const std::vector<std::string> &words);

  std::string vec2str(const std::vector<int> &input);

private:
  void *_language_model;
  bool _is_character_based;
  size_t _max_order;
  size_t _dict_size;

  int _SPACE_ID;
  std::vector<std::string> _char_list;
  std::unordered_map<char, int> _char_map;

  std::vector<std::string> _vocabulary;
};

#endif  // SCORER_H_