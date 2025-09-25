#include "raylib.h"
#include "game.h"

Entity::Entity()
	: position((Vector2){0.0f,0.0f}), velocity((Vector2){0.0f,0.0f}), shape(RECT), id(PLAYER)
{}

Entity entities[16];

void Process(RenderTexture2D& frame)
{
	BeginTextureMode(frame);
	{
		for (int i = 0; i < sizeof(entities) / sizeof(Entity); i++)
		{
			DrawText("h", entities[i].position.x, entities[i].position.y, 16, WHITE);
		}
	}
	EndTextureMode();
}
