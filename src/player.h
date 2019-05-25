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

typedef struct player_controller_t {
	bool move_forward, move_backward;
	bool strafe_left, strafe_right;
	bool crouch, run, jumped;
} player_controller_t;

struct player_t {
	de_node_t* camera;
	de_node_t* flash_light;
	de_node_t* weapon_pivot;
	float pitch;
	float desired_pitch;
	float yaw;
	float desired_yaw;
	float move_speed;
	float stand_body_radius;
	float crouch_body_radius;
	float stand_up_speed;
	float sit_down_speed;
	float run_speed_multiplier;
	float camera_wobble;
	float path_len;
	de_vec3_t camera_offset;
	de_vec3_t camera_dest_offset;
	de_vec3_t camera_position;
	de_vec3_t weapon_dest_offset;
	de_vec3_t weapon_offset;
	de_vec3_t weapon_position;
	int current_weapon;
	DE_ARRAY_DECLARE(weapon_t*, weapons);
	player_controller_t controller;
	DE_ARRAY_DECLARE(de_sound_source_t*, footsteps);
	de_node_t* laser_dot;
};

actor_dispatch_table_t* player_get_dispatch_table();

void player_add_weapon(player_t* player, weapon_t* wpn);

void player_remove_weapon(player_t* player, weapon_t* wpn);

void player_next_weapon(player_t* player);

void player_prev_weapon(player_t* player);