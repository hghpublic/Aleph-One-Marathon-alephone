/*
LUA_MAP.CPP

	Copyright (C) 2008 by Gregory Smith
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This license is contained in the file "COPYING",
	which is included with this source code; it is available online at
	http://www.gnu.org/licenses/gpl.html

	Implements Lua map classes
*/

#include "lua_map.h"
#include "lua_templates.h"
#include "lightsource.h"
#include "map.h"
#include "platforms.h"
#include "OGL_Setup.h"

#ifdef HAVE_LUA

const char *Lua_DamageTypes::name = "DamageTypes";

const luaL_reg Lua_DamageTypes::metatable[] = {
	{"__call", L_GlobalCall<Lua_DamageTypes, Lua_DamageType>},
	{"__index", L_GlobalIndex<Lua_DamageTypes, Lua_DamageType>},
	{"__newindex}", L_GlobalNewindex<Lua_DamageTypes>},
	{0, 0}
};

const luaL_reg Lua_DamageTypes::methods[] = {
	{0, 0}
};

const char *Lua_DamageType::name = "damage_type";

const luaL_reg Lua_DamageType::metatable[] = {
	{"__index", L_TableGet<Lua_DamageType>},
	{"__newindex", L_TableSet<Lua_DamageType>},
	{0, 0}
};

const luaL_reg Lua_DamageType::index_table[] = {
	{"index", L_TableIndex<Lua_DamageType>},
	{0, 0}
};

const luaL_reg Lua_DamageType::newindex_table[] = {
	{0, 0}
};

int Lua_Platform::get_active(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushboolean(L, PLATFORM_IS_ACTIVE(platform));
	return 1;
}

int Lua_Platform::get_ceiling_height(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushnumber(L, (double) platform->ceiling_height / WORLD_ONE);
	return 1;
}

int Lua_Platform::get_contracting(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushboolean(L, !PLATFORM_IS_EXTENDING(platform));
	return 1;
}

int Lua_Platform::get_extending(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushboolean(L, PLATFORM_IS_EXTENDING(platform));
	return 1;
}

int Lua_Platform::get_floor_height(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushnumber(L, (double) platform->floor_height / WORLD_ONE);
	return 1;
}

int Lua_Platform::get_monster_controllable(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushboolean(L, PLATFORM_IS_MONSTER_CONTROLLABLE(platform));
	return 1;
}

int Lua_Platform::get_player_controllable(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushboolean(L, PLATFORM_IS_PLAYER_CONTROLLABLE(platform));
	return 1;
}

int Lua_Platform::get_polygon(lua_State *L)
{
	L_Push<Lua_Polygon>(L, get_platform_data(L_Index<Lua_Platform>(L, 1))->polygon_index);
	return 1;
}

int Lua_Platform::get_speed(lua_State *L)
{
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	lua_pushnumber(L, (double) platform->speed * TICKS_PER_SECOND / WORLD_ONE);
	return 1;
}

int Lua_Platform::set_active(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "active: incorrect argument type");

	short platform_index = L_Index<Lua_Platform>(L, 1);
	try_and_change_platform_state(platform_index, lua_toboolean(L, 2));
	return 0;
}

int Lua_Platform::set_ceiling_height(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "ceiling_height: incorrect argument type");
	
	short platform_index = L_Index<Lua_Platform>(L, 1);
	platform_data *platform = get_platform_data(platform_index);

	platform->ceiling_height = static_cast<world_distance>(lua_tonumber(L, 2) * WORLD_ONE);
	adjust_platform_endpoint_and_line_heights(platform_index);
	adjust_platform_for_media(platform_index, false);
	
	return 0;
}	

int Lua_Platform::set_contracting(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "contracting: incorrect argument type");

	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	bool contracting = lua_toboolean(L, 2);
	if (contracting)
		SET_PLATFORM_IS_CONTRACTING(platform);
	else
		SET_PLATFORM_IS_EXTENDING(platform);
	return 0;
}

int Lua_Platform::set_extending(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "extending: incorrect argument type");

	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	bool extending = lua_toboolean(L, 2);
	if (extending)
		SET_PLATFORM_IS_EXTENDING(platform);
	else
		SET_PLATFORM_IS_CONTRACTING(platform);
	return 0;
}

int Lua_Platform::set_floor_height(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "ceiling_height: incorrect argument type");
	
	short platform_index = L_Index<Lua_Platform>(L, 1);
	platform_data *platform = get_platform_data(platform_index);

	platform->floor_height = static_cast<world_distance>(lua_tonumber(L, 2) * WORLD_ONE);
	adjust_platform_endpoint_and_line_heights(platform_index);
	adjust_platform_for_media(platform_index, false);
	
	return 0;
}	

