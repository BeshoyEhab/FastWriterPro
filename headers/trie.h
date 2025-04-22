#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    int frequency;
    TrieNode() : frequency(0) {}
};

class Trie {
private:
    TrieNode* root;

    struct Comparator {
        bool operator()(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return (a.second > b.second) || (a.second == b.second && a.first < b.first);
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
    std::vector<std::string> autoComplete(const std::string& prefix, int max_suggestions = 4);
};
