// TODO
// maybe rewrite velocity to be in pixels / sec
// and acceleration is pixels/s^2

#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include "game.h"

using namespace std;

const int GAME_WIDTH = 384;
const int GAME_HEIGHT = 216;

const Vector2 VECTOR2_CENTER = (Vector2){GAME_WIDTH / 2, GAME_HEIGHT / 2};
const float STATIC_FRICTION_THRESHOLD = 0.03f;

enum Shape {
	NONE = -1,
	ELLIPSE,
	RECT
};

enum EntityTag {
	BALL,
	PLAYER
};

struct Entity
{
	const EntityTag tag = BALL;
	const float top_speed;
	const float acceleration;
	const Shape shape;
	const Vector2 size;
	const bool is_solid; // has collision
	const Color color;
	const float static_friction;
	const float kinetic_friction; 

	Vector2 position = VECTOR2_CENTER;
	Vector2 velocity = Vector2Zero();
	Vector2 input = Vector2Zero();
	float rotation = 0;
	Vector2 scale = Vector2One();

	Entity() = delete; // disallows default constructor
	// this is to force the mega constructor below to be used
	// TODO: is this actually necessary?

	// ctor only sets values
	Entity(
			EntityTag new_tag = BALL,
			float new_top_speed = 0.01f,
			float new_acceleration = 0.01f,
			Shape new_shape = ELLIPSE,
			Vector2 new_size = Vector2One(),
			bool new_is_solid = false,
			Color new_color = BLACK,
			float new_static_friction = 0.1f,
			float new_kinetic_friction = 0.2f
		)
		: tag(new_tag)
		, top_speed(new_top_speed)
		, acceleration(new_acceleration)
		, shape(new_shape)
		, size(new_size)
		, is_solid(new_is_solid)
		, color(new_color)
		, static_friction(new_static_friction)
		, kinetic_friction(new_kinetic_friction)
	{}
};

struct Player : public Entity
{
	const float DASH_COOLDOWN_DURATION = 1.0f;
	const float dash_strength = 1500;
	float dash_cooldown = 0.0f;

	Player()
		: Entity(
				PLAYER, // TAG
				10.0f, // TOP_SPEED 
				100.0f, // ACCELERATION
				ELLIPSE, // SHAPE
				(Vector2){8, 8}, // SIZE
				true, // IS_SOLID
				ORANGE, // COLOR
				1.0f, // STATIC_FRICTION
				0.5f // KINETIC_FRICTION
			)
	{}
};

const int ENTITY_CAP = 64;
int entity_count = 0;
Entity* entities[ENTITY_CAP];

void SpawnEntity(Entity* ent, Vector2 spawn_pos = Vector2Zero(), Vector2 initial_velocity = Vector2Zero())
{
	ent->position = spawn_pos;
	ent->velocity = initial_velocity;

	entity_count++;
	entities[entity_count-1] = ent;
}

void DrawEntity(Entity* ent)
{
	if (ent->shape == NONE)
	{
		return;
	}
	else if (ent->shape == RECT)
	{
		DrawRectanglePro(
				(Rectangle){ent->position.x, ent->position.y, ent->size.x, ent->size.y},
				(Vector2){0.0f, 0.0f},
				ent->rotation,
				ent->color
			);
	}
	else if (ent->shape == ELLIPSE)
	{
		DrawEllipse(
				ent->position.x,
				ent->position.y,
				ent->size.x * ent->scale.x,
				ent->size.y * ent->scale.y,
				ent->color
			);
	}
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
	ent->position += ent->velocity * GetFrameTime() * 60.0f;
}

void PlayerControl(Player* player)
{
	// # setting input vector

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

	player->input = Vector2Normalize(player->input);

	// # dashing

	player->dash_cooldown -= GetFrameTime();

	if (IsKeyDown(KEY_SPACE) and player->input != Vector2Zero() and player->dash_cooldown <= 0)
	{
		player->velocity = player->input * player->dash_strength;
		player->dash_cooldown = player->DASH_COOLDOWN_DURATION;
	}
}

// =============================================================================================================

void Init()
{
	SpawnEntity(new Player, VECTOR2_CENTER);
}

void Update(const RenderTexture2D& frame)
{
	// # game logic

	// ## movement

	for (int i = 0; i < entity_count; i++)
	{
		Entity*& ent = entities[i];

		if (ent->tag == PLAYER)
		{
			PlayerControl( static_cast<Player*>(ent) );
		}

		MoveEntity(ent);
	}

	// ## collisions

	for (int i = 0; i < entity_count; i++)
	{
		Entity*& ent = entities[i];
		if (!ent->is_solid)
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

			// TODO: assuming spheres (wrong)
			if (CheckCollisionCircles(ent->position, ent->size.x, otherEnt->position, otherEnt->size.x))
			{
				// collision!
			}
		}
	}

	// # rendering

	BeginTextureMode(frame);
	{
		ClearBackground(PINK);

		// ## drawing entities
		for (int i = 0; i < entity_count; i++)
		{
			Entity*& ent = entities[i];
			DrawEntity(ent);
		}
	}
	EndTextureMode();
}