int Lua_Platform::set_monster_controllable(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "monster_controllable: incorrect argument type");
	
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	SET_PLATFORM_IS_MONSTER_CONTROLLABLE(platform, lua_toboolean(L, 2));
	return 0;
}

int Lua_Platform::set_player_controllable(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "monster_controllable: incorrect argument type");
	
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	SET_PLATFORM_IS_PLAYER_CONTROLLABLE(platform, lua_toboolean(L, 2));
	return 0;
}

int Lua_Platform::set_speed(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "speed: incorrect argument type");
	
	platform_data *platform = get_platform_data(L_Index<Lua_Platform>(L, 1));
	platform->speed = static_cast<int>(lua_tonumber(L, 2) * WORLD_ONE / TICKS_PER_SECOND);
	return 0;
}
	

const char *Lua_Platform::name = "platform";

const luaL_reg Lua_Platform::index_table[] = {
	{"active", Lua_Platform::get_active},
	{"ceiling_height", Lua_Platform::get_ceiling_height},
	{"contracting", Lua_Platform::get_contracting},
	{"extending", Lua_Platform::get_extending},
	{"floor_height", Lua_Platform::get_floor_height},
	{"index", L_TableIndex<Lua_Platform>},
	{"monster_controllable", Lua_Platform::get_monster_controllable},
	{"player_controllable", Lua_Platform::get_player_controllable},
	{"polygon", Lua_Platform::get_polygon},
	{"speed", Lua_Platform::get_speed},
	{0, 0}
};

const luaL_reg Lua_Platform::newindex_table[] = {
	{"active", Lua_Platform::set_active},
	{"ceiling_height", Lua_Platform::set_ceiling_height},
	{"contracting", Lua_Platform::set_contracting},
	{"extending", Lua_Platform::set_extending},
	{"floor_height", Lua_Platform::set_floor_height},
	{"monster_controllable", Lua_Platform::set_monster_controllable},
	{"player_controllable", Lua_Platform::set_player_controllable},
	{"speed", Lua_Platform::set_speed},
	{0, 0}
};

const luaL_reg Lua_Platform::metatable[] = {
	{"__index", L_TableGet<Lua_Platform>},
	{"__newindex", L_TableSet<Lua_Platform>},
	{0, 0}
};

struct Lua_Polygon_Floor {
	short index;
	static const char *name;
	static bool valid(int index) { return Lua_Polygons::valid(index); }

	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];

	static int get_height(lua_State *L);
	static int set_height(lua_State *L);
};

const char* Lua_Polygon_Floor::name = "polygon_floor";

const luaL_reg Lua_Polygon_Floor::index_table[] = {
	{"height", Lua_Polygon_Floor::get_height},
	{"z", Lua_Polygon_Floor::get_height},
	{0, 0}
};

const luaL_reg Lua_Polygon_Floor::newindex_table[] = {
	{"height", Lua_Polygon_Floor::set_height},
	{"z", Lua_Polygon_Floor::set_height},
	{0, 0}
};

const luaL_reg Lua_Polygon_Floor::metatable[] = {
	{"__index", L_TableGet<Lua_Polygon_Floor>},
	{"__newindex", L_TableSet<Lua_Polygon_Floor>},
	{0, 0}
};

int Lua_Polygon_Floor::get_height(lua_State *L)
{
	lua_pushnumber(L, (double) (get_polygon_data(L_Index<Lua_Polygon_Floor>(L, 1))->floor_height) / WORLD_ONE);
	return 1;
}

int Lua_Polygon_Floor::set_height(lua_State *L)
{
	if (!lua_isnumber(L, 2))
	{
		luaL_error(L, "height: incorrect argument type");
	}

	struct polygon_data *polygon = get_polygon_data(L_Index<Lua_Polygon_Floor>(L, 1));
	polygon->floor_height = static_cast<world_distance>(lua_tonumber(L,2)*WORLD_ONE);
	for (short i = 0; i < polygon->vertex_count; ++i)
	{
		recalculate_redundant_endpoint_data(polygon->endpoint_indexes[i]);
		recalculate_redundant_line_data(polygon->line_indexes[i]);
	}
	return 0;
}

struct Lua_Polygon_Ceiling {
	short index;
	static const char *name;
	static bool valid(int index) { return Lua_Polygons::valid(index); }

	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];

	static int get_height(lua_State *L);
	static int set_height(lua_State *L);
};

const char* Lua_Polygon_Ceiling::name = "polygon_ceiling";

