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
            if (secondPosSpace == string::npos)
                throw InvalidInstruction(line);
            identifier = line.substr(posSpace + 1, secondPosSpace - posSpace - 1);
            type = line.substr(secondPosSpace + 1);
            if (!validIdentifier(identifier) || !validType(type))
                throw InvalidInstruction(line);
            insert(identifier, type, this->level);
        } else if (instruction == "ASSIGN") {
            string identifier, value;
            size_t secondPosSpace = line.find(' ', posSpace + 1);
            if (secondPosSpace == string::npos)
                throw InvalidInstruction(line);
            identifier = line.substr(posSpace + 1, secondPosSpace - posSpace - 1);
            value = line.substr(secondPosSpace + 1);
            if (!validIdentifier(identifier) || typeValue(value) == "errorValue")
                throw InvalidInstruction(line);
            assign(identifier, value, this->level);            
        } else if (instruction == "BEGIN" || instruction == "END") {
            block(instruction);
        } else if (instruction == "LOOKUP") {
            string identifier;
            identifier = line.substr(posSpace + 1);
            if (!validIdentifier(identifier))
                throw InvalidInstruction(line);
            lookup(identifier, this->level);
        } else if (instruction == "PRINT") {
            print();
        } else if (instruction == "RPRINT") {
            rPrint();
        } else {
            throw InvalidInstruction(line);
        }
    }
    if (this->level > 0)
        throw UnclosedBlock(this->level);
    ifs.close();
}

void SymbolTable::insert(string identifier, string type, int scope)
{
    if (head == nullptr) {
        Node *newNode = new Node(identifier, type, scope);
        head = tail = newNode;
        ++size;
        cout << "success"; 
    } else {
        Node *current = head;
        for (int i = 0; i < size; ++i)
        {
            if (scope > 0 && current->identifier == "BEGIN") break;
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
        cout << "success"; 
    }
}

Node* SymbolTable::isDeclared(string identifier, int scope)
{
    Node *current = head;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN") --scope;
        if (scope == current->scope && identifier == current->identifier) break;
        current = current->next;
    }    
    return current;
}

void SymbolTable::assign(string identifier, string value, int scope)
{
    string instruction = "ASSIGN " + identifier + " " + value;
    Node *current = isDeclared(identifier, scope);
    if (current == nullptr)        
        throw Undeclared(instruction);        
    if (typeValue(value) == "identifier") {
        Node *nodeValue = isDeclared(value, scope);
        if (nodeValue == nullptr) 
            throw Undeclared(instruction);             
        if (nodeValue->type != current->type)
            throw TypeMismatch(instruction);
        cout << "success";
    } else {
        if (typeValue(value) != current->type) 
            throw TypeMismatch(instruction);
        cout << "success";
    }
}

void SymbolTable::block(string instruction)
{
    Node *terminate = new Node(instruction);
    if (instruction == "BEGIN") {
        ++(this->level);
    } else {
        --(this->level);
    }
    if (this->level < 0)
        throw UnknownBlock();
    terminate->next = head;
    head = terminate;
}

bool SymbolTable::lookup(string identifier, int scope)
{
    Node *current = head;
    for (int i = 0; i < size; ++i)
    {
        if (scope > 0 && current->identifier == "BEGIN") --scope;
        if (scope == current->scope && identifier == current->identifier) {
            cout << scope;
            break;
        }
        current = current->next;
    }    
    if (current != nullptr)
        return true;
    return false;    
}

void SymbolTable::print(Node* pHead)
{
    if (pHead == nullptr) return;
    print(pHead->next);
    cout << " " << pHead->identifier << "//" << pHead->scope;
}

void SymbolTable::print()
{
    print(head);
}

void SymbolTable::rPrint()
{
    Node *current = head;
    while (current != tail)
    {
        cout << current->identifier << "//" << current->scope << " ";
        current = current->next;
    }
    cout << tail->identifier << "//" << tail->scope;
}