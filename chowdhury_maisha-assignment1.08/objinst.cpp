#include <vector>
#include <cstring>
#include <stdlib.h>

#include "object.h"
#include "dungeon.h"
#include "utils.h"

void gen_object(dungeon_t *d){
    d->num_object = (d->object_descriptions.size() < 10) ? 
                     d->object_descriptions.size() : 10;

    for (uint32_t i = 0; i < d->num_object; ++i){
        object *o = d->object_descriptions.at(i).gen_object();

        uint32_t r = rand_range(0, d->num_rooms - 1);
        int y_start = d->rooms[r].position[dim_y];
        int y_end = y_start + d->rooms[r].size[dim_y] - 1;
        int x_start = d->rooms[r].position[dim_x];
        int x_end = x_start + d->rooms[r].size[dim_x] - 1;

        int y = rand_range(y_start, y_end);
        int x = rand_range(x_start, x_end);

        o->position[dim_y] = y;
        o->position[dim_x] = x;

        d->object_map[y][x] = o;
    }
}

int16_t *object_get_pos(object *o){
    return o->position;
}

void destroy_objects(dungeon_t *d){
    for (int y = 0; y < DUNGEON_Y; ++y){
        for (int x = 0; x < DUNGEON_X; ++x){
            object *obj = d->object_map[y][x];
            if (obj){
                delete obj;
                d->object_map[y][x] = nullptr;
            }
        }
    }
}
