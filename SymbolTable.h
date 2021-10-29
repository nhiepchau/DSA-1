#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node;
class SymbolTable
{
private:
    Node *head;
    Node *tail;
    int size;
    int level;
public:
    SymbolTable() {
        head = nullptr;
        tail = nullptr;
        size = 0;
        level = 0;
    }
    ~SymbolTable() {
        this->clear();
    }
    void run(string filename);
    void insert(string identifier, string type);
    void assign(string identifier, string value);
    void block(string instruction);
    void lookup(string identifier);
    void print();
    void rPrint();
    void clear();
};

class LinkedSymbol
{
private:
    Node *head;
    Node *tail;
    int size;
public:
    LinkedSymbol() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }
    ~LinkedSymbol() {
        this->clear();
    }
    void insertHead(string identifier, string type, int scope);
    void insertTail(string identifier, string type, int scope);
    bool contain(string identifier);
    void print();
    void clear();
};

class Node 
{
private:
    string identifier, type;
    int scope;
    Node *next;
    friend SymbolTable;
    friend LinkedSymbol;
public:
    Node() {
        identifier = "";
        type = "";
        scope = 0;
        next = nullptr;
    }
    Node(string identifier, string type, int scope) {
        this->identifier = identifier;
        this->type = type;
        this->scope = scope;
        next = nullptr;
    }
    Node(string identifier) {
        this->identifier = identifier;
        this->type = "";
        this->scope = 0;
        next = nullptr;        
    }
};
#endif