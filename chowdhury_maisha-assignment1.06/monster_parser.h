#ifndef MONSTER_PARSER_H
#define MONSTER_PARSER_H

#include <string>
#include <vector>

class Dice {
public:
    int base, dice, sides;

    Dice() : base(0), dice(0), sides(0) {}
    Dice(std::string s) {
        sscanf(s.c_str(), "%d+%dd%d", &base, &dice, &sides);
    }

    std::string to_string() const {
        return std::to_string(base) + "+" + std::to_string(dice) + "d" + std::to_string(sides);
    }
};

struct Monsterr {
    std::string name;
    std::string description;
    std::string symbol;
    std::string color;
    std::string abilities;
    std::string speed;
    std::string hp;
    std::string damage;
    int rarity;
};

bool parse_monsters(const std::string& filename, std::vector<Monsterr>& monsters);
bool save_monsters(const std::string& filename, const std::vector<Monsterr>& monsters);

#endif
