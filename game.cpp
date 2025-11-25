// TODO
// maybe rewrite velocity to be in pixels / sec
// and acceleration is pixels/s^2

#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include "game.h"
 
using namespace std;

const float STATIC_FRICTION_THRESHOLD = 0.03f;

enum Shape {
	ELLIPSE,
	RECT
};

enum EntityTag {
	UNKNOWN_ENTITY,
	BALL,
	PLAYER
};

struct Entity 
{
	Vector2 position = Vector2Zero();
	Vector2 old_pos = Vector2Zero(); // old positition before move, used for collisions
	Vector2 velocity = Vector2Zero();
	Shape shape = ELLIPSE;
	float width = 1.0f;
	float height = 1.0f;
	bool solid = true; // has collision
	float rotation = 0.0f;
	Color color = RAYWHITE;
	EntityTag tag = UNKNOWN_ENTITY;
	Vector2 scale = Vector2One();

	float static_friction = 0.0f;
	float kinetic_friction = 0.0f; 

	float acceleration = 1500.0f;
	Vector2 input = Vector2Zero();
	float top_speed = 180.0f;

	// player stuff
	float dash_strength = 700.0f;
	const float DASH_COOLDOWN = 0.5f;
	float dash_cooldown = 0.0f;
};

const int ENTITY_CAP = 16;
int entity_count = 0;
Entity* entities[ENTITY_CAP];

void SpawnEntity(Entity* new_entity)
{
	entity_count++;
	entities[entity_count-1] = new_entity;
}

void DrawEntity(Entity* entity)
{
	if (entity->shape == RECT)
	{
		DrawRectanglePro(
				(Rectangle){entity->position.x, entity->position.y, entity->width, entity->height},
				(Vector2){0.0f, 0.0f},
				entity->rotation,
				entity->color
			);
	}
	else if (entity->shape == ELLIPSE)
	{
		DrawEllipse(
				entity->position.x,
				entity->position.y,
				entity->width * entity->scale.x,
				entity->height * entity->scale.y,
				entity->color
			);
	}
}

void PlayerControl(Entity* player)
{
	if (IsKeyDown(KEY_D))
	{
		player->input.x = 1.0f;
	}
	else if (IsKeyDown(KEY_A))
	{
		player->input.x = -1.0f;
	}
	else
	{
		player->input.x = 0;
	}

	if (IsKeyDown(KEY_W))
	{
		player->input.y = -1.0f;
	}
	else if (IsKeyDown(KEY_S))
	{
		player->input.y = 1.0f;
	}
	else 
	{
		player->input.y = 0;
	}

	// dashing

	player->dash_cooldown -= GetFrameTime();

	if (IsKeyDown(KEY_SPACE) and player->input != Vector2Zero() and player->dash_cooldown <= 0)
	{
		player->input = Vector2Normalize(player->input);
		player->velocity = player->input * player->dash_strength;
		player->dash_cooldown = player->DASH_COOLDOWN;
	}
}

// =============================================================================================================

void Init()
{
	Entity* test_entity = new Entity;
	test_entity->position = (Vector2){50.0f, 50.0f};
	test_entity->width = 20.0f;
	test_entity->height = 10.0f;
	test_entity->tag = BALL;
	SpawnEntity(test_entity);

	Entity* player = new Entity;
	player->position = (Vector2){10.0f, 10.0f};
	player->shape = ELLIPSE;
	player->width = 8.0f;
	player->height = 8.0f;
	player->color = ORANGE;
	player->tag = PLAYER;
	SpawnEntity(player);
}

void MoveEntity(Entity* ent)
{
	// apply friction
	if (Vector2Length(ent->velocity) < STATIC_FRICTION_THRESHOLD)
	{
		Vector2MoveTowards(ent->velocity, Vector2Zero(), ent->static_friction * GetFrameTime());
	}
	else {
		Vector2MoveTowards(ent->velocity, Vector2Zero(), ent->kinetic_friction * GetFrameTime());
	}


	// apply acceleration (respects top_speed)
	ent->input = Vector2Normalize(ent->input);
	ent->velocity = Vector2MoveTowards(ent->velocity, ent->input * ent->top_speed, ent->acceleration * GetFrameTime());

	// actually move
	ent->position += ent->velocity * GetFrameTime();
}

void Update(const RenderTexture2D& frame)
{
	BeginTextureMode(frame);
	{
		ClearBackground(PINK);

		for (int i = 0; i < entity_count; i++)
		{
			Entity*& ent = entities[i];

			// === entity movement ===
			MoveEntity(ent);
			
			// === player control ===
			if (ent->tag == PLAYER)
			{
				PlayerControl(ent);
			}

		}

		// === entity collisions ===
		for (int i = 0; i < entity_count; i++)
		{
			Entity*& ent = entities[i];
			if (!ent->solid)
			{
				continue;
			}

			for (int j = 0; j < entity_count; j++)
			{
				if (i == j)
				{
					continue;
				}
				Entity*& otherEnt = entities[j];

				if (CheckCollisionCircles(ent->position, ent->width, otherEnt->position, otherEnt->width))
				{
					// collision!
				}
			}
		}

		// === entity drawing ===
		for (int i = 0; i < entity_count; i++)
		{
			DrawEntity(entities[i]);
		}
	}
	EndTextureMode();
}
