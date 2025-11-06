#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include "game.h"

using namespace std;

const Vector2 VEC2_ZERO = {0.0f, 0.0f};
const Vector2 VEC2_ONE = {1.0f, 1.0f};

enum Shape {
	RECT,
	ELLIPSE
};

enum EntityTag {
	UNKNOWN_ENTITY,
	BALL,
	PLAYER
};

struct Entity 
{
	Vector2 position = VEC2_ZERO;
	Vector2 velocity = VEC2_ZERO;
	Shape shape = RECT;
	float width = 1.0f;
	float height = 1.0f;
	float rotation = 0.0f;
	Color color = RAYWHITE;
	EntityTag tag = UNKNOWN_ENTITY;
	Vector2 scale = VEC2_ONE;

	float static_friction = 5.0f;
	float kinetic_friction = 3.0f; 

	float acceleration = 30.0f;
	Vector2 input = VEC2_ZERO;
	float top_speed = 3.0f;
};

const int ENTITY_CAP = 16;
int entity_count = 0;
Entity* entities[ENTITY_CAP];

void Vector2MoveToward(Vector2& vec, Vector2 vecEnd, float step)
{
	Vector2 diff = Vector2Subtract(vecEnd, vec);
	float diffLen = Vector2Length(diff);

	if (diffLen <= step)
	{
		vec = vecEnd;
	}
	else
	{
		vec = Vector2Add(vec, Vector2Scale(diff, step / diffLen) );
	}
}

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

	if (Vector2Length(player->input) != 0)
	{
		player->input = Vector2Scale(player->input, 1 / Vector2Length(player->input));
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
	if (Vector2Length(ent->velocity) < 0.05f)
	{
		Vector2MoveToward(ent->velocity, VEC2_ZERO, ent->static_friction * GetFrameTime());
	}
	else {
		if (ent->tag == PLAYER)
			cout << ent->velocity.x << ", " << ent->velocity.y;
		Vector2MoveToward(ent->velocity, VEC2_ZERO, ent->kinetic_friction * GetFrameTime());
		if (ent->tag == PLAYER)
			cout << " -> " << ent->velocity.x << ", " << ent->velocity.y << endl;
	}

	// apply acceleration
	ent->velocity = Vector2Add(ent->velocity, Vector2Scale(ent->input, ent->acceleration * GetFrameTime()));

	// top speed cap
	if (Vector2Length(ent->velocity) > ent->top_speed)
	{
		Vector2 dir = Vector2Scale(ent->velocity, 1 / Vector2Length(ent->velocity));
		ent->velocity = Vector2Scale(dir, ent->top_speed);
	}

	ent->position = Vector2Add(ent->position, ent->velocity);
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

			// === entity drawing ===
			DrawEntity(ent);
		}
	}
	EndTextureMode();
}
