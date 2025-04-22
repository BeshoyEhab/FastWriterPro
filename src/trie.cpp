#include "trie.h"
#include <QJsonObject>

Trie::Trie() : root(new TrieNode()) {}

void Trie::insert(const std::string& word, int frequency) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children.count(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    node->frequency += frequency;
}

std::vector<std::string> Trie::autoComplete(const std::string& prefix, int max_suggestions) {
    TrieNode* node = root;
    for (char c : prefix) {
        auto it = node->children.find(c);
        if (it == node->children.end()) return {};
        node = it->second;
    }

    std::priority_queue<
        std::pair<std::string, int>,
        std::vector<std::pair<std::string, int>>,
        Comparator
        > pq;

    std::string currentSuffix;
    collectWords(node, currentSuffix, pq, prefix, max_suggestions);

    // Extract results in reverse order (min-heap -> descending)
    std::vector<std::string> result;
    while (!pq.empty()) {
        result.insert(result.begin(), pq.top().first);
        pq.pop();
    }
    return result;
}

void Trie::collectWords(
    TrieNode* node,
    std::string& currentSuffix,
    std::priority_queue<
        std::pair<std::string, int>,
        std::vector<std::pair<std::string, int>>,
        Comparator
        >& pq,
    const std::string& prefix,
    int max_suggestions
    ) {
    if (node->frequency > 0) {
        pq.emplace(prefix + currentSuffix, node->frequency);
        if (pq.size() > max_suggestions) {
            pq.pop(); // Maintain only top N elements
        }
    }

    for (auto& kv : node->children) {
        currentSuffix.push_back(kv.first);
        collectWords(kv.second, currentSuffix, pq, prefix, max_suggestions);
        currentSuffix.pop_back();
    }
}

void Trie::makeJson(json &outJson)
{
    std::string buffer;
    collectJsonEntries(root, buffer, outJson);
}

void Trie::collectJsonEntries(TrieNode *node, std::string &currentWord, json& j) {
    if (node == nullptr) return;

    if (node->frequency > 0) {
        j[currentWord] = node->frequency;
    }

    for (const auto& pair : node->children) {
        currentWord.push_back(pair.first);
        collectJsonEntries(pair.second, currentWord, j);
        currentWord.pop_back();
    }
}

void Trie::reset()
{
    std::string buffer;
    resetEntries(root, buffer);
}

void Trie::resetEntries(TrieNode *node, std::string &currentWord)
{
    if (node == nullptr)
        return;

    if (node->frequency > 0) {
        node->frequency = 1;
    }

    for (const auto &pair : node->children) {
        currentWord.push_back(pair.first);
        resetEntries(pair.second, currentWord);
        currentWord.pop_back();
    }
}
