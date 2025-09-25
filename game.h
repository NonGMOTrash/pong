#pragma once

#include "raylib.h"

enum Shape {
	RECT,
	ELLIPSE
};

enum EntityID {
	PLAYER,
	BALL
};

struct Entity 
{
	Vector2 position;
	Vector2 velocity;
	Shape shape;
	EntityID id;

	Entity();
};


void Process(RenderTexture2D& frame);
