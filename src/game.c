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

#include "game.h"

/* include rest of modules directly */
#include "weapon.c"
#include "level.c"
#include "player.c"
#include "menu.c"
#include "bot.c"
#include "actor.c"
#include "hud.c"
#include "projectile.c"

bool game_save(game_t* game)
{
	bool result = false;
	if (game->level) {
		result = true;
		de_object_visitor_t visitor;
		de_object_visitor_init(game->core, &visitor);
		result &= de_core_visit(&visitor, game->core);
		result &= DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Level", &game->level, level_visit);
		/* debug output */
		{
			FILE* temp = fopen("save1.txt", "w");
			de_object_visitor_print_tree(&visitor, temp);
			fclose(temp);
		}
		de_object_visitor_save_binary(&visitor, "save1.bin");
		de_object_visitor_free(&visitor);
	}
	return result;
}

bool game_load(game_t* game)
{
	de_object_visitor_t visitor;
	bool result = true;
	de_object_visitor_load_binary(game->core, &visitor, "save1.bin");
	result &= de_core_visit(&visitor, game->core);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Level", &game->level, level_visit);
	de_object_visitor_free(&visitor);
	menu_set_visible(game->main_menu, false);
	return result;
}

static game_t* game_create(void)
{
	game_t* game = DE_NEW(game_t);
	de_log_open("Shooter.log");

	/* Init core */
	game->core = de_core_init(&(de_core_config_t)
	{
		.video_mode = (de_video_mode_t)
		{
			.width = 1350,
				.height = 690,
				.bits_per_pixel = 32,
				.fullscreen = false
		},
		.title = "Shooter"
	});
	de_core_set_user_pointer(game->core, game);
	de_renderer_set_framerate_limit(de_core_get_renderer(game->core), 60);

	/* Create menu */
	game->main_menu = menu_create(game);

	game->hud = hud_create(game);

	/* Create overlay */
	game->fps_text = de_gui_node_create(de_core_get_gui(game->core), DE_GUI_NODE_TEXT);

	return game;
}

static void game_main_loop(game_t* game)
{
	de_renderer_t* renderer = de_core_get_renderer(game->core);
	de_gui_t* gui = de_core_get_gui(game->core);
	const double fixed_fps = 60.0;
	const double fixed_timestep = 1.0f / fixed_fps;
	double game_clock = de_time_get_seconds();
	while (de_core_is_running(game->core)) {
		double dt = de_time_get_seconds() - game_clock;
		while (dt >= fixed_timestep) {
			dt -= fixed_timestep;
			game_clock += fixed_timestep;

			de_event_t evt;
			while (de_core_poll_event(game->core, &evt)) {
				bool processed = menu_process_event(game->main_menu, &evt);
				if (!processed) {
					processed = hud_process_event(game->hud, &evt);
				}
				if (!processed) {
					processed = de_gui_process_event(gui, &evt);
				}
				if (!processed && game->level) {
					processed = player_process_event(game->level->player, &evt);
				}
			}

			de_sound_context_update(de_core_get_sound_context(game->core));
			de_gui_update(gui);
			de_physics_step(game->core, fixed_timestep);
			DE_LINKED_LIST_FOR_EACH_H(de_scene_t*, scene, de_core_get_first_scene(game->core))
			{
				de_scene_update(scene, fixed_timestep);
			}

			if (game->level && !game->main_menu->visible) {
				level_update(game->level);
			}

			if (dt >= 4 * fixed_timestep) {
				game_clock = de_time_get_seconds();
				break;
			}
		}

		de_renderer_render(renderer);

		/* print statistics */
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "Frame time: %.2f ms\nFPS: (Mean: %d; Current: %d; Min: %d)\nAllocations: %d",
			de_render_get_frame_time(renderer),
			(int)de_renderer_get_mean_fps(renderer), (int)renderer->current_fps, (int)renderer->min_fps,
			(int)de_get_alloc_count());
		de_gui_text_set_text_utf8(game->fps_text, buffer);
	}
}

static void game_close(game_t* game)
{
	if (game->level) {
		level_free(game->level);
	}

	hud_free(game->hud);

	menu_free(game->main_menu);

	de_core_shutdown(game->core);

	de_free(game);
}

int main(int argc, char** argv)
{
	de_graph_tests();

	DE_UNUSED(argc);
    DE_UNUSED(argv);
	
	game_t* game = game_create();
		
	game_main_loop(game);

	game_close(game);

	return 0;
}
