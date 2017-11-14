#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Scene.h"
#include "ModulePlayer.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	for (pugi::xml_node iterator = config.child("maps"); iterator != nullptr; iterator = iterator.next_sibling("maps"))
	{
		maps.add(iterator.attribute("name").as_string());
	}
	current_map = maps.start;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load(current_map->data.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

//	debug_tex = App->tex->Load("maps/graveyard_tile.png");
	// Aixo pinta on estigui el ratoli, exactament nose perk ferho servir ara, pero bueno , esta aqui ...

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	ChangeScene();

	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	/*if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 1;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 1;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 1;*/

	if (App->player->playerData.x - (-App->render->camera.x + (App->render->camera.w / 2)) >= 0)
	{
		if (App->render->camera.x - App->render->camera.w > -(App->map->data.width*App->map->data.tile_width))
			App->render->camera.x -= App->player->playerData.speed;
	}
	if (App->player->playerData.x - (-App->render->camera.x + (App->render->camera.w / 2)) <= 0)
	{
		if (App->render->camera.x < 0)
			App->render->camera.x += App->player->playerData.speed;
	}

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}


bool j1Scene::ChangeScene() {
	if (App->map->IsCollidingWithGoal(App->player->playerData.x + App->player->playerData.w / 2, App->player->playerData.y + App->player->playerData.h, DOWN)
		|| App->map->IsCollidingWithGoal(App->player->playerData.x + App->player->playerData.w / 2, App->player->playerData.y, UP)
		|| App->map->IsCollidingWithGoal(App->player->playerData.x, App->player->playerData.y + App->player->playerData.h / 2, LEFT)
		|| App->map->IsCollidingWithGoal(App->player->playerData.x + App->player->playerData.w, App->player->playerData.y + App->player->playerData.h / 2, RIGHT))
	{
		App->map->CleanUp();
		App->tex->FreeTextures();
		App->player->LoadPLayerTexture();
		
		/*App->audio->;*/

		if (current_map->next == nullptr)
			current_map = App->scene->maps.start;
		else
			current_map = App->scene->current_map->next;

		App->map->Load(current_map->data.GetString());

		App->render->camera.x = 0;
		App->render->camera.y = 0;

		App->player->SpawnPLayer();
	}
	return true;
}