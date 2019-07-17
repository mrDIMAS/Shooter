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

weapon_t* weapon_create(level_t* level, weapon_type_t type)
{
	weapon_t* wpn = DE_NEW(weapon_t);
	wpn->type = type;
	wpn->level = level;
	wpn->ammo = 100;

	de_path_t path;
	switch (type) {
		case WEAPON_TYPE_AK47:
			de_path_from_cstr_as_view(&path, "data/models/ak47/ak47.fbx");
			break;
		case WEAPON_TYPE_M4:
			de_path_from_cstr_as_view(&path, "data/models/m4/m4.fbx");
			break;
		default:
			de_log("invalid weapon type");
			de_free(wpn);
			return NULL;
	}

	de_resource_t* model_resource = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_MODEL, &path);
	wpn->model = de_model_instantiate(de_resource_to_model(model_resource), level->scene);

	wpn->shot_light = de_node_create(level->scene, DE_NODE_TYPE_LIGHT);
	de_light_t* shot_light = de_node_to_light(wpn->shot_light);
	de_light_set_radius(shot_light, 0.0);
	de_node_attach(wpn->shot_light, wpn->model);

	/* make sure weapon will not penetrate into walls (in most cases it will be rendered above all
	 * other geometry) */
	 //de_node_set_depth_hack(wpn->model, 0.1f);
	return wpn;
}

void weapon_set_visible(weapon_t* wpn, bool state)
{
	de_node_set_local_visibility(wpn->model, state);
}

void weapon_free(weapon_t* wpn)
{
	de_node_free(wpn->model);
	de_free(wpn);
}

void weapon_update(weapon_t* wpn)
{
	wpn->offset.x += (wpn->dest_offset.x - wpn->offset.x) * 0.1f;
	wpn->offset.y += (wpn->dest_offset.y - wpn->offset.y) * 0.1f;
	wpn->offset.z += (wpn->dest_offset.z - wpn->offset.z) * 0.1f;

	de_node_set_local_position(wpn->model, &wpn->offset);

	de_light_t* shot_light = de_node_to_light(wpn->shot_light);
	de_light_set_radius(shot_light, wpn->shot_light_radius);
	de_light_set_color(shot_light, &(de_color_t){.r = 255, .g = 207, .b = 168});

	wpn->shot_light_radius -= 0.45f;
	if (wpn->shot_light_radius < 0.0f) {
		wpn->shot_light_radius = 0.0f;
	}
}

bool weapon_visit(de_object_visitor_t* visitor, weapon_t* wpn)
{
	bool result = true;
	result &= de_object_visitor_visit_int32(visitor, "Type", (int32_t*)&wpn->type);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Level", &wpn->level, level_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Model", &wpn->model, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "ShotLight", &wpn->shot_light, de_node_visit);
	return result;
}

void weapon_shoot(weapon_t* wpn)
{
	const game_t* game = wpn->level->game;
	if (game->time.seconds - wpn->last_shot_time >= 0.1 && wpn->ammo > 0) {
		wpn->offset = (de_vec3_t) { 0.0f, 0.0f, -0.1f };

		de_ray_t ray;
		de_node_get_global_position(wpn->model, &ray.origin);
		de_node_get_look_vector(wpn->model, &ray.dir);
		de_vec3_scale(&ray.dir, &ray.dir, 30.0f);

		de_path_t path;
		de_path_from_cstr_as_view(&path, "data/sounds/m4_shot.wav");
		de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path);
		if (res) {
			de_sound_context_t* context = de_core_get_sound_context(game->core);
			de_sound_source_t* shot_sound = de_sound_source_create(context, DE_SOUND_SOURCE_TYPE_3D);
			de_sound_source_set_buffer(shot_sound, de_resource_to_sound_buffer(res));
			de_sound_source_set_play_once(shot_sound, true);
			de_sound_source_play(shot_sound);
			de_sound_source_set_position(shot_sound, &ray.origin);
		}

		de_ray_cast_result_t result;
		if (de_ray_cast_closest(wpn->level->scene, &ray, DE_RAY_CAST_FLAGS_IGNORE_BODY_IN_RAY, &result)) {

		}

		wpn->shot_light_radius = 4.0f;

		wpn->last_shot_time = game->time.seconds;

		--wpn->ammo;
	}
}