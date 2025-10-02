#include "raylib.h"
#include "game.h"

enum Shape {
	RECT,
	ELLIPSE
};

enum EntityTag {
	BALL,
	PLAYER
};


struct Entity 
{
	Vector2 position;
	Vector2 velocity;
	Shape shape;
	float width;
	float height;
	float rotation;
	Color color;
	EntityTag tag;

	Entity();
};

const int ENTITY_CAP = 16;
int entity_count = 0;
Entity* entities[ENTITY_CAP];

Entity::Entity()
	: position((Vector2){0.0f,0.0f}), velocity((Vector2){0.0f,0.0f}), shape(RECT), width(1.0f), height(1.0f), rotation(1.0f), color(RAYWHITE), tag(PLAYER)
{}

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
				entity->width,
				entity->height,
				entity->color
			);
	}
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
			entities[i]->rotation += 1;
		}
	}
	EndTextureMode();
}
