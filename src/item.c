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

const item_definition_t* item_definition_from_item_type(item_type_t type)
{
	switch (type) {
		case ITEM_TYPE_MEDKIT: {
			static item_definition_t definition = {
				.health_restore = 25,
				.model_path = "data/models/medkit.fbx",
				.reactivation_time = 30.0f
			};
			return &definition;
		}
		case ITEM_TYPE_M4: {
			static item_definition_t definition = {
				.max_ammo = 200,
				.pick_up_ammo = 40,
				.damage = 20,
				.model_path = "data/models/m4/m4.fbx",
				.reactivation_time = 45.0f
			};
			return &definition;
		}
		case ITEM_TYPE_AK47: {
			static item_definition_t definition = {
				.max_ammo = 220,
				.pick_up_ammo = 35,
				.damage = 17.5,
				.model_path = "data/models/ak47/ak47.fbx",
				.reactivation_time = 45.0f
			};
			return &definition;
		}
	}

	return NULL;
}

item_t* item_create(level_t* level, item_type_t type)
{
	item_t* item = DE_NEW(item_t);
	item->level = level;
	item->type = type;
	item->time_until_reactivation = 0.0f;

	de_path_t model_path;
	const item_definition_t* definition = item_definition_from_item_type(type);
	de_path_from_cstr_as_view(&model_path, definition->model_path);

	de_resource_t* model_resource = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_MODEL, &model_path);
	if (model_resource) {
		item->model = de_model_instantiate(de_resource_to_model(model_resource), level->scene);
	}

	DE_ARRAY_APPEND(level->items, item);

	return item;
}

bool item_is_active(item_t* item)
{
	DE_ASSERT(item);
	return item->time_until_reactivation == 0.0f;
}

void item_set_position(item_t* item, const de_vec3_t* pos)
{
	DE_ASSERT(item);
	de_node_set_local_position(item->model, pos);
}

void item_free(item_t* item)
{
	DE_ASSERT(item);
	DE_ARRAY_REMOVE(item->level->items, item);
	de_node_free(item->model);
	de_free(item);
}

void item_deactivate(item_t* item)
{
	DE_ASSERT(item);
	DE_ASSERT(!item_is_active(item));
	const item_definition_t* definition = item_definition_from_item_type(item->type);
	item->time_until_reactivation = definition->reactivation_time;
}

void item_update(item_t* item, float dt)
{
	DE_ASSERT(item);
	if (item->time_until_reactivation > 0) {
		item->time_until_reactivation -= dt;
		if (item->time_until_reactivation < 0) {
			item->time_until_reactivation = 0.0f;
		}
	}
}