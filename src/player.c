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


bool player_process_event(player_t* p, const de_event_t* evt) {
	bool processed = false;
	switch (evt->type) {
		case DE_EVENT_TYPE_MOUSE_MOVE:
			p->desired_yaw -= evt->s.mouse_move.vx;
			p->desired_pitch -= evt->s.mouse_move.vy;
			if (p->desired_pitch > 90.0) {
				p->desired_pitch = 90.0;
			}
			if (p->desired_pitch < -90.0) {
				p->desired_pitch = -90.0;
			}
			break;
		case DE_EVENT_TYPE_KEY_DOWN:
			switch (evt->s.key.key) {
				case DE_KEY_W:
					p->controller.move_forward = true;
					break;
				case DE_KEY_S:
					p->controller.move_backward = true;
					break;
				case DE_KEY_A:
					p->controller.strafe_left = true;
					break;
				case DE_KEY_D:
					p->controller.strafe_right = true;
					break;
				case DE_KEY_C:
					p->controller.crouch = true;
					break;
				case DE_KEY_Space:
					if (!p->controller.jumped) {
						size_t k, contact_count;
						contact_count = de_body_get_contact_count(p->body);
						for (k = 0; k < contact_count; ++k) {
							de_contact_t* contact = de_body_get_contact(p->body, k);
							if (contact->normal.y > 0.7f) {
								de_body_set_y_velocity(p->body, 0.075f);
								break;
							}
						}
						p->controller.jumped = true;
					}
					break;
				case DE_KEY_LSHIFT:
					p->controller.run = true;
					break;
				default:
					break;
			}
			break;
		case DE_EVENT_TYPE_KEY_UP:
			switch (evt->s.key.key) {
				case DE_KEY_W:
					p->controller.move_forward = false;
					break;
				case DE_KEY_S:
					p->controller.move_backward = false;
					break;
				case DE_KEY_A:
					p->controller.strafe_left = false;
					break;
				case DE_KEY_D:
					p->controller.strafe_right = false;
					break;
				case DE_KEY_C:
					p->controller.crouch = false;
					break;
				case DE_KEY_Space:
					p->controller.jumped = false;
					break;
				case DE_KEY_LSHIFT:
					p->controller.run = false;
					break;
				default:
					break;
			}
			break;
		case DE_EVENT_TYPE_MOUSE_DOWN:
			break;
		default:
			break;
	}
	return processed;
}

player_t* player_create(level_t* level) {
	int i;
	de_sound_context_t* ctx;
	player_t* p;

	p = DE_NEW(player_t);

	p->parent_level = level;
	p->move_speed = 0.028f;
	p->stand_body_radius = 0.5f;
	p->crouch_body_radius = 0.35f;
	p->sit_down_speed = 0.045f;
	p->stand_up_speed = 0.06f;
	p->run_speed_multiplier = 1.75f;
	p->camera_position = (de_vec3_t) { 0, p->stand_body_radius, 0 };

	p->body = de_body_create(level->scene);
	de_body_set_gravity(p->body, &(de_vec3_t) {.y = -20 });
	de_body_set_radius(p->body, p->stand_body_radius);

	p->pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->pivot);
	de_node_set_body(p->pivot, p->body);

	p->camera = de_node_create(level->scene, DE_NODE_TYPE_CAMERA);
	de_scene_add_node(level->scene, p->camera);
	de_node_set_local_position(p->camera, &p->camera_position);
	de_node_attach(p->camera, p->pivot);

	p->flash_light = de_node_create(level->scene, DE_NODE_TYPE_LIGHT);
	de_light_set_radius(de_node_to_light(p->flash_light), 4);
	de_scene_add_node(level->scene, p->flash_light);
	de_node_attach(p->flash_light, p->camera);

	p->weapon_pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->weapon_pivot);
	de_node_attach(p->weapon_pivot, p->camera);
	de_node_set_local_position(p->weapon_pivot, &(de_vec3_t) { 0.03f, -0.052f, -0.02f });

	ctx = de_core_get_sound_context(level->game->core);

	for (i = 0; i < 4; ++i) {
		char buf[PATH_MAX];
		snprintf(buf, sizeof(buf), "data/sounds/footsteps/FootStep_shoe_metal_step%d.wav", i + 1);
		de_path_t path;
		de_path_from_cstr_as_view(&path, buf);
		de_resource_t* res = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path, 0);
		de_sound_source_t* src = de_sound_source_create(ctx, DE_SOUND_SOURCE_TYPE_3D);
		de_sound_source_set_buffer(src, de_resource_to_sound_buffer(res));
		DE_ARRAY_APPEND(p->footsteps, src);
	}


	weapon_t* shotgun = weapon_create(level, WEAPON_TYPE_SHOTGUN);
	de_node_attach(shotgun->model, p->weapon_pivot);
	DE_ARRAY_APPEND(p->weapons, shotgun);


	return p;
}

