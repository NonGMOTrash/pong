#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <algorithm>
#include <iostream>
#include "main.h"

using namespace std;
 
enum GameState {
	TITLE,
	GAME
};

float MoveValueTowards(const float value, const float to, const float step)
{
	float new_value = value;
	if (value < to)
	{
		new_value += step;
		return Clamp(new_value, value, to);
	}
	else {
		new_value -= step;
		return Clamp(new_value, to, value);
	}
}

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(game_width * 2, game_height * 2, "pong 'v'");
	SetTargetFPS(240);

	RenderTexture2D frame = LoadRenderTexture(game_width, game_height);
	float frame_scale = 0.0f;

	// int gameState = GAME;

	Init();

	while (!WindowShouldClose())
	{
		Update(frame);

		// draw final frame (with letterboxing)
		frame_scale = min((float)GetScreenWidth() / game_width, (float)GetScreenHeight() / game_height);

		BeginDrawing();
		{
			ClearBackground(BLACK);

			DrawTexturePro(
					frame.texture, // texture
					(Rectangle){ // source
						0.0f,
						0.0f,
						(float)frame.texture.width,
						(float)frame.texture.height * -1
					},
					(Rectangle){ // dest
						(GetScreenWidth() - (float)game_width * frame_scale) * 0.5f,
						(GetScreenHeight() - (float)game_height * frame_scale) * 0.5f,
						(float)game_width * frame_scale,
						(float)game_height * frame_scale 
					},
					(Vector2){0,0}, // origin
					0.0f, // rotation
					WHITE // tint
			);
		}
		EndDrawing();

	}

	CloseWindow();

	return 0;
}
