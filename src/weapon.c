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

weapon_t* weapon_create(level_t* level, weapon_type_t type) {
	weapon_t* wpn = DE_NEW(weapon_t);

	wpn->type = type;

	if (type == WEAPON_TYPE_SHOTGUN) {
		wpn->model = de_fbx_load_to_scene(level->scene, "data/models/shotgun.fbx");
	} else {
		de_log("invalid weapon type");
	}

	return wpn;
}

void weapon_free(weapon_t* wpn) {
	de_node_free(wpn->model);
	de_free(wpn);
}

void weapon_update(weapon_t* wpn) {
	DE_UNUSED(wpn);
}