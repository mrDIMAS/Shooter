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

static void quit_on_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	de_core_stop(game->core);
}

void menu_set_visible(menu_t* menu, bool visibility)
{
	de_gui_node_set_visibility(menu->main_page.window, visibility ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	menu->visible = visibility;
}

static void menu_on_new_game_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;

	DE_UNUSED(node);

	if (!game->level) {
		game->level = level_create_test(game);
	}

	menu_set_visible(game->main_menu, false);
}

static void menu_set_page(menu_t* menu, menu_page_type_t page)
{
	menu->page = page;

	de_gui_node_set_visibility(menu->settings_page.window, menu->page == MENU_PAGE_TYPE_SETTINGS ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	de_gui_node_set_visibility(menu->main_page.window, menu->page == MENU_PAGE_TYPE_MAIN ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
}

static void menu_on_save_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_save(game);
}

static void menu_on_load_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_load(game);
}

static void menu_on_settings_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	menu_set_page(game->main_menu, MENU_PAGE_TYPE_SETTINGS);
}

static void videomode_selector_item_getter(void* items, int n, char* out_buffer, int out_buffer_size)
{
	de_video_mode_t* videomode = ((de_video_mode_t*)items) + n;
	snprintf(out_buffer, out_buffer_size, "%dx%d@%d", videomode->width, videomode->height, videomode->bits_per_pixel);
}

static void videomode_selector_on_selection_changed(de_gui_node_t* node, int n)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	settings_page_t* page = (settings_page_t*)node->user_data;
	de_core_set_video_mode(page->menu->game->core, &page->video_modes.data[n]);
}

static void sound_volume_changed(de_gui_node_t* node, float old_value, float new_value)
{
	menu_t* menu = node->user_data;
	de_sound_context_set_master_volume(de_core_get_sound_context(menu->game->core), new_value);
	DE_UNUSED(node);
	DE_UNUSED(old_value);
}

static void music_volume_changed(de_gui_node_t* node, float old_value, float new_value)
{
	DE_UNUSED(node);
	DE_UNUSED(old_value);
	DE_UNUSED(new_value);
}

static void spot_shadows_checked_changed(de_gui_node_t* node, bool new_value)
{
	menu_t* menu = node->user_data;
	de_renderer_t* renderer = de_core_get_renderer(menu->game->core);
	de_quality_settings_t* quality_settings = de_renderer_get_quality_settings(renderer);
	quality_settings->spot_shadows_enabled = new_value;
	de_renderer_apply_quality_settings(renderer);
}

static void point_shadows_checked_changed(de_gui_node_t* node, bool new_value)
{
	menu_t* menu = node->user_data;
	de_renderer_t* renderer = de_core_get_renderer(menu->game->core);
	de_quality_settings_t* quality_settings = de_renderer_get_quality_settings(renderer);
	quality_settings->point_shadows_enabled = new_value;
	de_renderer_apply_quality_settings(renderer);
}

