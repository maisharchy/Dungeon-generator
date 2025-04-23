#ifndef IO_H
# define IO_H

#include "object.h"

class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
//new
void monster_selection(dungeon *d);
uint32_t destroy_equipment(dungeon *d);
uint32_t look_at_inventory(dungeon *d);
void show_equipment_inventory(dungeon *d);
uint32_t remove_equipment(dungeon *d);
void open_inventory(dungeon *d);
uint32_t put_equipment_on(dungeon *d);
uint32_t show_object_details(object *o);
uint32_t prompt_inventory_drop(dungeon *d);
uint32_t inspect_equipped_items(dungeon *d);
void convert_object_to_string(object *o, char *s, uint32_t size);
//
#endif