const luaL_reg Lua_Polygon_Ceiling::index_table[] = {
	{"height", Lua_Polygon_Ceiling::get_height},
	{"z", Lua_Polygon_Ceiling::get_height},
	{0, 0}
};

const luaL_reg Lua_Polygon_Ceiling::newindex_table[] = {
	{"height", Lua_Polygon_Ceiling::set_height},
	{"z", Lua_Polygon_Ceiling::set_height},
	{0, 0}
};

const luaL_reg Lua_Polygon_Ceiling::metatable[] = {
	{"__index", L_TableGet<Lua_Polygon_Ceiling>},
	{"__newindex", L_TableSet<Lua_Polygon_Ceiling>},
	{0, 0}
};

int Lua_Polygon_Ceiling::get_height(lua_State *L)
{
	lua_pushnumber(L, (double) (get_polygon_data(L_Index<Lua_Polygon_Ceiling>(L, 1))->ceiling_height) / WORLD_ONE);
	return 1;
}

int Lua_Polygon_Ceiling::set_height(lua_State *L)
{
	if (!lua_isnumber(L, 2))
	{
		luaL_error(L, "height: incorrect argument type");
	}

	struct polygon_data *polygon = get_polygon_data(L_Index<Lua_Polygon_Ceiling>(L, 1));
	polygon->ceiling_height = static_cast<world_distance>(lua_tonumber(L,2)*WORLD_ONE);
	for (short i = 0; i < polygon->vertex_count; ++i)
	{
		recalculate_redundant_endpoint_data(polygon->endpoint_indexes[i]);
		recalculate_redundant_line_data(polygon->line_indexes[i]);
	}
	return 0;
}

int Lua_Polygon::get_ceiling(lua_State *L)
{
	L_Push<Lua_Polygon_Ceiling>(L, L_Index<Lua_Polygon>(L, 1));
	return 1;
}

int Lua_Polygon::get_floor(lua_State *L)
{
	L_Push<Lua_Polygon_Floor>(L, L_Index<Lua_Polygon>(L, 1));
	return 1;
}

int Lua_Polygon::get_type(lua_State *L)
{
	lua_pushnumber(L, get_polygon_data(L_Index<Lua_Polygon>(L, 1))->type);
	return 1;
}