static void settings_page_init(menu_t* menu, settings_page_t* page)
{
	de_gui_t* gui = de_core_get_gui(menu->game->core);

	page->menu = menu;

	page->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
	de_gui_window_set_flags(page->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);
	de_gui_text_set_text_utf8(de_gui_window_get_title(page->window), "Settings");
	de_gui_node_attach(page->window, menu->root);
	de_gui_node_set_column(page->window, 1);
	de_gui_node_set_row(page->window, 1);

	/* Fucking Visual Studio keep messing up formatting here >:(
	 * I'm too fucking tired to fix it every time. */

	de_gui_node_t* page_grid = de_gui_node_create_with_desc(gui, DE_GUI_NODE_GRID, &(de_gui_node_descriptor_t) {
		.margin = (de_gui_thickness_t) { 
			.left = 5,
			.top = 5,
			.right = 5,
			.bottom = 5 
		}, 
		.s.grid = (de_gui_grid_descriptor_t) {
			.rows[0] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
			.rows[1] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
						
			.columns[0] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
		}
	});

	de_gui_node_t* stack_panel = de_gui_node_create_with_desc(gui, DE_GUI_NODE_STACK_PANEL, &(de_gui_node_descriptor_t) {
		.margin = (de_gui_thickness_t) { 
			.left = 2,
			.top = 2,
			.right = 2,
			.bottom = 2
		},  
		.row = 1,
		.column = 0,
		.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT,
		.parent = page_grid,
		.s.stack_panel = (de_gui_stack_panel_descriptor_t) {
			.orientation = DE_GUI_ORIENTATION_HORIZONTAL
		}
	});

	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.margin = (de_gui_thickness_t) {
			.left = 2,
			.top = 2,
			.right = 2,
			.bottom = 2
		},
		.width = 100,
		.parent = stack_panel,
		.s.button = (de_gui_button_descriptor_t) {
			.text = "Back"
		}
	});
		
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.margin = (de_gui_thickness_t) {
			.left = 2,
			.top = 2,
			.right = 2,
			.bottom = 2
		},
		.width = 100,
		.parent = stack_panel,
		.s.button = (de_gui_button_descriptor_t) {
			.text = "Apply"
		}
	});

	de_gui_node_t* grid = de_gui_node_create_with_desc(gui, DE_GUI_NODE_GRID, &(de_gui_node_descriptor_t) {
		.margin = (de_gui_thickness_t) { 
			.left = 5,
			.top = 5,
			.right = 5,
			.bottom = 5 
		}, 
		.row = 0,
		.column = 0,
		.parent = page_grid,
		.s.grid = (de_gui_grid_descriptor_t) {
			.rows[0] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[1] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[2] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[3] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[4] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[5] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[6] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.rows[7] = { .desired_height = 40, .size_mode = DE_GUI_SIZE_MODE_STRICT },

			.columns[0] = { .desired_width = 140, .size_mode = DE_GUI_SIZE_MODE_STRICT },
			.columns[1] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
		}
	});

	/* videomode */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 0,
			.column = 0,
			.parent = grid,
			.margin = (de_gui_thickness_t) { .left = 5 },
			.s.text_block = (de_gui_text_descriptor_t) {
				.text = "Video Mode",
				.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_t* selector = de_gui_node_create(gui, DE_GUI_NODE_SLIDE_SELECTOR);
		de_gui_slide_selector_set_selection_changed(selector, videomode_selector_on_selection_changed);
		page->video_modes = de_enum_video_modes();
		de_gui_slide_selector_set_items(selector, page->video_modes.data, page->video_modes.size, videomode_selector_item_getter);
		char buf[512];
		snprintf(buf, sizeof(buf), "%dx%d", de_core_get_window_width(menu->game->core), de_core_get_window_height(menu->game->core));
		de_gui_slide_selector_override_selection_text(selector, buf);
		de_gui_node_apply_descriptor(selector, &(de_gui_node_descriptor_t) {
			.row = 0,
				.column = 1,
				.parent = grid,
				.user_data = page,
				.margin = (de_gui_thickness_t) { .left = 5 }
		});
	}

	/* fullscreen */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 1,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Fullscreen", .vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_CHECK_BOX, &(de_gui_node_descriptor_t){
			.row = 1,
				.column = 1,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.width = 20,
				.height = 20,
				.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
				.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,
				.s.check_box = (de_gui_check_box_descriptor_t) { .checked = true }
		});
	}

	/* spot shadows */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 2,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Spot shadows", .vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_CHECK_BOX, &(de_gui_node_descriptor_t){
			.row = 2,
				.column = 1,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.width = 20,
				.height = 20,
				.user_data = menu,
				.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
				.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,
				.s.check_box = (de_gui_check_box_descriptor_t)
			{
				.checked = true,
					.checked_changed = spot_shadows_checked_changed
			}
		});
	}

	/* point shadows */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 3,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Point shadows", .vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_CHECK_BOX, &(de_gui_node_descriptor_t){
			.row = 3,
				.column = 1,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.width = 20,
				.height = 20,
				.user_data = menu,
				.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
				.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,
				.s.check_box = (de_gui_check_box_descriptor_t)
			{
				.checked = true,
					.checked_changed = point_shadows_checked_changed
			}
		});
	}

	/* point shadow map size */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 4,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Point shadow map size", .vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_CHECK_BOX, &(de_gui_node_descriptor_t){
			.row = 4,
				.column = 1,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.width = 20,
				.height = 20,
				.user_data = menu,
				.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
				.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,
				.s.check_box = (de_gui_check_box_descriptor_t)
			{
				.checked = true,
					.checked_changed = point_shadows_checked_changed
			}
		});
	}

	/* player name */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 5,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Player Name",
					.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_t* text_box = de_gui_node_create(gui, DE_GUI_NODE_TEXT_BOX);
		de_gui_node_apply_descriptor(text_box, &(de_gui_node_descriptor_t) {
			.row = 5,
				.column = 1,
				.parent = grid,
				.margin = de_gui_thickness_uniform(2)
		});
	}

	/* sound volume */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 6,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Sound Volume",
					.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_SCROLL_BAR, &(de_gui_node_descriptor_t) {
			.row = 6,
				.column = 1,
				.parent = grid,
				.margin = de_gui_thickness_uniform(2),
				.user_data = menu,
				.s.scroll_bar = (de_gui_scroll_bar_descriptor_t)
			{
				.value_changed = sound_volume_changed,
					.min = 0.0f,
					.max = 1.0f,
					.step = 0.05f,
					.value = 1.0
			}
		});
	}

	/* music volume */
	{
		de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
			.row = 7,
				.column = 0,
				.parent = grid,
				.margin = (de_gui_thickness_t) { .left = 5 },
				.s.text_block = (de_gui_text_descriptor_t)
			{
				.text = "Music Volume",
					.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			}
		});

		de_gui_node_create_with_desc(gui, DE_GUI_NODE_SCROLL_BAR, &(de_gui_node_descriptor_t) {
			.row = 7,
				.column = 1,
				.parent = grid,
				.margin = de_gui_thickness_uniform(2),
				.user_data = menu,
				.s.scroll_bar = (de_gui_scroll_bar_descriptor_t)
			{
				.value_changed = music_volume_changed,
					.min = 0.0f,
					.max = 1.0f,
					.step = 0.05f,
					.value = 1.0
			}
		});
	}

	de_gui_window_set_content(page->window, page_grid);
}

