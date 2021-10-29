#include "SymbolTable.h"

bool validIdentifier(string identifier) 
{
    if (identifier[0] < 'a' || identifier[0] > 'z')
        return false;
    int length = identifier.length();
    for (int i = 1; i < length; ++i)
    {
        if (identifier[i] >= 'a' && identifier[i] <= 'z'); 
        else if (identifier[i] >= 'A' && identifier[i] <= 'Z');
        else if (identifier[i] >= '0' && identifier[i] <= '9');
        else if (identifier[i] == '_');
        else
            return false;
    }
    return true;
}

bool validType(string type)
{
    if (type == "number" || type == "string")
        return true;
    return false;
}

string typeValue(string value)
{
    int length = value.length();
    if (value[0] == '\'' && value[length - 1] == '\'') {
        for (int i = 1; i < length - 1; ++i) {
            if (value[i] >= 'a' && value[i] <= 'z'); 
            else if (value[i] >= 'A' && value[i] <= 'Z');
            else if (value[i] >= '0' && value[i] <= '9');
            else if (value[i] == ' ');
            else
                return "errorValue";            
        }
        return "string";
    } else if (validIdentifier(value)) {
        return "identifier";
    } else {
        for (int i = 0; i < length; ++i) {
            if (value[i] >= '0' && value[i] <= '9');
            else
                return "errorValue";
        }
        return "number";
    }
}

void SymbolTable::run(string filename)
{
    ifstream ifs;
    ifs.open(filename);
    string line;
    while (getline(ifs, line))
    {
        size_t posSpace = line.find(' ');
        string instruction;
        if (posSpace == string::npos)   
            instruction = line;
        else
            instruction = line.substr(0, posSpace);
        if (instruction == "INSERT") {
            string identifier, type;
            size_t secondPosSpace = line.find(' ', posSpace + 1);
            if (secondPosSpace == string::npos || secondPosSpace <= posSpace + 1) {
                throw InvalidInstruction(line);
            }
            identifier = line.substr(posSpace + 1, secondPosSpace - posSpace - 1);
            type = line.substr(secondPosSpace + 1);
            if (!validIdentifier(identifier) || !validType(type)) {
                throw InvalidInstruction(line);
            }
            insert(identifier, type);
        } else if (instruction == "ASSIGN") {
            string identifier, value;
            size_t secondPosSpace = line.find(' ', posSpace + 1);
            if (secondPosSpace == string::npos || secondPosSpace <= posSpace + 1) {
                throw InvalidInstruction(line);
            }
            identifier = line.substr(posSpace + 1, secondPosSpace - posSpace - 1);
            value = line.substr(secondPosSpace + 1);
            if (!validIdentifier(identifier) || typeValue(value) == "errorValue") {
                throw InvalidInstruction(line);
            }
            assign(identifier, value);            
        } else if (instruction == "BEGIN" || instruction == "END") {
            block(instruction);
        } else if (instruction == "LOOKUP") {
            string identifier;
            identifier = line.substr(posSpace + 1);
            if (!validIdentifier(identifier)) {
                throw InvalidInstruction(line);
            }
            lookup(identifier);
        } else if (instruction == "PRINT") {
            print();
        } else if (instruction == "RPRINT") {
            rPrint();
        } else {
            throw InvalidInstruction(line);
        }
    }
    if (this->level > 0) {
        throw UnclosedBlock(this->level);
    }
    this->clear();
    ifs.close();
}

void SymbolTable::insert(string identifier, string type)
{
    int scope = this->level;
    if (head == nullptr) {
        head = tail = new Node(identifier, type, scope);
        ++size;
        cout << "success" << endl; 
    } else {
        Node *current = head;
        for (int i = 0; i < size; ++i)
        {
            if (scope > 0 && current->identifier == "BEGIN" && scope == current->scope) break;
            if (scope == current->scope && identifier == current->identifier) {
                string instruction = "INSERT " + identifier + " " + type;
                throw Redeclared(instruction);
            }
            current = current->next;
        }
        Node *newNode = new Node(identifier, type, scope);
        newNode->next = head;
        head = newNode;
        ++size;
        cout << "success" << endl; 
    }
}