bool player_visit(de_object_visitor_t* visitor, player_t* player) {
	bool result = true;
	if (visitor->is_reading) {
		game_t* game = de_core_get_user_pointer(visitor->core);
		player->parent_level = game->level;
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Level", &player->parent_level, level_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Pivot", &player->pivot, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Camera", &player->camera, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "FlashLight", &player->flash_light, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Body", &player->body, de_body_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "WeaponPivot", &player->weapon_pivot, de_node_visit);
	result &= de_object_visitor_visit_float(visitor, "Pitch", &player->pitch);
	result &= de_object_visitor_visit_float(visitor, "DesiredPitch", &player->desired_pitch);
	result &= de_object_visitor_visit_float(visitor, "Yaw", &player->yaw);
	result &= de_object_visitor_visit_float(visitor, "DesiredYaw", &player->desired_yaw);
	result &= de_object_visitor_visit_float(visitor, "MoveSpeed", &player->move_speed);
	result &= de_object_visitor_visit_float(visitor, "StandBodyRadius", &player->stand_body_radius);
	result &= de_object_visitor_visit_float(visitor, "CrouchBodyRadius", &player->crouch_body_radius);
	result &= de_object_visitor_visit_float(visitor, "StandUpSpeed", &player->stand_up_speed);
	result &= de_object_visitor_visit_float(visitor, "SitDownSpeed", &player->sit_down_speed);
	result &= de_object_visitor_visit_float(visitor, "RunSpeedMultiplier", &player->run_speed_multiplier);
	result &= de_object_visitor_visit_float(visitor, "CameraWobble", &player->camera_wobble);
	result &= de_object_visitor_visit_float(visitor, "PathLength", &player->path_len);
	result &= de_object_visitor_visit_vec3(visitor, "CameraOffset", &player->camera_offset);
	result &= de_object_visitor_visit_vec3(visitor, "CameraDestOffset", &player->camera_dest_offset);
	result &= de_object_visitor_visit_vec3(visitor, "CameraPosition", &player->camera_position);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Footsteps", player->footsteps, de_sound_source_visit);
	return result;
}



void player_free(player_t* p) {
	for (size_t i = 0; i < p->weapons.size; ++i) {
		weapon_free(p->weapons.data[i]);
	}
	DE_ARRAY_FREE(p->weapons);
	for (size_t i = 0; i < p->footsteps.size; ++i) {
		de_sound_source_free(p->footsteps.data[i]);
	}
	DE_ARRAY_FREE(p->footsteps);
	de_node_free(p->pivot);
	de_free(p);
}

