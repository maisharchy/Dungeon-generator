#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include "dice.h"
#include "character.h"
#include "descriptions.h"

class object {
public:
    std::string name;
    std::string description;
    char symbol;
    object_type_t type;
    uint32_t color;
    pair_t position;
    dice damage;

    int32_t weight;
    int32_t speed;
    int32_t attribute;
    int32_t value;
    int32_t hit;
    int32_t dodge;
    int32_t defense;

    bool seen;

    object() = default;
};

void gen_objects(dungeon_t *d);
void destroy_objects(dungeon_t *d);
int16_t *object_get_pos(object *o);

#endif