void SymbolTable::assign(string identifier, string value)
{
    string instruction = "ASSIGN " + identifier + " " + value;
    Node *current = head;
    int scope = this->level;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN" && scope == current->scope) --scope;
        if (scope == current->scope && identifier == current->identifier) break;
        current = current->next;
    }    
    if (current == nullptr) {
        throw Undeclared(instruction);
    } else {
        scope = this->level;
        if (typeValue(value) == "identifier") {
            Node *currValue = head;
            for (int i = 0; i < size; ++i)
            {
                if (scope > 0 && currValue->identifier == "BEGIN" && scope == currValue->scope) --scope;
                if (scope == currValue->scope && value == currValue->identifier) break;
                currValue = currValue->next;
            }      
            if (currValue == nullptr) {
                throw Undeclared(instruction); 
            } else {
                if (currValue->type != current->type) {
                    throw TypeMismatch(instruction);
                }
                cout << "success" << endl;
            }
        } else {
            if (typeValue(value) != current->type) {
                throw TypeMismatch(instruction);
            }
            cout << "success" << endl;
        }        
    }
}

void SymbolTable::block(string instruction)
{
    Node *terminate = new Node(instruction);
    if (instruction == "BEGIN") {
        terminate->scope = this->level + 1;
        ++(this->level);
    } else {
        terminate->scope = this->level;
        --(this->level);
    }
    if (head == nullptr) {
        head = tail = terminate;
        ++size;
    } else {
        terminate->next = head;
        head = terminate;
        ++size;
    }
    if (this->level < 0) {
        throw UnknownBlock();
    }
}

void SymbolTable::lookup(string identifier)
{
    Node *current = head;
    int scope = this->level;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN" && scope == current->scope) --scope;
        if (scope == current->scope && identifier == current->identifier) {
            cout << scope << endl;
            break;
        }
        current = current->next;
    }    
    if (current == nullptr)
    {
        string instruction = "LOOKUP " + identifier;
        throw Undeclared(instruction);
    }
}

void LinkedSymbol::insertHead(string identifier, string type, int scope)
{
    if (!contain(identifier)) 
    {
        ++(this->size);
        Node *newNode = new Node(identifier, type, scope);
        if (head == nullptr) {
            head = tail = newNode;
        } else 
        {
            newNode->next = head;
            head = newNode;
        }
    }
}

void LinkedSymbol::insertTail(string identifier, string type, int scope)
{
    if (!contain(identifier))
    {
        ++(this->size);
        Node *newNode = new Node(identifier, type, scope);
        if (head == nullptr) {
            head = tail = newNode;
        } else 
        {
            tail->next = newNode;
            tail = newNode;
        }
    }
}

bool LinkedSymbol::contain(string identifier)
{
    Node *current = head;
    while (current != nullptr)
    {
        if (current->identifier == identifier) return true;
        current = current->next;
    }
    return false;
}

void LinkedSymbol::print()
{
    if (size == 0) return;
    Node *current = head;
    for (int i = 0; i < size - 1; ++i)
    {
        cout << current->identifier << "//" << current->scope << " ";
        current = current->next;
    }
    cout << tail->identifier << "//" << tail->scope << endl;
}

void LinkedSymbol::clear()
{
    while (head != nullptr)
    {
        Node* prev = head;
        head = head->next;
        delete prev;  
    }
    head = tail = nullptr;
    size = 0;
}

void SymbolTable::print()
{
    int scope = this->level;
    Node *current = head;
    LinkedSymbol symbolToPrint;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN" && current->scope == scope) --scope;
        if (scope == current->scope && current->identifier != "BEGIN" && current->identifier != "END")
        {
            symbolToPrint.insertHead(current->identifier, current->type, current->scope);
        }
        current = current->next;
    }
    symbolToPrint.print();
}

void SymbolTable::rPrint()
{
    int scope = this->level;
    Node *current = head;
    LinkedSymbol symbolToPrint;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN" && current->scope == scope) --scope;
        if (scope == current->scope && current->identifier != "BEGIN" && current->identifier != "END")
        {
            symbolToPrint.insertTail(current->identifier, current->type, current->scope);
        }
        current = current->next;
    }
    symbolToPrint.print();
}

void SymbolTable::clear()
{
    while (head != nullptr)
    {
        Node* prev = head;
        head = head->next;
        delete prev;  
    }
    head = tail = nullptr;
    size = 0;
    level = 0;
}