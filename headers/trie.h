#pragma once
#include <vector>
#include <string>
#include <queue>
#include <../assets/json.hpp>
#include "trienode.h"

using json = nlohmann::json;

class Trie {
private:
    TrieNode* root;
    std::unordered_map<std::string, int> newWords;

    struct Comparator
    {
        bool useBFS;
        bool useFreq;
        Comparator(bool bfs, bool usefreq) : useBFS(bfs), useFreq(usefreq) {}
        bool operator()(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) const {
            if (useFreq && a.second != b.second)
                return a.second > b.second;

            if (useBFS) {
                if (a.first.length() != b.first.length())
                    return a.first.length() < b.first.length();
            }

            return a.first < b.first;
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
    bool changed = false;
    bool contain(const std::string& s);
    void addNew(std::string s);
    void makeJson(json& outJson);
    void insert(const std::string& word, int frequency = 1);
    void reset();
    bool remove(const std::string &word);
    std::vector<std::string> autoComplete(const std::string& prefix, bool bfs = false, bool nofreq = false, int max_suggestions = 4);
};