int Lua_Polygon::get_platform(lua_State *L)
{
	polygon_data *polygon = get_polygon_data(L_Index<Lua_Polygon>(L, 1));
	if (polygon->type == _polygon_is_platform)
	{
		L_Push<Lua_Platform>(L, polygon->permutation);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

int Lua_Polygon::get_x(lua_State *L)
{
	lua_pushnumber(L, (double) get_polygon_data(L_Index<Lua_Polygon>(L, 1))->center.x / WORLD_ONE);
	return 1;
}

int Lua_Polygon::get_y(lua_State *L)
{
	lua_pushnumber(L, (double) get_polygon_data(L_Index<Lua_Polygon>(L, 1))->center.y / WORLD_ONE);
	return 1;
}

int Lua_Polygon::get_z(lua_State *L)
{
	lua_pushnumber(L, (double) get_polygon_data(L_Index<Lua_Polygon>(L, 1))->floor_height / WORLD_ONE);
	return 1;
}

int Lua_Polygon::set_type(lua_State *L)
{
	if (!lua_isnumber(L, 2))
	{
		luaL_error(L, "type: incorrect argument type");
	}

	int type = static_cast<int>(lua_tonumber(L, 2));
	get_polygon_data(L_Index<Lua_Polygon>(L, 1))->type = type;

	return 0;
}

const char *Lua_Polygon::name = "polygon";

const luaL_reg Lua_Polygon::index_table[] = {
	{"ceiling", Lua_Polygon::get_ceiling},
	{"floor", Lua_Polygon::get_floor},
	{"index", L_TableIndex<Lua_Polygon>},
	{"platform", Lua_Polygon::get_platform},
	{"type", Lua_Polygon::get_type},
	{"x", Lua_Polygon::get_x},
	{"y", Lua_Polygon::get_y},
	{"z", Lua_Polygon::get_z},
	{0, 0}
};

const luaL_reg Lua_Polygon::newindex_table[] = {
	{"type", Lua_Polygon::set_type},
	{0, 0}
};

const luaL_reg Lua_Polygon::metatable[] = {
	{"__index", L_TableGet<Lua_Polygon>},
	{"__newindex", L_TableSet<Lua_Polygon>},
	{0, 0}
};

const char *Lua_Polygons::name = "Polygons";
const luaL_reg Lua_Polygons::methods[] = {
	{0, 0}
};

const luaL_reg Lua_Polygons::metatable[] = {
	{"__index", L_GlobalIndex<Lua_Polygons, Lua_Polygon>},
	{"__newindex", L_GlobalNewindex<Lua_Polygons>},
	{"__len", L_GlobalLength<Lua_Polygons>},
	{"__call", L_GlobalCall<Lua_Polygons, Lua_Polygon>},
	{0, 0}
};

const char *Lua_Lights::name = "Lights";
const luaL_reg Lua_Lights::methods[] = {
	{0, 0}
};

const luaL_reg Lua_Lights::metatable[] = {
	{"__call", L_GlobalCall<Lua_Lights, Lua_Light>},
	{"__index", L_GlobalIndex<Lua_Lights, Lua_Light>},
	{"__len", L_GlobalLength<Lua_Lights>},
	{"__newindex", L_GlobalNewindex<Lua_Lights>},
	{0, 0}
};

const char *Lua_Light::name = "light";

int Lua_Light::get_active(lua_State *L)
{
	lua_pushboolean(L, get_light_status(L_Index<Lua_Light>(L, 1)));
	return 1;
}

int Lua_Light::set_active(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "active: incorrect argument type");

	size_t light_index = L_Index<Lua_Light>(L, 1);
	bool active = lua_toboolean(L, 2);
	
	set_light_status(light_index, active);
	assume_correct_switch_position(_panel_is_light_switch, static_cast<short>(light_index), active);
	return 0;
}

const luaL_reg Lua_Light::index_table[] = {
	{"active", Lua_Light::get_active},
	{"index", L_TableIndex<Lua_Light>},
	{0, 0}
};

const luaL_reg Lua_Light::newindex_table[] = {
	{"active", Lua_Light::set_active},
	{0, 0}
};

const luaL_reg Lua_Light::metatable[] = {
	{"__index", L_TableGet<Lua_Light>},
	{"__newindex", L_TableSet<Lua_Light>},
	{0, 0}
};

const char *Lua_Tags::name = "Tags";
const luaL_reg Lua_Tags::methods[] = {
	{0, 0}
};

const luaL_reg Lua_Tags::metatable[] = {
	{"__index", L_GlobalIndex<Lua_Tags, Lua_Tag>},
	{"__newindex", L_GlobalNewindex<Lua_Tags>},
	{0, 0}
};

const char *Lua_Tag::name = "tag";

int Lua_Tag::get_active(lua_State *L)
{
	int tag = L_Index<Lua_Tag>(L, 1);
	bool changed = false;

	size_t light_index;
	light_data *light;

	for (light_index= 0, light= lights; light_index<MAXIMUM_LIGHTS_PER_MAP && !changed; ++light_index, ++light)
	{
		if (light->static_data.tag==tag)
		{
			if (get_light_status(light_index))
			{
				changed= true;
			}
		}
	}

	short platform_index;
	platform_data *platform;

	for (platform_index= 0, platform= platforms; platform_index<dynamic_world->platform_count && !changed; ++platform_index, ++platform)
	{
		if (platform->tag==tag)
		{
			if (PLATFORM_IS_ACTIVE(platform))
			{
				changed= true;
			}
		}
	}

	lua_pushboolean(L, changed);
	return 1;
}

int Lua_Tag::set_active(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "active: incorrect argument type");

	int16 tag = L_Index<Lua_Tag>(L, 1);
	bool active = lua_toboolean(L, 2);

	set_tagged_light_statuses(tag, active);
	try_and_change_tagged_platform_states(tag, active);
	assume_correct_switch_position(_panel_is_tag_switch, tag, active);

	return 0;
}

const luaL_reg Lua_Tag::index_table[] = {
	{"active", Lua_Tag::get_active},
	{"index", L_TableIndex<Lua_Tag>},
	{0, 0}
};

const luaL_reg Lua_Tag::newindex_table[] = {
	{"active", Lua_Tag::set_active},
	{0, 0}
};

const luaL_reg Lua_Tag::metatable[] = {
	{"__index", L_TableGet<Lua_Tag>},
	{"__newindex", L_TableSet<Lua_Tag>},
	{0, 0}
};

struct Lua_Annotations {
	static const char *name;
	static const luaL_reg metatable[];
	static const luaL_reg methods[];
	static bool valid(int index) { return index >= 0 && index < dynamic_world->default_annotation_count; }
	static int length() { return MapAnnotationList.size(); }

	static int new_annotation(lua_State *L);
};

