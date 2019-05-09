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

typedef enum projectile_type_t {
	PROJECTILE_TYPE_ROCKET,
	PROJECTILE_TYPE_GRENADE,
} projectile_type_t;

typedef struct projectile_definition_t {	
	float speed;
	bool need_body;
	bool flying;
	int lifetime;
	float body_radius;
	const char* model;
} projectile_definition_t;

typedef struct projectile_t {
	level_t* level;
	projectile_type_t type;
	projectile_definition_t* definition;
	de_node_t* model;
	de_vec3_t direction;
	int lifetime;
	DE_LINKED_LIST_ITEM(struct projectile_t);
} projectile_t;

projectile_t* projectile_create(level_t* level, projectile_type_t type, const de_vec3_t* pos, const de_vec3_t* dir);

void projectile_free(projectile_t* p);

void projectile_update(projectile_t* p);