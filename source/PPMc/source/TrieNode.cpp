#include "TrieNode.h"

TrieNode::TrieNode() : count(0)
{

}

// Libera a memória alocada recursivamente
TrieNode::~TrieNode() {
    for (auto& child : children) {
        delete child.second;
    }
}

// Retorna os filhos
const std::unordered_map<uint16_t, TrieNode*>& TrieNode::getChildren() const {
    return children;
}

// Retorna a contagem
int TrieNode::getCount() const {
    return count;
}

// Incrementa o contador deste nó
void TrieNode::incrementCount() {
    ++count;
}

// Retorna o filho associado a um byte específico
TrieNode* TrieNode::getChild(uint16_t symbol) {
    if (children.find(symbol) != children.end()) {
        return children[symbol];
    }
    return nullptr;
}

// Cria um novo filho para o símbolo dado, se ainda não existir
TrieNode* TrieNode::createChild(uint16_t symbol) {
    if (children.find(symbol) == children.end()) {
        children[symbol] = new TrieNode();
    }
    return children[symbol];
}

// Libera a memória alocada para a Trie
void TrieNode::deleteChildren() {
    for (auto& child : children) {
        if (child.second) {
            child.second->deleteChildren();
            delete child.second;
        }
    }
    children.clear();
}

void TrieNode::adoptChild(uint16_t symbol, TrieNode* child) {
    children[symbol] = child;
}

void TrieNode::setCount(int newCount) {
    count = newCount;
}