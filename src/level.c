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

void level_create_collider(level_t* level)
{
	de_node_t* polygon = de_scene_find_node(level->scene, "Polygon");
	if (polygon) {
		de_static_geometry_t* map_collider;
		assert(polygon->type == DE_NODE_TYPE_MESH);
		map_collider = de_scene_create_static_geometry(level->scene);
		de_node_calculate_transforms_ascending(polygon);
		de_static_geometry_fill(map_collider, de_node_to_mesh(polygon), polygon->global_matrix);
	}
}

bool level_visit(de_object_visitor_t* visitor, level_t* level)
{
	bool result = true;
	if (visitor->is_reading) {
		level->game = de_core_get_user_pointer(visitor->core);
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Scene", &level->scene, de_scene_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Player", &level->player, actor_visit);
	if (visitor->is_reading) {
		level_create_collider(level);
	}
	return result;
}

level_t* level_create_test(game_t* game)
{
	level_t* level = DE_NEW(level_t);
	level->game = game;
	level->scene = de_scene_create(game->core);

	de_path_t res_path;
	de_path_init(&res_path);

	/* Ripper */
	de_path_append_cstr(&res_path, "data/models/ripper.fbx");
	de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_MODEL, &res_path, 0);
	de_model_t* mdl = de_resource_to_model(res);
	de_node_t* ripper1 = de_model_instantiate(mdl, level->scene);
	de_node_set_local_position(ripper1, &(de_vec3_t){ -1, 0, -1 });
	de_node_t* ripper2 = de_model_instantiate(mdl, level->scene);
	de_node_set_local_position(ripper2, &(de_vec3_t){ 1, 0, -1 });
	de_node_t* ripper3 = de_model_instantiate(mdl, level->scene);
	de_node_set_local_position(ripper3, &(de_vec3_t){ 1, 0, 1 });
	de_node_t* ripper4 = de_model_instantiate(mdl, level->scene);
	de_node_set_local_position(ripper4, &(de_vec3_t){ -1, 0, 1 });

	/* Level */
	de_path_clear(&res_path);
	de_path_append_cstr(&res_path, "data/models/map2_bin.fbx");
	res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_MODEL, &res_path, 0);
	de_model_instantiate(de_resource_to_model(res), level->scene);

	level_create_collider(level);

	de_node_t* particle_system_node = de_node_create(level->scene, DE_NODE_TYPE_PARTICLE_SYSTEM);
	de_particle_system_t* particle_system = de_node_to_particle_system(particle_system_node);
	de_particle_system_emitter_t* emitter = de_particle_system_emitter_create(particle_system, DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE);
	emitter->max_particles = 1000;
	emitter->particle_spawn_rate = 50;	
	de_color_gradient_t* gradient = de_particle_system_get_color_gradient_over_lifetime(particle_system);
	de_color_gradient_add_point(gradient, 0.00f, &(de_color_t) { 150, 150, 150, 0 });
	de_color_gradient_add_point(gradient, 0.05f, &(de_color_t) { 150, 150, 150, 220 });
	de_color_gradient_add_point(gradient, 0.85f, &(de_color_t) { 255, 255, 255, 180 });
	de_color_gradient_add_point(gradient, 1.00f, &(de_color_t) { 255, 255, 255, 0 });
	de_path_clear(&res_path);
	de_path_append_cstr(&res_path, "data/particles/smoke_04.tga");
	res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_TEXTURE, &res_path, 0);
	de_particle_system_set_texture(particle_system, de_resource_to_texture(res));
	de_scene_add_node(level->scene, particle_system_node);

	de_path_free(&res_path);

	level->player = actor_create(level, ACTOR_TYPE_PLAYER);
	de_node_t* pp = de_scene_find_node(level->scene, "PlayerPosition");
	if (pp) {
		de_vec3_t pos;
		de_node_get_global_position(pp, &pos);
		actor_set_position(level->player, &pos);
	}

	return level;
}

void level_update(level_t* level)
{
	actor_update(level->player);
	DE_LINKED_LIST_FOR_EACH_T(projectile_t*, projectile, level->projectiles)
	{
		projectile_update(projectile);
	}
}

void level_free(level_t* level)
{
	actor_free(level->player);
	de_scene_free(level->scene);
	de_free(level);
}