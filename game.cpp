#include "raylib.h"
#include "raymath.h"
#include "main.h"
#include <iostream>

using namespace std;

const Vector2 center_position = (Vector2){(float)game_width / 2, (float)game_height / 2};
const float static_friction_threshold = 0.03f;

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
	const EntityTag tag;
	float top_speed; // in px/sec 
	float acceleration; // in px/sec^2
	Shape shape;
	Vector2 size;
	bool is_solid; // has collision
	Color tint;
	float static_friction;
	float kinetic_friction; 
	float bounciness; // proportion of other object's velocity that's converted into a collision force

	Vector2 position = center_position;
	Vector2 velocity = Vector2Zeros;
	Vector2 input = Vector2Zeros;
	float rotation = 0;
	Vector2 scale = Vector2One();

	Entity() = delete; // disallows default constructor
	// this is to force the mega constructor below to be used
	// TODO: is this actually necessary?

	Entity(
			EntityTag new_tag = BALL,
			float new_top_speed = 100,
			float new_acceleration = 1000,
			Shape new_shape = ELLIPSE,
			Vector2 new_size = Vector2One(),
			bool new_is_solid = false,
			Color new_tint = BLACK,
			float new_static_friction = 1000,
			float new_kinetic_friction = 100,
			float new_bounciness = 0.2f
		)
		: tag(new_tag)
		, top_speed(new_top_speed)
		, acceleration(new_acceleration)
		, shape(new_shape)
		, size(new_size)
		, is_solid(new_is_solid)
		, tint(new_tint)
		, static_friction(new_static_friction)
		, kinetic_friction(new_kinetic_friction)
		, bounciness(new_bounciness)
	{}
};

struct Player : public Entity
{
	const float friction_moving = 300;
	const float friction_still = 1400;
	const float dash_cooldown_duration = 0.3f;
	const float dash_strength = 750;

	float dash_cooldown = 0;

	Player()
		: Entity(
				PLAYER, // TAG
				200, // TOP_SPEED 
				1500, // ACCELERATION
				ELLIPSE, // SHAPE
				(Vector2){8, 8}, // SIZE
				true, // IS_SOLID
				ORANGE, // COLOR
				0, // STATIC_FRICTION
				800, // KINETIC_FRICTION
				0.5f // BOUNCINESS
			)
	{}
};

struct Ball : public Entity
{
	Ball()
		: Entity(
				BALL, // TAG
				2000, // TOP_SPEED 
				0, // ACCELERATION
				ELLIPSE, // SHAPE
				(Vector2){10, 10}, // SIZE
				true, // IS_SOLID
				WHITE, // COLOR
				1000, // STATIC_FRICTION
				100, // KINETIC_FRICTION
				0.9f // BOUNCINESS
			)
	{}
};


static const int ENTITY_CAP = 64;
static int entity_count = 0;
static Entity* entities[ENTITY_CAP];

void SpawnEntity(Entity* ent, Vector2 spawn_pos = Vector2Zeros, Vector2 initial_velocity = Vector2Zeros)
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
				ent->tint
			);
	}
	else if (ent->shape == ELLIPSE)
	{
		DrawEllipse(
				ent->position.x,
				ent->position.y,
				ent->size.x * ent->scale.x,
				ent->size.y * ent->scale.y,
				ent->tint
			);
	}
}

void MoveEntity(Entity* ent)
{
	// apply input acceleration (respects top_speed)
	ent->input = Vector2Normalize(ent->input);
	if (Vector2Length(ent->input) != 0)
	{
		ent->velocity = Vector2MoveTowards(ent->velocity, ent->input * ent->top_speed, ent->acceleration * GetFrameTime());
	}

	// === apply friction ===
	// NOTE: Vector2MoveTowards does not do anything if the target vector is (0,0) so i can't use it here
	float friction_strength = ent->kinetic_friction;
	if (Vector2Length(ent->velocity) < static_friction_threshold)
	{
		friction_strength = ent->static_friction;
	}
	Vector2 friction_vector = Vector2Normalize(ent->velocity) * -1 * friction_strength * GetFrameTime();
	ent->velocity += friction_vector;

	// actually move
	ent->position += ent->velocity * GetFrameTime();

	// do collisions

	if (!ent->is_solid)
	{
		return; // if the entity has no collisions, we just stop here
	}

	Vector2 displacement = Vector2Zeros;

	for (int j = 0; j < entity_count; j++)
	{
		Entity*& otherEnt = entities[j];

		if (otherEnt->position == ent->position)
		{
			// hacky way to prevent an entity from colliding with itself.
			// i'm assuming if they're positions are exactly equal, then they are the same entity
			continue;
		}

		if (ent->shape == ELLIPSE && otherEnt->shape == ELLIPSE)
		{
			// TODO: assuming perfect circles (so size.x == size.y)
			if (CheckCollisionCircles(ent->position, ent->size.x, otherEnt->position, otherEnt->size.x))
			{
				float total_width = ent->size.x + otherEnt->size.y; // sum of the two entities radii
				float min_displace_dist = abs(Vector2Distance(ent->position, otherEnt->position) - total_width) + EPSILON; // minimum displacement distance
				// that's the length of the displacement, basically how far the two entities are overlapping. the +EPSILON is a safety measure to ensure
				// that the objects are not still colliding after the collision for the next frame.
				Vector2 dir = Vector2Normalize(ent->position - otherEnt->position);
				displacement += dir * min_displace_dist;

				ent->velocity += dir * ( ent->bounciness ) * Vector2Length(otherEnt->velocity); 
				otherEnt->velocity += dir * -1 * ( otherEnt->bounciness ) * Vector2Length(ent->velocity); 
			}
		}
	}

	ent->position += displacement;
}

void WrapEntityPosition(Entity* ent)
{
	ent->position.x = Wrap(ent->position.x, -ent->size.x, game_width + ent->size.x);
	ent->position.y = Wrap(ent->position.y, -ent->size.y, game_height + ent->size.y);
}

void PlayerControl(Player* player)
{
	// setting input vector
	// NOTE: the actual movement happens in MoveEntity

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

	// adjusting friction
	
	if (Vector2Length(player->input) != 0)
	{
		player->kinetic_friction = player->friction_moving;
	}
	else {
		player->kinetic_friction = player->friction_still;
	}

	// dashing

	player->dash_cooldown -= GetFrameTime();

	if (IsKeyDown(KEY_SPACE) and player->input != Vector2Zeros and player->dash_cooldown <= 0)
	{
		player->velocity = player->input * player->dash_strength;
		player->dash_cooldown = player->dash_cooldown_duration;
	}
}

// =============================================================================================================

void Init()
{
	SpawnEntity(new Player, center_position);
	SpawnEntity(new Ball, (Vector2){100, 100});
}

void Update(const RenderTexture2D& frame)
{
	// ========= game logic ==========

	// entity movements
	for (int i = 0; i < entity_count; i++)
	{
		Entity*& ent = entities[i];

		if (ent->tag == PLAYER)
		{
			PlayerControl( static_cast<Player*>(ent) );
		}

		MoveEntity(ent);

		WrapEntityPosition(ent);
	}


	// =========== rendering ===========

	BeginTextureMode(frame);
	{
		ClearBackground(PINK);

		// drawing entities
		for (int i = 0; i < entity_count; i++)
		{
			Entity*& ent = entities[i];
			DrawEntity(ent);
		}

		DrawFPS(3, 3);
	}
	EndTextureMode();
}
