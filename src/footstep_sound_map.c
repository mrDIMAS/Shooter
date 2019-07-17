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

static sound_type_t string_to_sound_type(const char* str)
{
	if (strncmp(str, "Stone", 5) == 0) {
		return SOUND_TYPE_STONE;
	} else if (strncmp(str, "Metal", 5) == 0) {
		return SOUND_TYPE_METAL;
	} else if (strncmp(str, "Wood", 4) == 0) {
		return SOUND_TYPE_WOOD;
	} else if (strncmp(str, "Grass", 5) == 0) {
		return SOUND_TYPE_GRASS;
	}
	de_log("game: unknown surface type %s!", str);
	return SOUND_TYPE_UNKNOWN;
}

footstep_sound_group_t* footstep_sound_map_find_or_create_sound_group(footstep_sound_map_t* map, sound_type_t type)
{
	for (size_t i = 0; i < map->groups.size; ++i) {
		footstep_sound_group_t* group = map->groups.data + i;
		if (group->type == type) {
			return group;
		}
	}

	/* create new one */
	footstep_sound_group_t group;
	DE_ARRAY_INIT(group.buffers);
	group.type = type;
	DE_ARRAY_APPEND(map->groups, group);
	return &DE_ARRAY_LAST(map->groups);
}

void footstep_sound_map_read(de_core_t* core, footstep_sound_map_t* map)
{
	memset(map, 0, sizeof(*map));
	char buffer[1024];

	/* read texture to sounds group map */
	FILE* file = fopen("data/sounds/texture_to_sound_type_map.txt", "r");
	if (!file) {
		de_log("game: unable to read texture_to_sound_type_map.txt map! footstep sounds won't play!");
		return;
	}
	while (fgets(buffer, sizeof(buffer), file)) {
		char* str_texture_name = strtok(buffer, " ");
		char* str_sound_type = strtok(NULL, " ");
		if (str_texture_name && str_sound_type) {
			sound_type_t sound_type = string_to_sound_type(str_sound_type);
			if (sound_type != SOUND_TYPE_UNKNOWN) {
				const footstep_sound_type_map_entry_t entry = {
					.texture_name_hash = de_hash_murmur3((uint8_t*)str_texture_name, strlen(str_texture_name), 0),
					.sound_type = sound_type
				};
				DE_ARRAY_APPEND(map->entries, entry);
			}
		}
	}

	/* read sound type to sound buffer list */
	file = fopen("data/sounds/sound_to_sound_type_map.txt", "r");
	if (!file) {
		de_log("game: unable to read sound_to_sound_type_map.txt sound map! footstep sounds won't play!");
		return;
	}

	while (fgets(buffer, sizeof(buffer), file)) {
		char* str_sound_filename = strtok(buffer, " ");
		char* str_sound_type = strtok(NULL, " ");
		if (str_sound_filename && str_sound_type) {
			sound_type_t type = string_to_sound_type(str_sound_type);
			if (type != SOUND_TYPE_UNKNOWN) {
				footstep_sound_group_t* group = footstep_sound_map_find_or_create_sound_group(map, type);
				if (group) {
					de_path_t path;
					de_path_from_cstr_as_view(&path, str_sound_filename);
					de_resource_t* res = de_core_request_resource(core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path);
					if (res) {
						de_resource_add_ref(res);
						/* make sure that sound resources will be alive all the time */
						de_resource_set_flags(res, DE_RESOURCE_FLAG_PERSISTENT);
						DE_ARRAY_APPEND(group->buffers, res);
					}
				}
			}
		}
	}
}

void footstep_sound_map_free(footstep_sound_map_t* map)
{
	/* release resources */
	for (size_t i = 0; i < map->groups.size; ++i) {
		footstep_sound_group_t* group = map->groups.data + i;
		for (size_t k = 0; k < group->buffers.size; ++k) {
			de_resource_release(group->buffers.data[k]);
		}
		DE_ARRAY_FREE(group->buffers);
	}
	DE_ARRAY_FREE(map->groups);

	/* clean up */
	DE_ARRAY_FREE(map->entries);
}

static sound_type_t footstep_sound_map_texture_name_hash_to_sound_type(footstep_sound_map_t* map, uint32_t texture_path_hash)
{
	for (size_t i = 0; i < map->entries.size; ++i) {
		const footstep_sound_type_map_entry_t* entry = map->entries.data + i;
		if (entry->texture_name_hash == texture_path_hash) {
			return entry->sound_type;
		}
	}
	return SOUND_TYPE_UNKNOWN;
}

de_resource_t* footstep_sound_map_probe(footstep_sound_map_t* map, uint32_t texture_path_hash)
{
	const sound_type_t sound_type = footstep_sound_map_texture_name_hash_to_sound_type(map, texture_path_hash);
	if (sound_type != SOUND_TYPE_UNKNOWN) {
		for (size_t i = 0; i < map->groups.size; ++i) {
			footstep_sound_group_t* group = map->groups.data + i;
			if (group->type == sound_type && group->buffers.size != 0) {
				size_t index = rand() % group->buffers.size;
				DE_ASSERT(index < group->buffers.size);
				return group->buffers.data[index];
			}
		}
	}

	return NULL;
}