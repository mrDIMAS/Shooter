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

typedef enum sound_type_t {
	SOUND_TYPE_UNKNOWN,
	SOUND_TYPE_STONE,
	SOUND_TYPE_METAL,
	SOUND_TYPE_WOOD,
	SOUND_TYPE_GRASS
} sound_type_t;

typedef struct footstep_sound_type_map_entry_t {
	uint32_t texture_name_hash;
	sound_type_t sound_type;
} footstep_sound_type_map_entry_t;

typedef struct footstep_sound_group_t {
	sound_type_t type;
	DE_ARRAY_DECLARE(de_resource_t*, buffers);
} footstep_sound_group_t;

typedef struct footstep_sound_map_t {
	DE_ARRAY_DECLARE(footstep_sound_type_map_entry_t, entries);
	DE_ARRAY_DECLARE(footstep_sound_group_t, groups);
} footstep_sound_map_t;

void footstep_sound_map_read(de_core_t* core, footstep_sound_map_t* map);

void footstep_sound_map_free(footstep_sound_map_t* map);

de_resource_t* footstep_sound_map_probe(footstep_sound_map_t* map, uint32_t texture_path_hash);