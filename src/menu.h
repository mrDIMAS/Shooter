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

typedef enum menu_page_type_t {
	MENU_PAGE_TYPE_MAIN,
	MENU_PAGE_TYPE_SETTINGS,
} menu_page_type_t;

typedef struct main_page_t {
	de_gui_node_t* window;
} main_page_t;

typedef struct settings_page_t {
	menu_t* menu;
	de_gui_node_t* window;
	de_video_mode_array_t video_modes;
	de_gui_node_t* sound_volume;
	de_gui_node_t* music_volume;
} settings_page_t;

struct menu_t {
	de_gui_node_t* root;
	de_sound_source_t* music;
	game_t* game;
	main_page_t main_page;
	settings_page_t settings_page;
	menu_page_type_t page;	
	bool visible;
};

menu_t* menu_create(game_t* game);

void menu_set_visible(menu_t* menu, bool visibility);

void menu_free(menu_t* menu);

bool menu_process_event(menu_t* menu, de_event_t* evt);