static void settings_page_deinit(settings_page_t* page)
{
	DE_ARRAY_FREE(page->video_modes);
}

static void main_page_init(menu_t* menu, main_page_t* page)
{
	de_gui_t* gui = de_core_get_gui(menu->game->core);

	page->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
	de_gui_text_set_text_utf8(de_gui_window_get_title(page->window), "Shooter");
	de_gui_window_set_flags(page->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);
	de_gui_node_attach(page->window, menu->root);
	de_gui_node_set_column(page->window, 1);
	de_gui_node_set_row(page->window, 1);

	de_gui_node_t* grid = de_gui_node_create_with_desc(gui, DE_GUI_NODE_GRID, &(de_gui_node_descriptor_t) {
		.s.grid = (de_gui_grid_descriptor_t)
		{
			.rows[0] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[1] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[2] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[3] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[4] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[5] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.columns[0] = { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
		}
	});

	/* new game */
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.row = 0,
			.parent = grid,
			.margin = de_gui_thickness_uniform(10),
			.s.button = (de_gui_button_descriptor_t)
		{
			.text = "New Game",
				.click = (de_gui_callback_t)
			{
				.func = menu_on_new_game_click,
					.arg = menu->game
			}
		}
	});

	/* save game */
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.row = 1,
			.parent = grid,
			.margin = de_gui_thickness_uniform(10),
			.s.button = (de_gui_button_descriptor_t)
		{
			.text = "Save Game",
				.click = (de_gui_callback_t)
			{
				.func = menu_on_save_click,
					.arg = menu->game
			}
		}
	});

	/* load game */
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.row = 2,
			.parent = grid,
			.margin = de_gui_thickness_uniform(10),
			.s.button = (de_gui_button_descriptor_t)
		{
			.text = "Load Game",
				.click = (de_gui_callback_t)
			{
				.func = menu_on_load_click,
					.arg = menu->game
			}
		}
	});

	/* settings */
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.row = 3,
			.parent = grid,
			.margin = de_gui_thickness_uniform(10),
			.s.button = (de_gui_button_descriptor_t)
		{
			.text = "Settings",
				.click = (de_gui_callback_t)
			{
				.func = menu_on_settings_click,
					.arg = menu->game
			}
		}
	});

	/* quit */
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_BUTTON, &(de_gui_node_descriptor_t) {
		.row = 4,
			.parent = grid,
			.margin = de_gui_thickness_uniform(10),
			.s.button = (de_gui_button_descriptor_t)
		{
			.text = "Quit",
				.click = (de_gui_callback_t)
			{
				.func = quit_on_click,
					.arg = menu->game
			}
		}
	});

	de_gui_window_set_content(page->window, grid);
}

menu_t* menu_create(game_t* game)
{
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

	de_gui_node_set_size(menu->root,
		(float)de_core_get_window_width(game->core),
		(float)de_core_get_window_height(game->core)
	);
	
	de_path_t path;
	de_path_from_cstr_as_view(&path, "data/sounds/Sonic_Mayhem_Collapse.wav");
	de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path);
	if (res) {
		de_resource_set_flags(res, DE_RESOURCE_FLAG_PERSISTENT);
		de_sound_buffer_t* buffer = de_resource_to_sound_buffer(res);
		de_sound_buffer_set_flags(buffer, DE_SOUND_BUFFER_FLAGS_STREAM);
		menu->music = de_sound_source_create(de_core_get_sound_context(game->core), DE_SOUND_SOURCE_TYPE_2D);
		de_sound_source_set_loop(menu->music, true);
		de_sound_source_set_buffer(menu->music, de_resource_to_sound_buffer(res));
		de_sound_source_play(menu->music);
	}
	
	main_page_init(menu, &menu->main_page);
	settings_page_init(menu, &menu->settings_page);

	menu_set_page(menu, MENU_PAGE_TYPE_MAIN);

	return menu;
}

bool menu_process_event(menu_t* menu, de_event_t* evt)
{
	switch (evt->type) {
		case DE_EVENT_TYPE_KEY_DOWN:
			if (evt->s.key.key == DE_KEY_ESC) {
				if (menu->page != MENU_PAGE_TYPE_MAIN) {
					menu_set_page(menu, MENU_PAGE_TYPE_MAIN);
				} else {
					if (menu->game->level) {
						menu_set_visible(menu, !menu->visible);
						hud_set_visible(menu->game->hud, !menu->visible);
					} else {
						menu_set_visible(menu, true);
					}
				}
				return true;
			}
			break;
		case DE_EVENT_TYPE_RESIZE:
			de_gui_node_set_size(menu->root, (float)evt->s.resize.w, (float)evt->s.resize.h);
			break;
		default:
			break;
	}
	return false;
}

void menu_free(menu_t* menu)
{
	settings_page_deinit(&menu->settings_page);
	if (menu->music) {
		de_sound_source_free(menu->music);
	}
	de_free(menu);
}