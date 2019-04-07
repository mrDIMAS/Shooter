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

static void quit_on_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	de_core_stop(game->core);
}

void menu_set_visible(menu_t* menu, bool visibility) {	
	de_gui_node_set_visibility(menu->main_page.window, visibility ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	menu->visible = visibility;
}

static void menu_on_new_game_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;

	DE_UNUSED(node);

	if (!game->level) {
		game->level = level_create_test(game);
	}

	menu_set_visible(game->main_menu, false);
}

static void menu_set_page(menu_t* menu, menu_page_type_t page) {
	menu->page = page;

	de_gui_node_set_visibility(menu->settings_page.window, menu->page == MENU_PAGE_TYPE_SETTINGS ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	de_gui_node_set_visibility(menu->main_page.window, menu->page == MENU_PAGE_TYPE_MAIN ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
}

static void menu_on_save_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_save(game);
}

static void menu_on_load_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_load(game);
}

static void menu_on_settings_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	menu_set_page(game->main_menu, MENU_PAGE_TYPE_SETTINGS);
}

static void videomode_selector_item_getter(void* items, int n, char* out_buffer, int out_buffer_size) {
	de_video_mode_t* videomode = ((de_video_mode_t*)items) + n;
	snprintf(out_buffer, out_buffer_size, "%dx%d@%d", videomode->width, videomode->height, videomode->bits_per_pixel);
}

static void videomode_selector_on_selection_changed(de_gui_node_t* node, int n) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	settings_page_t* page = (settings_page_t*)node->user_data;
	de_core_set_video_mode(page->menu->game->core, &page->video_modes.data[n]);
}

static void sound_volume_changed(de_gui_node_t* node) {
	DE_UNUSED(node);
}

static void music_volume_changed(de_gui_node_t* node) {
	DE_UNUSED(node);
}

static void settings_page_init(menu_t* menu, settings_page_t* page) {
	de_gui_t* gui = de_core_get_gui(menu->game->core);

	page->menu = menu;

	page->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
	de_gui_window_set_flags(page->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);
	de_gui_text_set_text_utf8(de_gui_window_get_title(page->window), "Settings");
	de_gui_node_attach(page->window, menu->root);
	de_gui_node_set_column(page->window, 1);
	de_gui_node_set_row(page->window, 1);

	/* content */	
	de_gui_node_t* grid = de_gui_node_create(gui, DE_GUI_NODE_GRID);
	de_gui_grid_add_column(grid, 140, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 40, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_row(grid, 40, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_row(grid, 40, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_row(grid, 40, DE_GUI_SIZE_MODE_STRICT);
	
	/* videomode */
	{
		de_gui_node_t* lbl = de_gui_node_create(gui, DE_GUI_NODE_TEXT);
		de_gui_text_set_vertical_alignment(lbl, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
		de_gui_node_apply_descriptor(lbl, &(de_gui_node_descriptor_t) {
			.row = 0, .column = 0, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 5 },
			.s.text_block = (de_gui_text_block_descriptor_t) {
				.text = "Video Mode"
			}
		});
		
		de_gui_node_t* selector = de_gui_node_create(gui, DE_GUI_NODE_SLIDE_SELECTOR);
		de_gui_slide_selector_set_selection_changed(selector, videomode_selector_on_selection_changed);
		page->video_modes = de_enum_video_modes();
		de_gui_slide_selector_set_items(selector, page->video_modes.data, page->video_modes.size, videomode_selector_item_getter);
		de_gui_node_apply_descriptor(selector, &(de_gui_node_descriptor_t) {
			.row = 0, .column = 1, .parent = grid, .user_data = page,
			.margin = (de_gui_thickness_t) { .left = 5 }
		});
	}

	/* player name */
	{
		de_gui_node_t* lbl = de_gui_node_create(gui, DE_GUI_NODE_TEXT);		
		de_gui_text_set_vertical_alignment(lbl, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
		de_gui_node_apply_descriptor(lbl, &(de_gui_node_descriptor_t) {
			.row = 1, .column = 0, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 5 },
			.s.text_block = (de_gui_text_block_descriptor_t) {
				.text = "Player Name"
			}
		});

		de_gui_node_t* text_box = de_gui_node_create(gui, DE_GUI_NODE_TEXT_BOX);
		de_gui_node_apply_descriptor(text_box, &(de_gui_node_descriptor_t) {
			.row = 1, .column = 1, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 2, .top = 2, .right = 2, .bottom = 2 }			
		});
	}

	/* sound volume */
	{
		de_gui_node_t* lbl = de_gui_node_create(gui, DE_GUI_NODE_TEXT);
		de_gui_text_set_text_utf8(lbl, "Sound Volume");
		de_gui_text_set_vertical_alignment(lbl, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
		de_gui_node_apply_descriptor(lbl, &(de_gui_node_descriptor_t) {
			.row = 2, .column = 0, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 5 }			
		});				

		page->sound_volume = de_gui_node_create(gui, DE_GUI_NODE_SCROLL_BAR);
		de_gui_scroll_bar_set_value_changed(page->sound_volume, sound_volume_changed);
		de_gui_node_apply_descriptor(page->sound_volume, &(de_gui_node_descriptor_t) {
			.row = 2, .column = 1, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 2, .top = 2, .right = 2, .bottom = 2 }		
		});		
	}

	/* music volume */
	{
		de_gui_node_t* lbl = de_gui_node_create(gui, DE_GUI_NODE_TEXT);
		de_gui_text_set_text_utf8(lbl, "Music Volume");
		de_gui_text_set_vertical_alignment(lbl, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
		de_gui_node_apply_descriptor(lbl, &(de_gui_node_descriptor_t) {
			.row = 3, .column = 0, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 5 }
		});

		page->music_volume = de_gui_node_create(gui, DE_GUI_NODE_SCROLL_BAR);
		de_gui_scroll_bar_set_value_changed(page->music_volume, music_volume_changed);
		de_gui_node_apply_descriptor(page->music_volume, &(de_gui_node_descriptor_t) {
			.row = 3, .column = 1, .parent = grid,
			.margin = (de_gui_thickness_t) { .left = 2, .top = 2, .right = 2, .bottom = 2 }
		});
	}

	de_gui_window_set_content(page->window, grid);	
}

static void settings_page_deinit(settings_page_t* page) {
	DE_ARRAY_FREE(page->video_modes);
}

static void main_page_init(menu_t* menu, main_page_t* page) {
	de_gui_t* gui = de_core_get_gui(menu->game->core);

	page->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
	de_gui_text_set_text_utf8(de_gui_window_get_title(page->window), "Shooter");
	de_gui_window_set_flags(page->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);
	de_gui_node_attach(page->window, menu->root);
	de_gui_node_set_column(page->window, 1);
	de_gui_node_set_row(page->window, 1);

	de_gui_node_t* grid = de_gui_node_create(gui, DE_GUI_NODE_GRID);
	de_gui_node_set_desired_size(grid, 100, 100);
	de_gui_node_set_desired_local_position(grid, 10, 10);
	de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);

	/* new game */
	de_gui_node_t* new_game = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(new_game), "New Game");
	de_gui_node_set_row(new_game, 0);
	de_gui_node_attach(new_game, grid);
	de_gui_node_set_margin_uniform(new_game, 10);
	de_gui_button_set_click(new_game, menu_on_new_game_click, menu->game);
	
	/* save game */
	de_gui_node_t* save = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(save), "Save Game");
	de_gui_node_set_row(save, 1);
	de_gui_node_attach(save, grid);
	de_gui_node_set_margin_uniform(save, 10);
	de_gui_button_set_click(save, menu_on_save_click, menu->game);

	/* load game */
	de_gui_node_t* load = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(load), "Load Game");
	de_gui_node_set_row(load, 2);
	de_gui_node_attach(load, grid);
	de_gui_node_set_margin_uniform(load, 10);
	de_gui_button_set_click(load, menu_on_load_click, menu->game);
	
    /* settings */
	de_gui_node_t* settings = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(settings), "Settings");
	de_gui_node_set_row(settings, 3);
	de_gui_node_attach(settings, grid);
	de_gui_node_set_margin_uniform(settings, 10);
	de_gui_button_set_click(settings, menu_on_settings_click, menu->game);
	
    /* quit */
	de_gui_node_t* quit = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(quit), "Quit");
	de_gui_node_set_row(quit, 4);
	de_gui_node_attach(quit, grid);
	de_gui_node_set_margin_uniform(quit, 10);
	de_gui_button_set_click(quit, quit_on_click, menu->game);
	
	de_gui_window_set_content(page->window, grid);
}

