#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

struct Symbol {
    string name;
    int address;
};

struct Literal {
    string value;
    int address;
};

struct IntermediateCode {
    int lc;
    string code;
};

map<string, pair<string, string>> OPTAB = {
    {"STOP", {"IS", "00"}}, {"ADD", {"IS", "01"}}, {"SUB", {"IS", "02"}},
    {"MULT", {"IS", "03"}}, {"MOVER", {"IS", "04"}}, {"MOVEM", {"IS", "05"}},
    {"COMP", {"IS", "06"}}, {"BC", {"IS", "07"}}, {"DIV", {"IS", "08"}},
    {"READ", {"IS", "09"}}, {"PRINT", {"IS", "10"}},

    {"START", {"AD", "01"}}, {"END", {"AD", "02"}}, {"ORIGIN", {"AD", "03"}},
    {"EQU", {"AD", "04"}}, {"LTORG", {"AD", "05"}},

    {"DC", {"DL", "01"}}, {"DS", {"DL", "02"}}
};

map<string, int> REGTAB = {
    {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
};

map<string, int> COND = {
    {"LT", 1}, {"LE", 2}, {"EQ", 3}, {"GT", 4}, {"GE", 5}, {"ANY", 6}
};

vector<Symbol> SYMTAB;
vector<Literal> LITTAB;
vector<int> POOLTAB;
vector<IntermediateCode> IC;
int LC = 0;

int searchSymbol(string name) {
    for (int i = 0; i < SYMTAB.size(); i++) {
        if (SYMTAB[i].name == name)
            return i;
    }
    return -1;
}

void addSymbol(string name, int address = -1) {
    int idx = searchSymbol(name);
    if (idx == -1) {
        SYMTAB.push_back({name, address});
    } else if (address != -1 && SYMTAB[idx].address == -1) {
        SYMTAB[idx].address = address;
    }
}

int searchLiteral(string value) {
    for (int i = 0; i < LITTAB.size(); i++) {
        if (LITTAB[i].value == value)
            return i;
    }
    return -1;
}

void addLiteral(string value) {
    if (searchLiteral(value) == -1) {
        LITTAB.push_back({value, -1}); // Address is -1 until LTORG/END
    }
}

void processLiterals() {
    // Process all unprocessed literals in current pool (those with address = -1)
    int poolStart = POOLTAB.empty() ? 0 : POOLTAB.back();
    
    for (int i = poolStart; i < LITTAB.size(); i++) {
        if (LITTAB[i].address == -1) {
            LITTAB[i].address = LC;
            // Extract numeric value from literal (e.g., ='5' -> 5)
            string litValue = LITTAB[i].value;
            litValue = litValue.substr(2, litValue.length() - 3); // Remove =' and '
            IC.push_back({LC, "(DL,01) (C," + litValue + ")"});
            LC++;
        }
    }
    // Mark the end of current pool
    POOLTAB.push_back(LITTAB.size());
}

bool isLiteral(string str) {
    return str.length() > 0 && str[0] == '=';
}

int evaluateExpression(string expr) {
    // Simple expression evaluator for EQU and ORIGIN
    // Handles: number, symbol, symbol+number, symbol-number
    if (isdigit(expr[0])) {
        return stoi(expr);
    }
    
    size_t plusPos = expr.find('+');
    size_t minusPos = expr.find('-');
    
    if (plusPos != string::npos) {
        string sym = expr.substr(0, plusPos);
        int offset = stoi(expr.substr(plusPos + 1));
        int symIdx = searchSymbol(sym);
        if (symIdx != -1 && SYMTAB[symIdx].address != -1) {
            return SYMTAB[symIdx].address + offset;
        }
    } else if (minusPos != string::npos && minusPos > 0) {
        string sym = expr.substr(0, minusPos);
        int offset = stoi(expr.substr(minusPos + 1));
        int symIdx = searchSymbol(sym);
        if (symIdx != -1 && SYMTAB[symIdx].address != -1) {
            return SYMTAB[symIdx].address - offset;
        }
    } else {
        int symIdx = searchSymbol(expr);
        if (symIdx != -1 && SYMTAB[symIdx].address != -1) {
            return SYMTAB[symIdx].address;
        }
    }
    return -1;
}

void processLine(string line) {
    if (line.empty()) return;
    
    string label, opcode, op1, op2;
    stringstream ss(line);
    string first;
    ss >> first;

    // Check if first token is a label or opcode
    if (OPTAB.find(first) == OPTAB.end()) {
        label = first;
        ss >> opcode;
    } else {
        opcode = first;
        label = "";
    }

    ss >> op1 >> op2;

    // Handle START
    if (opcode == "START") {
        LC = stoi(op1);
        IC.push_back({LC, "(AD,01) (C," + op1 + ")"});
        POOLTAB.push_back(0); // Initialize first pool at literal index 0
        return;
    }

    // Add label to symbol table if present (except for EQU)
    if (!label.empty() && opcode != "EQU") {
        addSymbol(label, LC);
    }

    // Handle END
    if (opcode == "END") {
        processLiterals(); // Process all remaining literals
        IC.push_back({LC, "(AD,02)"});
        return;
    }

    // Handle LTORG
    if (opcode == "LTORG") {
        IC.push_back({LC, "(AD,05)"});
        processLiterals(); // Process all pending literals
        return;
    }

    // Handle ORIGIN
    if (opcode == "ORIGIN") {
        int newLC = evaluateExpression(op1);
        if (newLC != -1) {
            LC = newLC;
            IC.push_back({LC, "(AD,03) (C," + to_string(LC) + ")"});
        }
        return;
    }

    // Handle EQU
    if (opcode == "EQU") {
        int value = evaluateExpression(op1);
        if (value != -1 && !label.empty()) {
            addSymbol(label, value);
            IC.push_back({LC, "(AD,04) (S," + to_string(searchSymbol(label) + 1) + ")"});
        }
        return;
    }

    // Handle DS
    if (opcode == "DS") {
        string type = OPTAB[opcode].first;
        string code = OPTAB[opcode].second;
        IC.push_back({LC, "(" + type + "," + code + ") (C," + op1 + ")"});
        LC += stoi(op1);
        return;
    }

    // Handle DC
    if (opcode == "DC") {
        string type = OPTAB[opcode].first;
        string code = OPTAB[opcode].second;
        IC.push_back({LC, "(" + type + "," + code + ") (C," + op1 + ")"});
        LC += 1;
        return;
    }

    // Handle imperative statements
    string type = OPTAB[opcode].first;
    string code = OPTAB[opcode].second;
    string ic = "(" + type + "," + code + ")";

    // Handle register operand
    if (REGTAB.find(op1) != REGTAB.end()) {
        ic += " (" + to_string(REGTAB[op1]) + ")";
    } else if (COND.find(op1) != COND.end()) {
        ic += " (" + to_string(COND[op1]) + ")";
    }

    // Handle second operand (symbol or literal)
    if (!op2.empty()) {
        if (isLiteral(op2)) {
            addLiteral(op2); // Just add to LITTAB, address assigned later
            int litIdx = searchLiteral(op2);
            ic += " (L," + to_string(litIdx + 1) + ")";
        } else {
            addSymbol(op2);
            int symIdx = searchSymbol(op2);
            ic += " (S," + to_string(symIdx + 1) + ")";
        }
    } else if (!op1.empty() && REGTAB.find(op1) == REGTAB.end() && COND.find(op1) == COND.end()) {
        // First operand is a symbol or literal
        if (isLiteral(op1)) {
            addLiteral(op1); // Just add to LITTAB, address assigned later
            int litIdx = searchLiteral(op1);
            ic += " (L," + to_string(litIdx + 1) + ")";
        } else {
            addSymbol(op1);
            int symIdx = searchSymbol(op1);
            ic += " (S," + to_string(symIdx + 1) + ")";
        }
    }

    IC.push_back({LC, ic});
    LC++;
}

int main() {
    // Example program with literals
    vector<string> program = {
        "START 200",
        "MOVER AREG ='5'",
        "MOVEM AREG X",
        "ADD AREG ='10'",
        "LTORG",
        "LOOP MOVER BREG ='3'",
        "ADD BREG Y",
        "MOVEM BREG Z",
        "X DS 1",
        "Y DS 1", 
        "Z DS 1",
        "END"
    };

    cout << "=== Starting Pass I Assembler ===\n\n";
    cout << "Source Program:\n";
    for (auto &line : program) {
        cout << line << "\n";
    }
    cout << "\n";

    for (string line : program) {
        processLine(line);
    }

    // Write output files
    ofstream icfile("IC.txt"), symfile("SYMTAB.txt"), litfile("LITTAB.txt"), poolfile("POOLTAB.txt");

    cout << "Intermediate Code:\n";
    cout << "LC\tCode\n";
    cout << "---\t----\n";
    for (auto &i : IC) {
        icfile << i.lc << "\t" << i.code << "\n";
        cout << i.lc << "\t" << i.code << "\n";
    }

    cout << "\nSymbol Table:\n";
    cout << "Index\tSymbol\tAddress\n";
    cout << "-----\t------\t-------\n";
    for (int i = 0; i < SYMTAB.size(); i++) {
        symfile << i + 1 << "\t" << SYMTAB[i].name << "\t" << SYMTAB[i].address << "\n";
        cout << i + 1 << "\t" << SYMTAB[i].name << "\t" << SYMTAB[i].address << "\n";
    }

    if (!LITTAB.empty()) {
        cout << "\nLiteral Table:\n";
        cout << "Index\tLiteral\tAddress\n";
        cout << "-----\t-------\t-------\n";
        for (int i = 0; i < LITTAB.size(); i++) {
            litfile << i + 1 << "\t" << LITTAB[i].value << "\t" << LITTAB[i].address << "\n";
            cout << i + 1 << "\t" << LITTAB[i].value << "\t" << LITTAB[i].address << "\n";
        }

        cout << "\nPool Table:\n";
        cout << "Pool#\tLiteral Start Index\n";
        cout << "-----\t-------------------\n";
        for (int i = 0; i < POOLTAB.size(); i++) {
            poolfile << i + 1 << "\t" << POOLTAB[i] << "\n";
            cout << i + 1 << "\t" << POOLTAB[i] << "\n";
        }
    }

    icfile.close();
    symfile.close();
    litfile.close();
    poolfile.close();

    cout << "\n=== Pass I Complete ===\n";
    cout << "Files generated: IC.txt, SYMTAB.txt, LITTAB.txt, POOLTAB.txt\n";
    cout << "\nNote: Literals are assigned addresses ONLY at LTORG or END\n";
    
    return 0;
}