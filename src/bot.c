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

#define UNUSED(...) (void)(__VA_ARGS__)

static void bot_init(actor_t* actor)
{
	bot_t* bot = actor_to_bot(actor);
	de_path_t path;
	de_path_from_cstr_as_view(&path, "data/models/soldier/soldier.fbx");
	de_resource_t* soldier_model = de_core_request_resource(actor->parent_level->game->core, DE_RESOURCE_TYPE_MODEL, &path);
	if (soldier_model) {
		bot->model = de_model_instantiate(de_resource_to_model(soldier_model), actor->parent_level->scene);
		de_node_attach(bot->model, actor->pivot);
	}
}

static void bot_deinit(actor_t* actor)
{
	DE_UNUSED(actor);
}

static bool bot_visit(de_object_visitor_t* visitor, actor_t* actor)
{
	bool result = true;
	DE_UNUSED(visitor);
    DE_UNUSED(actor);
	return result;
}

static void bot_update(actor_t* actor)
{
	DE_UNUSED(actor);
}

actor_dispatch_table_t* bot_get_dispatch_table()
{
	static actor_dispatch_table_t table = {
		.init = bot_init,
		.deinit = bot_deinit,
		.visit = bot_visit,
		.update = bot_update
	};
	return &table;
}