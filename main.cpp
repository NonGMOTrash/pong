#include <string>
#include <algorithm>
#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include "game.h"

using std::min;
using std::max;

enum GameState {
	TITLE,
	GAME
};

const int GAME_WIDTH = 384;
const int GAME_HEIGHT = 216;

// gets proj_v(u) or, the component of v in the direction of u
Vector2 Vector2Project(const Vector2& v, const Vector2& u)
{
	return v * ( Vector2DotProduct(v, u) / Vector2DotProduct(v, v) );
}

int main()
{
	int window_width = GAME_WIDTH * 2;
	int window_height = GAME_WIDTH * 2;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(window_width, window_height, "pong 'v'");
	SetTargetFPS(60);

	RenderTexture2D frame = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
	float frameScale = 0.0f;

	// int gameState = GAME;

	Init();

	while (!WindowShouldClose())
	{
		Update(frame);

		// draw final frame (with letterboxing)
		window_width = GetScreenWidth();
		window_height = GetScreenHeight();
		frameScale = min((float)window_width / GAME_WIDTH, (float)window_height / GAME_HEIGHT);

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
						(window_width - (float)GAME_WIDTH * frameScale) * 0.5f,
						(window_height - (float)GAME_HEIGHT * frameScale) * 0.5f,
						(float)GAME_WIDTH * frameScale,
						(float)GAME_HEIGHT * frameScale 
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
