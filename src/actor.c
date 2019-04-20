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

static actor_dispatch_table_t* actor_get_dispatch_table_by_type(actor_type_t type)
{
	static actor_dispatch_table_t stub;
	switch (type) {
		case ACTOR_TYPE_BOT:
			return &stub;
		case ACTOR_TYPE_PLAYER:
			return player_get_dispatch_table();
		default:
			break;
	}
	return NULL;
}

actor_t* actor_create(level_t* level, actor_type_t type)
{
	actor_t* actor = DE_NEW(actor_t);
	actor->type = type;
	actor->parent_level = level;
	actor->dispatch_table = actor_get_dispatch_table_by_type(type);

	actor->body = de_body_create(level->scene);
	de_body_set_gravity(actor->body, &(de_vec3_t) {.y = -20 });

	actor->pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, actor->pivot);
	de_node_set_body(actor->pivot, actor->body);

	if (actor->dispatch_table->init) {
		actor->dispatch_table->init(actor);
	}
	return actor;
}

void actor_free(actor_t* actor)
{
	if (actor->dispatch_table->deinit) {
		actor->dispatch_table->deinit(actor);
	}
	de_node_free(actor->pivot);
	de_free(actor);
}

void actor_update(actor_t* actor)
{
	if (actor->dispatch_table->update) {
		actor->dispatch_table->update(actor);
	}
}

bool actor_process_event(actor_t* actor, const de_event_t* evt)
{
	if (actor->dispatch_table->process_event) {
		return actor->dispatch_table->process_event(actor, evt);
	}
	return false;
}

bool actor_visit(de_object_visitor_t* visitor, actor_t* actor)
{
	bool result = true;
	result &= de_object_visitor_visit_int32(visitor, "Type", (int32_t*)&actor->type);
	if (visitor->is_reading) {
		actor->dispatch_table = actor_get_dispatch_table_by_type(actor->type);
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Body", &actor->body, de_body_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Level", &actor->parent_level, level_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Pivot", &actor->pivot, de_node_visit);
	if (actor->dispatch_table->visit) {
		result &= actor->dispatch_table->visit(visitor, actor);
	}
	return result;
}

void actor_set_position(actor_t* actor, const de_vec3_t* pos)
{
	de_node_set_local_position(actor->pivot, pos);
}

player_t* actor_to_player(actor_t* actor)
{
	return &actor->s.player;
}

actor_t* actor_from_player(player_t* player)
{
	return (actor_t*)((char*)player - offsetof(actor_t, s.player));
}

bot_t* actor_to_bot(actor_t* actor)
{
	return &actor->s.bot;
}

actor_t* actor_from_bot(bot_t* bot)
{
	return (actor_t*)((char*)bot - offsetof(actor_t, s.bot));
}