#include <iostream>
#include <algorithm>
#include "raylib.h"

using std::cout;
using std::endl;
using std::min;
using std::max;

const int GAME_WIDTH = 384;
const int GAME_HEIGHT = 216;

struct Entity 
{

};

int main()
{
	int window_width = GAME_WIDTH * 2;
	int window_height = GAME_WIDTH * 2;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(window_width, window_height, "pong 'v'");
	SetTargetFPS(60);

	RenderTexture2D frame = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
	float frameScale = 0.0f;

	while (!WindowShouldClose())
	{
		// draw frame
		BeginTextureMode(frame);
		{
			ClearBackground(BLACK);

			DrawText("hi", 50, 50, 50, BLUE);
		}
		EndTextureMode();

		// draw final frame (with letterboxing)
		window_width = GetScreenWidth();
		window_height = GetScreenHeight();
		frameScale = min((float)window_width / GAME_WIDTH, (float)window_height / GAME_HEIGHT);

		BeginDrawing();
		{
			ClearBackground(PINK);
			DrawText("hi", 50, 50, 50, RED);

			DrawTexturePro(
					frame.texture, // texture
					(Rectangle){ // source
						0.0f,
						0.0f,
						(float)frame.texture.width,
						(float)frame.texture.height * -1
					},
					(Rectangle){ // dest
						(GetScreenWidth() - (float)GAME_WIDTH * frameScale) * 0.5f,
						(GetScreenHeight() - (float)GAME_HEIGHT * frameScale) * 0.5f,
						(float)GAME_WIDTH * frameScale,
						(float)GAME_HEIGHT * frameScale 
					},
					(Vector2){0,0}, // origin
					0.0f, // rotation
					WHITE // tint
			);

			/* the model */
			// DrawTexturePro(
			// 	render_texture.texture,
			// 	(Rectangle){
			// 		0.0f,
			// 		0.0f,
			// 		(float)render_texture.texture.width,
			// 		(float)-render_texture.texture.height
			// 	},
			// 	(Rectangle){
			// 		(GetScreenWidth() - ((float)viewport_width*scale))*0.5f,
			// 		(GetScreenHeight() - ((float)viewport_height*scale))*0.5f,
			// 		(float)viewport_width*scale,
			// 		(float)viewport_height*scale
			// 	},
			// 	(Vector2){ 0, 0 }, 0.0f, WHITE
			// );
		}
		EndDrawing();
	}

	CloseWindow();
}
