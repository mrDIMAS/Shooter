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

#include "de_main.h"

/* forward declaractions */
typedef struct game_t game_t;
typedef struct menu_t menu_t;
typedef struct level_t level_t;
typedef struct player_t player_t;
typedef struct weapon_t weapon_t;
typedef struct bot_t bot_t;
typedef struct actor_t actor_t;
typedef struct hud_t hud_t;

struct game_t {
	de_core_t* core;
	level_t* level;
	menu_t* main_menu;
	hud_t* hud;
	de_gui_node_t* fps_text;
};

bool game_save(game_t* game);

bool game_load(game_t* game);


typedef struct actor_dispatch_table_t {
	void(*init)(actor_t* actor);
	void(*deinit)(actor_t* actor);
	void(*update)(actor_t* actor);
	bool(*visit)(de_object_visitor_t* visitor, actor_t* actor);
	bool(*process_event)(actor_t* actor, const de_event_t* evt);
} actor_dispatch_table_t;

#include "level.h"
#include "weapon.h"
#include "player.h"
#include "menu.h"
#include "bot.h"
#include "actor.h"
#include "hud.h"
#include "projectile.h"