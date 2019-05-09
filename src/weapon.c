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

	de_resource_t* model_resource = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_MODEL, &path, 0);
	wpn->model = de_model_instantiate(de_resource_to_model(model_resource), level->scene);
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
	DE_UNUSED(wpn);
}

bool weapon_visit(de_object_visitor_t* visitor, weapon_t* wpn)
{
	bool result = true;
	result &= de_object_visitor_visit_int32(visitor, "Type", (int32_t*)&wpn->type);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Model", &wpn->model, de_node_visit);
	return result;
}

void weapon_shoot(weapon_t* wpn)
{
	DE_UNUSED(wpn);
}