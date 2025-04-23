#include <vector>
#include <cstring>

#include "object.h"
#include "dungeon.h"
#include "utils.h"

object::object(object_description &o, pair_t p, object *next) :
  name(o.get_name()),
  description(o.get_description()),
  type(o.get_type()),
  color(o.get_color()),
  damage(o.get_damage()),
  hit(o.get_hit().roll()),
  dodge(o.get_dodge().roll()),
  defence(o.get_defence().roll()),
  weight(o.get_weight().roll()),
  speed(o.get_speed().roll()),
  attribute(o.get_attribute().roll()),
  value(o.get_value().roll()),
  seen(false),
  next(next),
  od(o)
{
  position[dim_x] = p[dim_x];
  position[dim_y] = p[dim_y];

  od.generate();
}

object::~object()
{
  od.destroy();
  if (next) {
    delete next;
  }
}

void gen_object(dungeon *d)
{
  object *o;
  uint32_t room;
  pair_t p;
  std::vector<object_description> &v = d->object_descriptions;
  int i;

  do {
    i = rand_range(0, v.size() - 1);
  } while (!v[i].can_be_generated() || !v[i].pass_rarity_roll());
  
  room = rand_range(0, d->num_rooms - 1);
  do {
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                          (d->rooms[room].position[dim_x] +
                           d->rooms[room].size[dim_x] - 1));
  } while (mappair(p) > ter_stairs);

  o = new object(v[i], p, d->objmap[p[dim_y]][p[dim_x]]);

  d->objmap[p[dim_y]][p[dim_x]] = o;
  
}

void gen_objects(dungeon *d)
{
  uint32_t i;

  memset(d->objmap, 0, sizeof (d->objmap));

  for (i = 0; i < d->max_objects; i++) {
    gen_object(d);
  }

  d->num_objects = d->max_objects;
}

char object::get_symbol()
{
  return next ? '&' : object_symbol[type];
}

uint32_t object::get_color()
{
  return color;
}

const char *object::get_name()
{
  return name.c_str();
}

int32_t object::get_speed()
{
  return speed;
}

int32_t object::roll_dice()
{
  return damage.roll();
}

void destroy_objects(dungeon *d)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->objmap[y][x]) {
        delete d->objmap[y][x];
        d->objmap[y][x] = 0;
      }
    }
  }
}

int32_t object::get_type()
{
  return type;
}
//new here
uint32_t object::can_be_equipped() const {
  return (type >= objtype_WEAPON && type <= objtype_RING);
}

uint32_t object::can_be_removed() const {
  return static_cast<uint32_t>(true);
}

uint32_t object::is_droppable() const {
  return 1U; 
}

uint32_t object::can_be_destroyed() const {
  return true;
}

int32_t object::equipment_slot_index() const {
  switch (type) {
    case objtype_WEAPON:
    case objtype_OFFHAND:
    case objtype_RANGED:
    case objtype_ARMOR:
    case objtype_HELMET:
    case objtype_CLOAK:
    case objtype_GLOVES:
    case objtype_BOOTS:
    case objtype_AMULET:
    case objtype_LIGHT:
    case objtype_RING:
      return static_cast<int32_t>(type) - static_cast<int32_t>(objtype_WEAPON);
    default:
      return -1;
  }
}
void object::stack_onto_tile(dungeon *d, const int16_t *coord) {
  int row = coord[dim_y];
  int col = coord[dim_x];

  object *previous = reinterpret_cast<object *>(d->objmap[row][col]);

  this->next = previous;
  d->objmap[row][col] = this;
}
