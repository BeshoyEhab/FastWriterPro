#pragma once
#include <unordered_map>

class TrieNode {
  public:
    std::unordered_map<char, TrieNode*> children;
    int frequency;

    TrieNode();
};
