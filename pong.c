#include <stdio.h>
#include "raylib.h"

#define TENNIS_GREEN (Color){0, 123, 34}
#define BALL_GREEN (Color){204, 255, 0, 255}

#define WWIDTH 800
#define WHEIGHT 450

#define CENTER_X WWIDTH / 2
#define CENTER_Y WHEIGHT / 2

#define BALL_SPEED 3.0

#define PLYR_WIDTH 13
#define PLYR_HEIGHT 80

#define LEFT -1
#define RIGHT 1

struct balls {
	Vector2 position;
	float radius;
	Color color;
	float speed;
	int direction;
	float y_angle;
};

struct hitpoint {
	float x;
	float y_top;
	float y_bottom;
	float y_middle;
};

struct players {
	int points;
	float speed;
	Color color;
	Vector2 position;
	struct hitpoint hitpoints;
};

int GameInProgress(struct players *player1p, struct players *player2p);

void DrawBall(struct balls *ballp);
void MoveBall(struct balls *ballp);
void ResetBall(struct balls *ballp);
int ReverseBall(struct balls *ballp);
float BallHit(struct balls *ballp, struct players *player1p,
		     struct players *player2p);
int BounceBall(struct balls *ballp);
int GoalLeftSide(struct balls *ballp);
int GoalRightSide(struct balls *ballp);

void DrawPlayers(struct players *player1p, struct players *player2p);
void DrawScore(struct players *player1p, struct players *player2p);

void DrawCenterLines(void);
void ControlPlayers(struct players *player1p, struct players *player2p);

void InitBall(struct balls *ballp);
void InitPlayers(struct players *player1p, struct players *player2p,
		 struct balls *ballp);
void UpdateHitboxes(struct players *playerp);

int WallBounce(struct balls *ballp);

void ReverseBallAngle(struct balls *ballp);