struct Lua_Annotation {
	short index;
	static const char *name;
	static bool valid(int index) { return index >= 0 && index < MapAnnotationList.size(); }
	
	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];

	static int get_polygon(lua_State *L);
	static int get_text(lua_State *L);
	static int get_x(lua_State *L);
	static int get_y(lua_State *L);

	static int set_polygon(lua_State *L);
	static int set_text(lua_State *L);
	static int set_x(lua_State *L);
	static int set_y(lua_State *L);
};

const char *Lua_Annotations::name = "Annotations";

// Annotations.new(polygon, text, [x, y])
int Lua_Annotations::new_annotation(lua_State *L)
{
	if (dynamic_world->default_annotation_count == INT16_MAX)
		return luaL_error(L, "new: annotation limit reached");

	if (!lua_isstring(L, 2))
		return luaL_error(L, "new: incorrect argument type");

	map_annotation annotation;
	annotation.type = 0;

	if (lua_isnil(L, 1))
	{
		annotation.polygon_index = NONE;
	}
	else
	{
		annotation.polygon_index = L_ToIndex<Lua_Polygon>(L, 1);
	}

	int x = 0, y = 0;
	if (annotation.polygon_index != NONE)
	{
		polygon_data *polygon = get_polygon_data(annotation.polygon_index);
		x = polygon->center.x;
		y = polygon->center.y;
	}
		
	annotation.location.x = luaL_optint(L, 3, x);
	annotation.location.y = luaL_optint(L, 4, y);

	strncpy(annotation.text, lua_tostring(L, 2), MAXIMUM_ANNOTATION_TEXT_LENGTH);
	annotation.text[MAXIMUM_ANNOTATION_TEXT_LENGTH-1] = '\0';
	
	MapAnnotationList.push_back(annotation);
	dynamic_world->default_annotation_count++;
	L_Push<Lua_Annotation>(L, MapAnnotationList.size() - 1);
	return 1;
}

const luaL_reg Lua_Annotations::methods[] = {
	{"new", Lua_Annotations::new_annotation},
	{0, 0}
};

const luaL_reg Lua_Annotations::metatable[] = {
	{"__call", L_GlobalCall<Lua_Annotations, Lua_Annotation>},
	{"__len", L_GlobalLength<Lua_Annotations>},
	{"__index", L_GlobalIndex<Lua_Annotations, Lua_Annotation>},
	{"__newindex", L_GlobalNewindex<Lua_Annotations>},
	{0, 0}
};

const char* Lua_Annotation::name = "annotation";

int Lua_Annotation::get_polygon(lua_State *L)
{
	L_Push<Lua_Polygon>(L, MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].polygon_index);
	return 1;
}

int Lua_Annotation::get_text(lua_State *L)
{
	lua_pushstring(L, MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].text);
	return 1;
}

int Lua_Annotation::get_x(lua_State *L)
{
	lua_pushnumber(L, (double) MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].location.x / WORLD_ONE);
	return 1;
}

int Lua_Annotation::get_y(lua_State *L)
{
	lua_pushnumber(L, (double) MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].location.y / WORLD_ONE);
	return 1;
}

const luaL_reg Lua_Annotation::index_table[] = {
	{"polygon", Lua_Annotation::get_polygon},
	{"text", Lua_Annotation::get_text},
	{"x", Lua_Annotation::get_x},
	{"y", Lua_Annotation::get_y},
	{0, 0}
};

int Lua_Annotation::set_polygon(lua_State *L)
{
	int polygon_index = NONE;
	if (lua_isnil(L, 2))
	{
		polygon_index = NONE;
	}
	else
	{
		polygon_index = L_ToIndex<Lua_Polygon>(L, 2);
	}

	MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].polygon_index = polygon_index;
	return 0;
}

int Lua_Annotation::set_text(lua_State *L)
{
	if (!lua_isstring(L, 2))
		return luaL_error(L, "text: incorrect argument type");

	int annotation_index = L_Index<Lua_Annotation>(L, 1);
	strncpy(MapAnnotationList[annotation_index].text, lua_tostring(L, 2), MAXIMUM_ANNOTATION_TEXT_LENGTH);
	MapAnnotationList[annotation_index].text[MAXIMUM_ANNOTATION_TEXT_LENGTH-1] = '\0';
	return 0;
}

int Lua_Annotation::set_x(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "x: incorrect argument type");
	
	MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].location.x = static_cast<int>(lua_tonumber(L, 2) * WORLD_ONE);
	return 0;
}

int Lua_Annotation::set_y(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "y: incorrect argument type");

	MapAnnotationList[L_Index<Lua_Annotation>(L, 1)].location.y = static_cast<int>(lua_tonumber(L, 2) * WORLD_ONE);
	return 0;
}

