#include "monster_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

class MonsterDescription {
public:
    string name, description, symbol;
    vector<string> color, abilities;
    Dice speed, hp, damage;
    int rarity = 0;
    bool valid = false;

    void print() const {
        cout << name << endl;
        cout << description;
        cout << symbol << endl;
        for (const auto& c : color) cout << c << " ";
        cout << endl;
        cout << speed.to_string() << endl;
        for (const auto& a : abilities) cout << a << " ";
        cout << endl;
        cout << hp.to_string() << endl;
        cout << damage.to_string() << endl;
        cout << rarity << endl << endl;
    }
};

vector<string> split(const string& s) {
    vector<string> tokens;
    istringstream iss(s);
    string word;
    while (iss >> word) tokens.push_back(word);
    return tokens;
}

bool parse_monsters(const string& filename, vector<Monsterr>& monsters) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Failed to open file.\n";
        return false;
    }

    getline(file, line);
    if (line != "RLG327 MONSTER DESCRIPTION 1") {
        cerr << "Invalid file header.\n";
        return false;
    }

    MonsterDescription monster;
    map<string, bool> seen;
    bool inMonster = false;

    while (getline(file, line)) {
        if (line == "BEGIN MONSTER") {
            monster = MonsterDescription();
            seen.clear();
            inMonster = true;
            continue;
        }

        if (line == "END" && inMonster) {
            //cout << "Seen size: " << seen.size() << endl;
            //for (auto& p : seen) cout << p.first << " ";
            //cout << endl;
            if (seen.size() == 9) {
                //cout << "Monster parsed: " << monster.name << endl;
                //monster.print();
                monster.valid = true;
                Monsterr m;
                m.name = monster.name;
                m.description = monster.description;
                m.symbol = monster.symbol;
                m.color = !monster.color.empty() ? monster.color[0] : "";
                m.abilities = !monster.abilities.empty() ? monster.abilities[0] : "";
                m.speed = monster.speed.to_string();
                m.hp = monster.hp.to_string();
                m.damage = monster.damage.to_string();
                m.rarity = monster.rarity;
                monsters.push_back(m);
                monster.print();
            }
            inMonster = false;
            continue;
        }

        if (!inMonster) continue;

        istringstream ss(line);
        string keyword;
        ss >> keyword;
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);


        try {
            if (keyword == "NAME" && !seen["NAME"]) {
                getline(ss, monster.name);
                monster.name = monster.name.substr(1); 
                seen["NAME"] = true;
            } else if (keyword == "DESC" && !seen["DESC"]) {
                string desc, temp;
                while (getline(file, temp)) {
                    if (temp == ".") break;
                    desc += temp + "\n";
                }
                monster.description = desc;
                seen["DESC"] = true;
            } else if (keyword == "SYMB" && !seen["SYMB"]) {
                ss >> monster.symbol;
                seen["SYMB"] = true;
            } else if (keyword == "COLOR" && !seen["COLOR"]) {
                string rest;
                getline(ss, rest);
                monster.color = split(rest);
                seen["COLOR"] = true;
            } else if (keyword == "ABIL" && !seen["ABIL"]) {
                string rest;
                getline(ss, rest);
                monster.abilities = split(rest);
                seen["ABIL"] = true;
            } else if (keyword == "SPEED" && !seen["SPEED"]) {
                string val; ss >> val;
                monster.speed = Dice(val);
                seen["SPEED"] = true;
            } else if (keyword == "HP" && !seen["HP"]) {
                string val; ss >> val;
                monster.hp = Dice(val);
                seen["HP"] = true;
            } else if (keyword == "DAM" && !seen["DAM"]) {
                string val; ss >> val;
                monster.damage = Dice(val);
                seen["DAM"] = true;
            } else if (keyword == "RRTY" && !seen["RRTY"]) {
                ss >> monster.rarity;
                seen["RRTY"] = true;
            } else {
                inMonster = false;
                while (getline(file, line) && line != "END"); 
            }
        } catch (...) {
            inMonster = false;
            while (getline(file, line) && line != "END"); 
        }
    }

    return true;
}
bool save_monsters(const std::string& filename, const std::vector<Monsterr>& monsters) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving.\n";
        return false;
    }

    file << "RLG327 MONSTER DESCRIPTION 1\n";

    for (const auto& monster : monsters) {
        file << "BEGIN MONSTER\n";
        file << "NAME " << monster.name << "\n";
        file << "DESC " << monster.description << ".\n"; 
        file << "COLOR " << monster.color << "\n";
        file << "SYMB " << monster.symbol << "\n";
        file << "SPEED " << monster.speed << "\n";
        file << "HP " << monster.hp << "\n";
        file << "DAM " << monster.damage << "\n";
        file << "RRTY " << monster.rarity << "\n";
        file << "ABIL " << monster.abilities << "\n";
        file << "END\n";
    }

    return true;
}
