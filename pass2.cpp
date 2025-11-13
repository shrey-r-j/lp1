#include <bits/stdc++.h>
using namespace std;

int main() {
    ifstream ic("IC.txt");
    ifstream symtab("SYMTAB.txt");
    ifstream littab("LITTAB.txt");
    ofstream mcode("machinecode.txt");

    if (!ic || !symtab || !littab) {
        cout << "Error: Missing one of the input files\n";
        return 1;
    }

    unordered_map<int, int> SYMTAB;
    unordered_map<int, int> LITTAB;

    int idx, addr;
    string name;

    while (symtab >> idx >> name >> addr)
        SYMTAB[idx] = addr;

    while (littab >> idx >> name >> addr)
        LITTAB[idx] = addr;

    string line;
    while (getline(ic, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        int LC;
        ss >> LC;

        vector<string> parts;
        string tok;
        while (ss >> tok) parts.push_back(tok);
        if (parts.empty()) continue;

        string op = parts[0];

        // ----------------- AD (Assembler Directive)
        if (op.find("(AD") != string::npos) {
            mcode << LC << " : " << "No Machine Code\n";
            continue;
        }

        // ----------------- DL (Declarative Statement)
        if (op.find("(DL") != string::npos) {
            string dlcode = op.substr(4, 2); // e.g. (DL,01)
            int dltype = stoi(dlcode);

            if (dltype == 1 && parts.size() > 1 && parts[1].find("(C,") != string::npos) {
                // DC (Define Constant)
                int val = stoi(parts[1].substr(3, parts[1].size() - 4));
                mcode << LC << " : 00  00  "
                      << setw(3) << setfill('0') << val << "\n";
            } else if (dltype == 2) {
                // DS (Define Storage)
                mcode << LC << " : 00  00  000\n";
            }
            continue;
        }

        // ----------------- IS (Imperative Statement)
        if (op.find("(IS") != string::npos) {
            int opcode = stoi(op.substr(4, 2));
            int regCode = 0, address = 0;

            for (size_t i = 1; i < parts.size(); ++i) {
                string &p = parts[i];

                if (p.size() >= 3 && p.front() == '(' && p.back() == ')' && isdigit(p[1])) {
                    regCode = stoi(p.substr(1, p.size() - 2));
                }
                else if (p.find("(S,") != string::npos) {
                    int symIndex = stoi(p.substr(3, p.size() - 4));
                    address = SYMTAB[symIndex];
                }
                else if (p.find("(L,") != string::npos) {
                    int litIndex = stoi(p.substr(3, p.size() - 4));
                    address = LITTAB[litIndex];
                }
                else if (p.find("(C,") != string::npos) {
                    address = stoi(p.substr(3, p.size() - 4));
                }
            }

            mcode << LC << " : "
                  << setw(2) << setfill('0') << opcode << "  "
                  << setw(2) << regCode << "  "
                  << setw(3) << setfill('0') << address << "\n";
        }
    }

    cout << "Machine code generated in machinecode.txt\n";
    return 0;
}
