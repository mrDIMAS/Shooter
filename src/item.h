/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

typedef enum item_type_t {
	ITEM_TYPE_AK47,
	ITEM_TYPE_M4,
	ITEM_TYPE_MEDKIT,
} item_type_t;

typedef struct item_definition_t {
	size_t max_ammo;
	size_t pick_up_ammo;
	float damage;
	const char* model_path;
	int health_restore;	
	float reactivation_time; /**< Amount of time left for item to become activated again. */
} item_definition_t;

struct item_t {	
	item_type_t type;
	level_t* level;
	de_node_t* model;
	float time_until_reactivation;
};

const item_definition_t* item_definition_from_item_type(item_type_t type);

item_t* item_create(level_t* level, item_type_t type);

void item_set_position(item_t* item, const de_vec3_t* pos);

bool item_is_active(item_t* item);

void item_deactivate(item_t* item);

void item_free(item_t* item);

void item_update(item_t* item, float dt);