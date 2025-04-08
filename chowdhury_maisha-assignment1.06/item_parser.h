#ifndef ITEM_PARSER_H
#define ITEM_PARSER_H

#include <string>
#include <vector>
#include <iostream>

struct Item {
    std::string name;
    std::string type;
    std::string color;
    std::string description;
    std::string hit;
    std::string damage;
    std::string dodge;
    std::string defense;
    std::string weight;
    std::string speed;
    std::string special;
    bool artifact;
    int rarity;
    std::string value;
};

class ItemParser {
public:
    static void parse_items(const std::string& file_name, std::vector<Item>& items);
    static void print_item(const Item& item);
    static void parse_item_fields(std::ifstream& file, Item& item);
};

#endif
