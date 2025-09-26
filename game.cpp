#include "raylib.h"
#include "game.h"

enum Shape {
	RECT,
	ELLIPSE
};

enum EntityTag {
	BALL = 0,
	PLAYER
};

struct Entity 
{
	Vector2 position;
	Vector2 velocity;
	Shape shape;
	EntityTag tag;

	Entity();
};


Entity::Entity()
	: position((Vector2){0.0f,0.0f}), velocity((Vector2){0.0f,0.0f}), shape(RECT), tag(PLAYER)
{}

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
	char letter = '_';
	if      (entity->tag == BALL)
		letter = 'b';
	else if (entity->tag == PLAYER)
		letter = 'p';

	DrawText(TextFormat("%c", letter), entity->position.x, entity->position.y, 16, WHITE);
}

void PlayerControl(Entity* player)
{
	Vector2 input = {0.0f, 0.0f};
	if (IsKeyDown(KEY_D))
	{
		input.x = 1.0f;
	}
	if (IsKeyDown(KEY_A))
	{
		input.x = -1.0f;
	}
	if (IsKeyDown(KEY_W))
	{
		input.y = -1.0f;
	}
	if (IsKeyDown(KEY_S))
	{
		input.y = 1.0f;
	}

	player->position.x += input.x;
	player->position.y += input.y;
}

// =============================================================================================================

void Init()
{
	Entity* test_entity = new Entity;
	test_entity->position = (Vector2){50.0f, 50.0f};
	test_entity->tag = BALL;
	SpawnEntity(test_entity);

	Entity* player = new Entity;
	player->position = (Vector2){10.0f, 10.0f};
	player->tag = PLAYER;
	SpawnEntity(player);

	for (int i = 0; i < entity_count; i++)
	{
		TraceLog(LOG_INFO, TextFormat("%i", entities[i]->tag));
	}
}

void Update(const RenderTexture2D& frame)
{
	BeginTextureMode(frame);
	{
		ClearBackground(PINK);

		for (int i = 0; i < entity_count; i++)
		{
			if (entities[i]->tag == PLAYER)
			{
				PlayerControl(entities[i]);
			}

			DrawEntity(entities[i]);
		}
	}
	EndTextureMode();
}