const luaL_reg Lua_Annotation::newindex_table[] = {
	{"polygon", Lua_Annotation::set_polygon},
	{"text", Lua_Annotation::set_text},
	{"x", Lua_Annotation::set_x},
	{"y", Lua_Annotation::set_y},
	{0, 0}
};

const luaL_reg Lua_Annotation::metatable[] = {
	{"__index", L_TableGet<Lua_Annotation>},
	{"__newindex", L_TableSet<Lua_Annotation>},
	{0, 0}
};

struct Lua_Fog {
	short index; // above or under water
	static const char *name;
	
	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];
	
	static bool valid(int index) { return index == OGL_Fog_AboveLiquid || index == OGL_Fog_BelowLiquid; }

	static int get_active(lua_State *L);
	static int get_affects_landscapes(lua_State *L);
	static int get_color(lua_State *L);
	static int get_depth(lua_State *L);

	static int set_active(lua_State *L);
	static int set_affects_landscapes(lua_State *L);
	static int set_depth(lua_State *L);
};

struct Lua_Fog_Color {
	short index;
	static const char *name;

	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];

	static bool valid(int index) { return Lua_Fog::valid(index); }

	static int get_r(lua_State *L);
	static int get_g(lua_State *L);
	static int get_b(lua_State *L);

	static int set_r(lua_State *L);
	static int set_g(lua_State *L);
	static int set_b(lua_State *L);
};

const char* Lua_Fog_Color::name = "fog_color";

int Lua_Fog_Color::get_r(lua_State *L)
{
	lua_pushnumber(L, (float) (OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.red) / 65535);
	return 1;
}

int Lua_Fog_Color::get_g(lua_State *L)
{
	lua_pushnumber(L, (float) (OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.green) / 65535);
	return 1;
}

int Lua_Fog_Color::get_b(lua_State *L)
{
	lua_pushnumber(L, (float) (OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.blue) / 65535);
	return 1;
}

int Lua_Fog_Color::set_r(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		luaL_error(L, "r: incorrect argument type");

	float color = static_cast<float>(lua_tonumber(L, 2));
	OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.red = PIN(int(65535 * color + 0.5), 0, 65535);
	return 0;
}

int Lua_Fog_Color::set_g(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		luaL_error(L, "g: incorrect argument type");

	float color = static_cast<float>(lua_tonumber(L, 2));
	OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.green = PIN(int(65535 * color + 0.5), 0, 65535);
	return 0;
}

int Lua_Fog_Color::set_b(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		luaL_error(L, "b: incorrect argument type");

	float color = static_cast<float>(lua_tonumber(L, 2));
	OGL_GetFogData(L_Index<Lua_Fog_Color>(L, 1))->Color.blue = PIN(int(65535 * color + 0.5), 0, 65535);
	return 0;
}

const luaL_reg Lua_Fog_Color::index_table[] = {
	{"r", Lua_Fog_Color::get_r},
	{"g", Lua_Fog_Color::get_g},
	{"b", Lua_Fog_Color::get_b},
	{0, 0}
};

const luaL_reg Lua_Fog_Color::newindex_table[] = {
	{"r", Lua_Fog_Color::set_r},
	{"g", Lua_Fog_Color::set_g},
	{"b", Lua_Fog_Color::set_b},
	{0, 0}
};

const luaL_reg Lua_Fog_Color::metatable[] = {
	{"__index", L_TableGet<Lua_Fog_Color>},
	{"__newindex", L_TableSet<Lua_Fog_Color>},
	{0, 0}
};

const char* Lua_Fog::name = "fog";

int Lua_Fog::get_active(lua_State *L)
{
	lua_pushboolean(L, OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->IsPresent);
	return 1;
}

int Lua_Fog::get_affects_landscapes(lua_State *L)
{
	lua_pushboolean(L, OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->AffectsLandscapes);
	return 1;
}

int Lua_Fog::get_color(lua_State *L)
{
	L_Push<Lua_Fog_Color>(L, L_Index<Lua_Fog>(L, 1));
	return 1;
}

int Lua_Fog::get_depth(lua_State *L)
{
	lua_pushnumber(L, OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->Depth);
	return 1;
}

const luaL_reg Lua_Fog::index_table[] = {
	{"active", Lua_Fog::get_active},
	{"affects_landscapes", Lua_Fog::get_affects_landscapes},
	{"color", Lua_Fog::get_color},
	{"depth", Lua_Fog::get_depth},
	{"present", Lua_Fog::get_active},
	{0, 0}
};

