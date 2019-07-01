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

static void bot_init(actor_t* actor)
{
	actor->move_speed = 0.019f;
	bot_t* bot = actor_to_bot(actor);
	de_path_t path;
	de_path_from_cstr_as_view(&path, "data/models/soldier/soldier.fbx");
	de_resource_t* soldier_model = de_core_request_resource(actor->parent_level->game->core, DE_RESOURCE_TYPE_MODEL, &path);
	if (soldier_model) {
		bot->model = de_model_instantiate(de_resource_to_model(soldier_model), actor->parent_level->scene);
		de_node_attach(bot->model, actor->pivot);
		de_body_set_radius(actor->body, 0.3f);
	}
}

static void bot_deinit(actor_t* actor)
{
	DE_UNUSED(actor);
}

static bool bot_visit(de_object_visitor_t* visitor, actor_t* actor)
{
	bool result = true;
	bot_t* bot = actor_to_bot(actor);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Model", &bot->model, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_ENUM(visitor, "Type", &bot->type);
	return result;
}

static void bot_update(actor_t* actor)
{
//	bot_t* bot = actor_to_bot(actor);
	const actor_t* player = actor->parent_level->player;

	de_vec3_t self_pos;
	de_node_get_global_position(actor->pivot, &self_pos);

	de_vec3_t player_pos;
	de_node_get_global_position(player->pivot, &player_pos);

	de_vec3_t dir;
	de_vec3_sub(&dir, &player_pos, &self_pos);
	
	float distance = 0.0;
	de_vec3_normalize_ex(&dir, &dir, &distance);

	if (distance > 1.0f) {
		de_vec3_t velocity;
		de_vec3_scale(&velocity, &dir, actor->move_speed);

		de_body_set_x_velocity(actor->body, 0.0f);
		de_body_set_z_velocity(actor->body, 0.0f);

		de_body_move(actor->body, &velocity);
	}

	de_vec3_t look;
	de_node_get_look_vector(actor->pivot, &look);

	const float angle = -(float)(atan2(dir.z, dir.x) );
	de_quat_t rotation;
	de_quat_from_axis_angle(&rotation, &(de_vec3_t){0, 1, 0}, angle);

	de_node_set_local_rotation(actor->pivot, &rotation);
}

actor_dispatch_table_t* bot_get_dispatch_table()
{
	static actor_dispatch_table_t table = {
		.init = bot_init,
		.deinit = bot_deinit,
		.visit = bot_visit,
		.update = bot_update
	};
	return &table;
}