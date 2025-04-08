#include "item_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

void ItemParser::parse_items(const std::string& file_name, std::vector<Item>& items) {
    std::ifstream file(file_name);
    if (!file) {
        std::cerr << "Error: Cannot open file " << file_name << std::endl;
        return;
    }
    std::string line;
    Item item;
    getline(file, line);
    if (line != "RLG327 OBJECT DESCRIPTION 1") {
        std::cout << "Invalid file header.\n";
        return;
    }
    while (std::getline(file, line)) {
        if (line.find("BEGIN OBJECT") != std::string::npos) {
            parse_item_fields(file, item);
            items.push_back(item);
        }
    }

    file.close();
}

void ItemParser::parse_item_fields(std::ifstream& file, Item& item) {
    std::string line;

    item.name = item.type = item.color = item.description = item.hit = item.damage = item.dodge =
    item.defense = item.weight = item.speed = item.special = item.value = "";
    item.artifact = false;
    item.rarity = 0;

    while (std::getline(file, line)) {
        if (line.find("END") != std::string::npos) {
            break;
        }
        
        // Parsing the lines for fields
        std::smatch match;
        if (std::regex_search(line, match, std::regex("NAME (.*)"))) {
            item.name = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("TYPE (.*)"))) {
            item.type = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("COLOR (.*)"))) {
            item.color = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("DESC (.*)"))) {
            item.description = match.str(1);
            // Read description over multiple lines
            while (std::getline(file, line) && line.find(".") == std::string::npos) {
                item.description += " " + line;
            }
        }
        else if (std::regex_search(line, match, std::regex("HIT (.*)"))) {
            item.hit = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("DAM (.*)"))) {
            item.damage = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("DODGE (.*)"))) {
            item.dodge = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("DEF (.*)"))) {
            item.defense = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("WEIGHT (.*)"))) {
            item.weight = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("SPEED (.*)"))) {
            item.speed = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("ATTR (.*)"))) {
            item.special = match.str(1);
        }
        else if (std::regex_search(line, match, std::regex("ART (TRUE|FALSE)"))) {
            item.artifact = (match.str(1) == "TRUE");
        }
        else if (std::regex_search(line, match, std::regex("RRTY (\\d+)"))) {
            item.rarity = std::stoi(match.str(1));
        }
        else if (std::regex_search(line, match, std::regex("VAL (.*)"))) {
            item.value = match.str(1);
        }
    }
}

void ItemParser::print_item(const Item& item) {
    std::cout <<item.name << std::endl;
    std::cout <<item.type << std::endl;
    std::cout <<item.color << std::endl;
    std::cout <<item.description << std::endl;
    std::cout <<item.hit << std::endl;
    std::cout <<item.damage << std::endl;
    std::cout <<item.dodge << std::endl;
    std::cout <<item.defense << std::endl;
    std::cout <<item.weight << std::endl;
    std::cout <<item.speed << std::endl;
    std::cout <<item.special << std::endl;
    std::cout <<(item.artifact ? "TRUE" : "FALSE") << std::endl;
    std::cout <<item.rarity << std::endl;
    std::cout <<item.value << std::endl;
    std::cout << std::endl;
}