int Lua_Fog::set_active(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "active: incorrect argument type");
	
	OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->IsPresent = static_cast<bool>(lua_toboolean(L, 2));
	return 0;
}

int Lua_Fog::set_affects_landscapes(lua_State *L)
{
	if (!lua_isboolean(L, 2))
		return luaL_error(L, "affects_landscapes: incorrect argument type");
	
	OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->AffectsLandscapes = static_cast<bool>(lua_toboolean(L, 2));
	return 0;
}

int Lua_Fog::set_depth(lua_State *L)
{
	if (!lua_isnumber(L, 2))
		return luaL_error(L, "depth: incorrect argument type");

	OGL_GetFogData(L_Index<Lua_Fog>(L, 1))->Depth = static_cast<float>(lua_tonumber(L, 2));
	return 0;
}

const luaL_reg Lua_Fog::newindex_table[] = {
	{"active", Lua_Fog::set_active},
	{"affects_landscapes", Lua_Fog::set_affects_landscapes},
	{"depth", Lua_Fog::set_depth},
	{"present", Lua_Fog::set_active},
	{0, 0}
};

const luaL_reg Lua_Fog::metatable[] = {
	{"__index", L_TableGet<Lua_Fog>},
	{"__newindex", L_TableSet<Lua_Fog>},
	{0, 0}
};

struct Lua_Level {
	short index;
	static const char *name;

	static const luaL_reg metatable[];
	static const luaL_reg index_table[];
	static const luaL_reg newindex_table[];

	static bool valid(int) { return true; }

	static int get(lua_State *L);

	static int get_fog(lua_State *L);
	static int get_name(lua_State *L);
	static int get_underwater_fog(lua_State *L);
};

const char* Lua_Level::name = "Level";

template<int16 flag>
static int get_environment_flag(lua_State *L)
{
	lua_pushboolean(L, static_world->environment_flags & flag);
	return 1;
}

int Lua_Level::get_fog(lua_State *L)
{
	L_Push<Lua_Fog>(L, OGL_Fog_AboveLiquid);
	return 1;
}
	
int Lua_Level::get_name(lua_State *L)
{
	lua_pushstring(L, static_world->level_name);
	return 1;
}

int Lua_Level::get_underwater_fog(lua_State *L)
{
	L_Push<Lua_Fog>(L, OGL_Fog_BelowLiquid);
	return 1;
}

const luaL_reg Lua_Level::index_table[] = {
	{"fog", Lua_Level::get_fog},
	{"low_gravity", get_environment_flag<_environment_low_gravity>},
	{"magnetic", get_environment_flag<_environment_magnetic>},
	{"name", Lua_Level::get_name},
	{"rebellion", get_environment_flag<_environment_rebellion>},
	{"underwater_fog", Lua_Level::get_underwater_fog},
	{"vacuum", get_environment_flag<_environment_vacuum>},
	{0, 0}
};

const luaL_reg Lua_Level::newindex_table[] = {
	{0, 0}
};

const luaL_reg Lua_Level::metatable[] = {
	{"__index", L_TableGet<Lua_Level>},
	{"__newindex", L_TableSet<Lua_Level>},
	{0, 0}
};

static int compatibility(lua_State *L);

int Lua_Map_register(lua_State *L)
{
	L_Register<Lua_DamageType>(L);
	L_GlobalRegister<Lua_DamageTypes>(L);
	L_Register<Lua_Platform>(L);
	L_Register<Lua_Polygon_Floor>(L);
	L_Register<Lua_Polygon_Ceiling>(L);
	L_Register<Lua_Polygon>(L);
	L_Register<Lua_Light>(L);
	L_GlobalRegister<Lua_Lights>(L);
	L_Register<Lua_Tag>(L);
	L_GlobalRegister<Lua_Tags>(L);
	L_GlobalRegister<Lua_Polygons>(L);
	L_Register<Lua_Level>(L);
	L_Register<Lua_Annotation>(L);
	L_GlobalRegister<Lua_Annotations>(L);

	L_Register<Lua_Fog>(L);
	L_Register<Lua_Fog_Color>(L);

	// register one Level userdatum globally
	L_Push<Lua_Level>(L, 0);
	lua_setglobal(L, Lua_Level::name);

	compatibility(L);
}

