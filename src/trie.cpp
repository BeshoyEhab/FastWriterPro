#include "trie.h"
#include <QMessageBox>

Trie::Trie() : root(new TrieNode()) {}

void Trie::insert(const std::string& word, int frequency) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children.count(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    changed = true;
    if (node->frequency < 0)
        node->frequency = 0;
    node->frequency += frequency;
}

bool Trie::contain(const std::string& s)
{
    TrieNode* node = root;
    for (char c : s)
    {
        if (!node->children.count(c))
            return false;
        node = node->children[c];
    }
    return node->frequency > 0;
}

void Trie::addNew(std::string s)
{
    if (s.empty())
        return;
    if (contain(s))
        insert(s);
    else if (newWords.count(s) > 0) {
        if (++newWords[s] >= 3) {
            changed = true;
            insert(s, newWords[s]);
            newWords.erase(s);
        }
    } else
        newWords[s] = 1;
}

std::vector<std::string> Trie::autoComplete(const std::string& prefix, bool bfs, bool usefreq, int max_suggestions) {
    if (prefix.empty())
        return {prefix};
    TrieNode* node = root;
    for (char c : prefix) {
        auto it = node->children.find(c);
        if (it == node->children.end()) return {};
        node = it->second;
    }

    std::priority_queue<std::pair<std::string, int>,
                        std::vector<std::pair<std::string, int>>,
                        Comparator>
        pq((Comparator(bfs, usefreq)));
    std::string currentSuffix;
    collectWords(node, currentSuffix, pq, prefix, max_suggestions);

    std::vector<std::string> result;
    while (!pq.empty()) {
        if (pq.top().first != prefix)
            result.insert(result.begin(), pq.top().first);
        pq.pop();
    }
    if (result.size() == max_suggestions)
        result.pop_back();
    result.insert(result.begin(), prefix);
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
            pq.pop();
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

    if (node->frequency >= 0) {
        j[currentWord] = node->frequency;
    }

    for (const auto& pair : node->children) {
        currentWord.push_back(pair.first);
        collectJsonEntries(pair.second, currentWord, j);
        currentWord.pop_back();
    }
}

bool Trie::remove(const std::string& word)
{
    TrieNode* node = root;
    for (char c : word)
    {
        if (!node->children.count(c))
            return false;
        node = node->children[c];
    }
    node->frequency = 0;
    return true;
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
