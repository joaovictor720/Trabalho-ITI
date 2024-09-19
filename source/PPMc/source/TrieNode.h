#ifndef TRIENODE_H
#define TRIENODE_H

#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>

class TrieNode
{
public:
    TrieNode();
    ~TrieNode();

    const std::unordered_map<uint16_t, TrieNode*>& getChildren() const;
    int getCount() const;
    void setCount(int count);  // Define o contador diretamente

    void incrementCount();
    TrieNode* getChild(uint16_t symbol);
    TrieNode* createChild(uint16_t symbol);
    void deleteChildren();
    void adoptChild(uint16_t symbol, TrieNode* child);  // Adiciona um filho diretamente


private:
    int count;  // Contador para a frequência do símbolo
    std::unordered_map<uint16_t, TrieNode*> children;

};

#endif