void player_update(player_t* p) {
	de_node_t *pivot, *camera;
	de_vec3_t right_axis = { 1, 0, 0 };
	de_vec3_t up_axis = { 0, 1, 0 };
	de_vec3_t look, side, offset;
	de_quat_t pitch_rot, yaw_rot;
	float speed_multiplier = 1.0f;
	bool is_moving = false;

	pivot = p->pivot;
	camera = p->camera;

	offset = (de_vec3_t) { 0 };
	look = (de_vec3_t) { 0 };
	side = (de_vec3_t) { 0 };
	de_node_get_look_vector(pivot, &look);
	de_node_get_side_vector(pivot, &side);

	/* movement */
	if (p->controller.move_forward) {
		de_vec3_sub(&offset, &offset, &look);
		is_moving = true;
	}
	if (p->controller.move_backward) {
		de_vec3_add(&offset, &offset, &look);
		is_moving = true;
	}
	if (p->controller.strafe_left) {
		de_vec3_sub(&offset, &offset, &side);
		is_moving = true;
	}
	if (p->controller.strafe_right) {
		de_vec3_add(&offset, &offset, &side);
		is_moving = true;
	}

	/* crouch */
	if (p->controller.crouch) {
		float radius = de_body_get_radius(p->body);
		radius -= p->sit_down_speed;
		if (radius < p->crouch_body_radius) {
			radius = p->crouch_body_radius;
		}
		de_body_set_radius(p->body, radius);
	} else {
		float radius = de_body_get_radius(p->body);
		radius += p->stand_up_speed;
		if (radius > p->stand_body_radius) {
			radius = p->stand_body_radius;
		}
		de_body_set_radius(p->body, radius);
	}

	/* make sure that camera will be always at the top of the body */
	p->camera_position.y = de_body_get_radius(p->body);

	/* apply camera wobbling */
	if (is_moving && de_body_get_contact_count(p->body) > 0) {
		p->camera_dest_offset.x = 0.05f * (float)cos(p->camera_wobble * 0.5f);
		p->camera_dest_offset.y = 0.1f * (float)sin(p->camera_wobble);

		p->camera_wobble += 0.25f;
	} else {
		p->camera_dest_offset.x = 0;
		p->camera_dest_offset.y = 0;
	}

	/* camera offset will follow destination offset -> smooth movements */
	p->camera_offset.x += (p->camera_dest_offset.x - p->camera_offset.x) * 0.1f;
	p->camera_offset.y += (p->camera_dest_offset.y - p->camera_offset.y) * 0.1f;
	p->camera_offset.z += (p->camera_dest_offset.z - p->camera_offset.z) * 0.1f;

	/* set actual camera position */
	{
		de_vec3_t combined_position;
		de_vec3_add(&combined_position, &p->camera_position, &p->camera_offset);
		de_node_set_local_position(p->camera, &combined_position);
	}

	/* run */
	if (p->controller.run) {
		speed_multiplier = p->run_speed_multiplier;
	}

	if (de_vec3_sqr_len(&offset) > 0) {
		de_vec3_normalize(&offset, &offset);
		de_vec3_scale(&offset, &offset, speed_multiplier * p->move_speed);
		p->path_len += 0.05f;
		if (p->path_len >= 1) {
			de_sound_source_t* src = p->footsteps.data[rand() % p->footsteps.size];
			if (src) {
				de_sound_source_play(src);
				de_vec3_t pos;
				de_node_get_global_position(p->pivot, &pos);
				de_sound_source_set_position(src, &pos);
			}
			p->path_len = 0;
		}
	}

	p->yaw += (p->desired_yaw - p->yaw) * 0.22f;
	p->pitch += (p->desired_pitch - p->pitch) * 0.22f;

	de_body_set_x_velocity(p->body, 0);
	de_body_set_z_velocity(p->body, 0);

	de_body_move(p->body, &offset);

	de_node_set_local_rotation(pivot, de_quat_from_axis_angle(&yaw_rot, &up_axis, de_deg_to_rad(p->yaw)));
	de_node_set_local_rotation(camera, de_quat_from_axis_angle(&pitch_rot, &right_axis, de_deg_to_rad(p->pitch)));

	/* listener */
	de_vec3_t pos;
	de_sound_context_t* ctx = de_core_get_sound_context(p->parent_level->game->core);
	de_listener_t* lst = de_sound_context_get_listener(ctx);
	de_node_get_global_position(p->camera, &pos);
	de_listener_set_orientation(lst, &look, &up_axis);
	de_listener_set_position(lst, &pos);
}