static const char* compatibility_script = ""
	"function annotations() local i = 0 local n = # Annotations return function() if i < n then a = Annotations[i] i = i + 1 if a.polygon then p = a.polygon.index else p = -1 end return a.text, p, a.x, a.y else return nil end end end\n"
	"function get_fog_affects_landscapes() return Level.fog.affects_landscapes end\n"
	"function get_fog_color() return Level.fog.color.r, Level.fog.color.g, Level.fog.color.b end\n"
	"function get_fog_depth() return Level.fog.depth end\n"
	"function get_fog_present() return Level.fog.active end\n"
	"function get_level_name() return Level.name end\n"
	"function get_light_state(light) return Lights[light].active end\n"
	"function get_map_environment() return Level.vacuum, Level.magnetic, Level.rebellion, Level.low_gravity end\n"
	"function get_platform_ceiling_height(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.ceiling_height end end\n"
	"function get_platform_floor_height(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.floor_height end end\n"
	"function get_platform_index(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.index else return -1 end end\n"
	"function get_platform_monster_control(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.monster_controllable end end\n"
	"function get_platform_movement(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.extending end end\n"
	"function get_platform_player_control(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.player_controllable end end\n"
	"function get_platform_speed(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.speed * 1024 / 30 end end\n"
	"function get_platform_state(polygon) if Polygons[polygon].platform then return Polygons[polygon].platform.active end end\n"
	"function get_polygon_ceiling_height(polygon) return Polygons[polygon].ceiling.height end\n"
	"function get_polygon_center(polygon) return Polygons[polygon].x * 1024, Polygons[polygon].y * 1024 end\n"
	"function get_polygon_floor_height(polygon) return Polygons[polygon].floor.height end\n"
	"function get_polygon_type(polygon) return Polygons[polygon].type end\n"
	"function get_tag_state(tag) return Tags[tag].active end\n"
	"function get_underwater_fog_affects_landscapes() return Level.underwater_fog.affects_landscapes end\n"
	"function get_underwater_fog_color() return Level.underwater_fog.color.r, Level.underwater_fog.color.g, Level.underwater_fog.color.b end\n"
	"function get_underwater_fog_depth() return Level.underwater_fog.depth end\n"
	"function get_underwater_fog_present() return Level.underwater_fog.active end\n"

	"function number_of_polygons() return # Polygons end\n"
	"function set_fog_affects_landscapes(affects_landscapes) Level.fog.affects_landscapes = affects_landscapes end\n"
	"function set_fog_color(r, g, b) Level.fog.color.r = r Level.fog.color.g = g Level.fog.color.b = b end\n"
	"function set_fog_depth(depth) Level.fog.depth = depth end\n"
	"function set_fog_present(present) Level.fog.active = present end\n"
	"function set_light_state(light, state) Lights[light].active = state end\n"
	"function set_platform_ceiling_height(polygon, height) if Polygons[polygon].platform then Polygons[polygon].platform.ceiling_height = height end end\n"
	"function set_platform_floor_height(polygon, height) if Polygons[polygon].platform then Polygons[polygon].platform.floor_height = height end end\n"
	"function set_platform_monster_control(polygon, control) if Polygons[polygon].platform then Polygons[polygon].platform.monster_controllable = control end end\n"
	"function set_platform_movement(polygon, movement) if Polygons[polygon].platform then Polygons[polygon].platform.extending = movement end end\n"
	"function set_platform_player_control(polygon, control) if Polygons[polygon].platform then Polygons[polygon].platform.player_controllable = control end end\n"
	"function set_platform_speed(polygon, speed) if Polygons[polygon].platform then Polygons[polygon].platform.speed = speed * 30 / 1024 end end\n"
	"function set_platform_state(polygon, state) if Polygons[polygon].platform then Polygons[polygon].platform.active = state end end\n"
	"function set_polygon_ceiling_height(polygon, height) Polygons[polygon].ceiling.height = height end\n"
	"function set_polygon_floor_height(polygon, height) Polygons[polygon].floor.height = height end\n"
	"function set_polygon_type(polygon, type) Polygons[polygon].type = type end\n"
	"function set_tag_state(tag, state) Tags[tag].active = state end\n"
	"function set_underwater_fog_affects_landscapes(affects_landscapes) Level.underwater_fog.affects_landscapes = affects_landscapes end\n"
	"function set_underwater_fog_color(r, g, b) Level.underwater_fog.color.r = r Level.underwater_fog.color.g = g Level.underwater_fog.color.b = b end\n"
	"function set_underwater_fog_depth(depth) Level.underwater_fog.depth = depth end\n"
	"function set_underwater_fog_present(present) Level.underwater_fog.active = present end\n"

	;

static int compatibility(lua_State *L)
{
	luaL_loadbuffer(L, compatibility_script, strlen(compatibility_script), "map_compatibility");
	lua_pcall(L, 0, 0, 0);
}

#endif