int main(void)
{
	/* Initialize game */
	InitWindow(WWIDTH, WHEIGHT, "Tennis Pong");
	SetTargetFPS(60);

	/* Initialize ball */
	struct balls ball;
	InitBall(&ball);

	/* Initialize players */
	struct players player1, player2;
	InitPlayers(&player1, &player2, &ball);

	/* Main game loop */
	while (!WindowShouldClose()) {
		/* Keep ball moving */
		if (GameInProgress(&player1, &player2))
			MoveBall(&ball);

		if (WallBounce(&ball))
			ReverseBallAngle(&ball);

		/* Bounce ball */
		if (BallHit(&ball, &player1, &player2))
			BounceBall(&ball);

		/* Process goals */
		if (GoalLeftSide(&ball)) {
			player2.points++;
			ResetBall(&ball);
		} else if (GoalRightSide(&ball)) {
			player1.points++;
			ResetBall(&ball);
		}

		/* Control players movement */
		ControlPlayers(&player1, &player2);

		/* Draw */
		BeginDrawing();
			ClearBackground(TENNIS_GREEN);
			DrawCenterLines();

			DrawScore(&player1, &player2);
			DrawPlayers(&player1, &player2);
			DrawBall(&ball);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}

int WallBounce(struct balls *ballp)
{
	return (ballp->position.y < ballp->radius) ||
		(ballp->position.y > (WHEIGHT - ballp->radius));
}

void ReverseBallAngle(struct balls *ballp)
{
	ballp->y_angle *= -1;
}

void InitBall(struct balls *ballp)
{
	ballp->radius = 10.0;
	ballp->position.x = CENTER_X;
	ballp->position.y = CENTER_Y;
	ballp->color = BALL_GREEN;
	ballp->speed = BALL_SPEED;
	ballp->direction = RIGHT;
	ballp->y_angle = 0.0;
}

void InitPlayers(struct players *player1p, struct players *player2p,
		 struct balls *ballp)
{
	player1p->points = player2p->points = 0;
	player1p->speed = player2p->speed = 4.0;
	player1p->color = player2p->color = WHITE;

	player1p->position.y = CENTER_Y - (PLYR_HEIGHT / 2);
	player1p->position.x = 0.0;
	player1p->hitpoints.x = PLYR_WIDTH + (ballp->radius / 2);
	player1p->hitpoints.y_top = player1p->position.y;
	player1p->hitpoints.y_bottom = player1p->position.y + PLYR_HEIGHT;

	player2p->position.x = WWIDTH - PLYR_WIDTH;
	player2p->position.y = CENTER_Y - (PLYR_HEIGHT / 2);
	player2p->hitpoints.x = WWIDTH - PLYR_WIDTH - (ballp->radius / 2);
	player2p->hitpoints.y_top = player2p->position.y;
	player2p->hitpoints.y_bottom = player2p->position.y + PLYR_HEIGHT;
}

int GameInProgress(struct players *player1p, struct players *player2p)
{
	return (player1p->points < 5) && (player2p->points < 5);
}

void DrawBall(struct balls *ballp)
{
	DrawCircleV(ballp->position, ballp->radius, ballp->color);
}

void DrawPlayers(struct players *player1p, struct players *player2p)
{
	DrawRectangleV(player1p->position, (Vector2){ PLYR_WIDTH, PLYR_HEIGHT },
		       player1p->color);

	DrawRectangleV(player2p->position, (Vector2){ PLYR_WIDTH, PLYR_HEIGHT },
		       player2p->color);
}

void DrawScore(struct players *player1p, struct players *player2p)
{
	DrawText(TextFormat("Player1: %d", player1p->points),
		 25, 10, 20, WHITE);

	DrawText(TextFormat("Player2: %d", player2p->points),
		 WWIDTH - 128, 10, 20, WHITE);
}

void MoveBall(struct balls *ballp)
{
	ballp->position.x += ballp->speed * ballp->direction;
	ballp->position.y += ballp->y_angle;
}

void ResetBall(struct balls *ballp)
{
	ballp->position.x = CENTER_X;
	ballp->position.y = CENTER_Y;
	ballp->y_angle = 0;
	ballp->speed = BALL_SPEED;
	ReverseBall(ballp);
}

void DrawCenterLines(void)
{
	int nlines = 7;
	int width = 2;
	float height = WHEIGHT / nlines;
	float posx = CENTER_X - (width / 2);
	int posy = 0;
	Color color;

	while (nlines-- > 0) {
		color = (nlines % 2 == 0) ? WHITE : WHITE;

		DrawRectangle(posx, posy, width, height, color);

		posy += height;
	}
}

void ControlPlayers(struct players *player1p, struct players *player2p)
{
	float newPosition;

	if (IsKeyDown(KEY_W)) {
		newPosition = player1p->position.y - player1p->speed;

		if (newPosition >= 0)
			player1p->position.y = newPosition;
	}

	if (IsKeyDown(KEY_S)) {
		newPosition = player1p->position.y + player1p->speed;

		if (newPosition <= (WHEIGHT - PLYR_HEIGHT))
			player1p->position.y = newPosition;

	}

	if (IsKeyDown(KEY_UP)) {
		newPosition = player2p->position.y - player2p->speed;

		if (newPosition >= 0)
			player2p->position.y = newPosition;

	}

	if (IsKeyDown(KEY_DOWN)) {
		newPosition = player2p->position.y + player2p->speed;

		if (newPosition <= (WHEIGHT - PLYR_HEIGHT))
			player2p->position.y = newPosition;

	}
}

int GoalLeftSide(struct balls *ballp)
{
	return ballp->position.x < 0;
}

int GoalRightSide(struct balls *ballp)
{
	return ballp->position.x > WWIDTH;
}

int ReverseBall(struct balls *ballp)
{
	ballp->direction = (ballp->direction == LEFT) ? RIGHT : LEFT;
}

int BounceBall(struct balls *ballp)
{
	ReverseBall(ballp);
	ballp->speed *= 1.05; /* increase ball speed slightly after hit */

	if (ballp->speed > 7.5)
		ballp->speed = 7.5; /* max speed */
}

void UpdateBallAngle(struct balls *ballp, struct players *playerp)
{
	ballp->y_angle = 1 - playerp->hitpoints.y_middle / ballp->position.y;
	ballp->y_angle *= 7;
}

/*
 * BallHit: checks if ball is hit by one of the players
 * On hit sets y_angle and returns 1;
 *
 * Returns 0 if not hit
 */
float BallHit(struct balls *ballp, struct players *player1p,
		     struct players *player2p)
{
	UpdateHitboxes(player1p);
	UpdateHitboxes(player2p);

	if (ballp->position.x < player1p->hitpoints.x)
		if (ballp->position.y >= player1p->hitpoints.y_top &&
		    ballp->position.y <= player1p->hitpoints.y_bottom) {
			UpdateBallAngle(ballp, player1p);
			return 1;
		}
	if (ballp->position.x > player2p->hitpoints.x)
		if (ballp->position.y >= player2p->hitpoints.y_top &&
		    ballp->position.y <= player2p->hitpoints.y_bottom) {
			UpdateBallAngle(ballp, player2p);
			return 1;
		}

	return 0;
}

void UpdateHitboxes(struct players *playerp)
{
	playerp->hitpoints.y_top = playerp->position.y;
	playerp->hitpoints.y_bottom = playerp->position.y + PLYR_HEIGHT;
	playerp->hitpoints.y_middle = playerp->position.y + (PLYR_HEIGHT / 2);
}