menu_t* menu_create(game_t* game) {
	de_gui_t* gui = de_core_get_gui(game->core);
	menu_t* menu = DE_NEW(menu_t);
	menu->game = game;

	/* root grid for menu pages, will perform automatic layout when
	 * resolution or main render window size changing.
	 * -------------
	 * |   |   |   |
	 * |---|---|---|
	 * |   | * |   |  <-- Every page will live in central cell
	 * |---|---|---|
	 * |   |   |   |
	 * -------------
	 */
	menu->root = de_gui_node_create(gui, DE_GUI_NODE_GRID);

	de_gui_grid_add_row(menu->root, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(menu->root, 500, DE_GUI_SIZE_MODE_STRICT); // central row
	de_gui_grid_add_row(menu->root, 0, DE_GUI_SIZE_MODE_STRETCH);

	de_gui_grid_add_column(menu->root, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(menu->root, 400, DE_GUI_SIZE_MODE_STRICT); // central column
	de_gui_grid_add_column(menu->root, 0, DE_GUI_SIZE_MODE_STRETCH);

	{
		de_path_t path;
		de_path_from_cstr_as_view(&path, "data/sounds/test.wav");
		de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path,
			DE_RESOURCE_FLAG_PERSISTENT);
		de_sound_buffer_t* buffer = de_resource_to_sound_buffer(res);
		de_sound_buffer_set_flags(buffer, DE_SOUND_BUFFER_FLAGS_STREAM);
		menu->music = de_sound_source_create(de_core_get_sound_context(game->core), DE_SOUND_SOURCE_TYPE_2D);
		de_sound_source_set_buffer(menu->music, de_resource_to_sound_buffer(res));
		//de_sound_source_play(menu->music);
	}

	main_page_init(menu, &menu->main_page);
	settings_page_init(menu, &menu->settings_page);

	menu_set_page(menu, MENU_PAGE_TYPE_MAIN);

	return menu;
}

bool menu_process_event(menu_t* menu, de_event_t* evt) {
	switch (evt->type) {
		case DE_EVENT_TYPE_KEY_DOWN:
			if (evt->s.key.key == DE_KEY_ESC) {
				if (menu->page != MENU_PAGE_TYPE_MAIN) {
					menu_set_page(menu, MENU_PAGE_TYPE_MAIN);
				} else {
					if (menu->game->level) {
						menu_set_visible(menu, !menu->visible);
					} else {
						menu_set_visible(menu, true);
					}
				}
				return true;
			}
			break;
		case DE_EVENT_TYPE_RESIZE:
			de_gui_node_set_desired_size(menu->root, (float)evt->s.resize.w, (float)evt->s.resize.h);
			break;
		default:
			break;
	}
	return false;
}

void menu_free(menu_t* menu) {
	settings_page_deinit(&menu->settings_page);
	de_sound_source_free(menu->music);
	de_free(menu);
}