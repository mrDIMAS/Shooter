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

typedef enum actor_type_t {
	ACTOR_TYPE_PLAYER,
	ACTOR_TYPE_BOT,
	ACTOR_TYPE_FORCE_SIZE = INT32_MAX,
} actor_type_t;

struct actor_t {
	actor_type_t type;
	actor_dispatch_table_t* dispatch_table;
	level_t* parent_level;
	de_body_t* body;
	de_node_t* pivot;
	float move_speed;
	float health;
	union {
		player_t player;
		bot_t bot;
	}s;
	DE_LINKED_LIST_ITEM(actor_t);
};

actor_t* actor_create(level_t* level, actor_type_t type);

void actor_free(actor_t* actor);

void actor_update(actor_t* actor);

bool actor_visit(de_object_visitor_t* visitor, actor_t* actor);

bool actor_process_event(actor_t* actor, const de_event_t* evt);

void actor_set_position(actor_t* actor, const de_vec3_t* pos);

player_t* actor_to_player(actor_t* actor);

bool actor_has_ground_contact(actor_t* actor);

actor_t* actor_from_player(player_t* player);

bot_t* actor_to_bot(actor_t* actor);

actor_t* actor_from_bot(bot_t* bot);