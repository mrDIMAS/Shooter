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

hud_t* hud_create(game_t* game)
{
	hud_t* hud = DE_NEW(hud_t);
	de_gui_t* gui = de_core_get_gui(game->core);
	hud->game = game;
	hud->root = de_gui_node_create_with_desc(gui, DE_GUI_NODE_GRID, &(de_gui_node_descriptor_t) {
		.width = (float)de_core_get_window_width(game->core),
			.height = (float)de_core_get_window_height(game->core),
			.visibility = DE_GUI_NODE_VISIBILITY_COLLAPSED,
			.s.grid = (de_gui_grid_descriptor_t)
		{
			.columns[0] = (de_gui_grid_column_descriptor_t) { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
				.rows[0] = (de_gui_grid_row_descriptor_t) { .size_mode = DE_GUI_SIZE_MODE_STRETCH },
		}
	});

	de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
		.parent = hud->root,
			.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_BOTTOM,
			.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,
			.width = 100,
			.height = 30,
			.row = 0, .column = 0,
			.s.text_block = (de_gui_text_descriptor_t)
		{
			.text = "Health"
		}
	});

	de_gui_node_create_with_desc(gui, DE_GUI_NODE_TEXT, &(de_gui_node_descriptor_t) {
		.parent = hud->root,
			.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_BOTTOM,
			.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT,
			.width = 100,
			.height = 30,
			.row = 0, .column = 0,
			.s.text_block = (de_gui_text_descriptor_t)
		{
			.text = "Ammo"
		}
	});

	de_path_t crosshair;
	de_path_from_cstr_as_view(&crosshair, "data/textures/crosshair.tga");
	de_gui_node_create_with_desc(gui, DE_GUI_NODE_IMAGE, &(de_gui_node_descriptor_t) {
		.parent = hud->root,
			.vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_CENTER,
			.horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_CENTER,
			.width = 33,
			.height = 33,
			.row = 0, .column = 0,
			.s.image = (de_gui_image_descriptor_t)
		{
			.texture = de_resource_to_texture(de_core_request_resource(game->core, DE_RESOURCE_TYPE_TEXTURE,
				&crosshair, DE_RESOURCE_FLAG_PERSISTENT))
		}
	});

	return hud;
}

void hud_free(hud_t* hud)
{
	de_free(hud);
}

void hud_set_visible(hud_t* hud, bool visible)
{
	de_gui_node_set_visibility(hud->root, visible ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);	
}

bool hud_process_event(hud_t* hud, de_event_t* evt)
{
	switch (evt->type) {
		case DE_EVENT_TYPE_RESIZE:
			de_gui_node_set_size(hud->root, (float)evt->s.resize.w, (float)evt->s.resize.h);
			break;
		default:
			break;
	}
	/* none of events will be consumed */
	return false;
}