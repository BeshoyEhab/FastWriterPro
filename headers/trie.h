#include <vector>
#include <string>
#include <queue>
#include <nlohmann/json.hpp>
#include "trienode.h"

using json = nlohmann::json;

class Trie {
private:
    TrieNode* root;

    struct Comparator {
        bool useBFS;
        Comparator(bool bfs) : useBFS(bfs) {}
        bool operator()(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
        {
            if (a.second != b.second)
                return a.second > b.second;
            else {
                if (useBFS) {
                    if (a.first.length() == b.first.length())
                        return a.first > b.first;
                    else
                        return a.first.length() < b.first.length();
                } else
                    return a.first < b.first;
            }
        }
    };

    void collectWords(TrieNode* node, std::string& currentSuffix,
                      std::priority_queue<std::pair<std::string, int>,
                                          std::vector<std::pair<std::string, int>>,
                                          Comparator>& pq,
                      const std::string& prefix, int max_suggestions);
    void collectJsonEntries(TrieNode *node, std::string &currentWord, json &j);
    void resetEntries(TrieNode *node, std::string &currentWord);

public:
    Trie();
    void makeJson(json& outJson);
    void insert(const std::string& word, int frequency = 1);
    void reset();
    std::vector<std::string> autoComplete(const std::string& prefix, int max_suggestions = 4, bool bfs = false);
};
