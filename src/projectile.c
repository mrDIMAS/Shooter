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

static projectile_definition_t* projectile_get_definition(projectile_type_t type)
{
	static projectile_definition_t rocket = {
		.speed = 1.0f,
		.model = "data/models/projectile/rocket.fbx",
		.body_radius = 0.15f,
		.need_body = true,
		.flying = true,
		.lifetime = 180,
	};
	static projectile_definition_t grenade = {
		.speed = 0.01f,
		.model = "data/models/projectile/grenade.fbx",
		.body_radius = 0.1f,
		.need_body = true,
		.lifetime = 90
	};
	switch (type) {
		case PROJECTILE_TYPE_ROCKET:
			return &rocket;
		case PROJECTILE_TYPE_GRENADE:
			return &grenade;
	}
	return NULL;
}

projectile_t* projectile_create(level_t* level, projectile_type_t type, const de_vec3_t* pos, const de_vec3_t* dir)
{
	projectile_t* p = DE_NEW(projectile_t);
	p->level = level;
	p->type = type;
	de_vec3_normalize(&p->direction, dir);	
	p->definition = projectile_get_definition(type);
	de_path_t model_path;
	de_path_from_cstr_as_view(&model_path, p->definition->model);
	de_resource_t* res = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_MODEL, &model_path);
	if (res) {
		de_model_t* model = de_resource_to_model(res);
		p->model = de_model_instantiate(model, level->scene);				
	}
	if (p->definition->need_body) {
		de_body_t* body = de_body_create(level->scene, de_convex_shape_create_sphere(p->definition->body_radius));		
		de_node_set_body(p->model, body);		
	}
	p->lifetime = p->definition->lifetime;
	de_node_set_local_position(p->model, pos);
	DE_LINKED_LIST_APPEND(level->projectiles, p);
	return p;
}

void projectile_free(projectile_t* p)
{
	DE_ASSERT(p);
	DE_LINKED_LIST_REMOVE(p->level->projectiles, p);
	de_free(p);
}

void projectile_update(projectile_t* p)
{
	DE_ASSERT(p);	
	if (p->definition->flying) {
		de_vec3_t velocity;
		de_vec3_scale(&velocity, &p->direction, p->definition->speed);
		de_node_move(p->model, &velocity);
	}
	--p->lifetime;	
}