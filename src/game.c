#include "game.h"

#include "battle.h"
#include "pause.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"

Player player = {0};
Camera2D camera = {0};
Map tileMap = {0};
GameState state = {0};
Image image = {0};
MonTexture froge = {0};

static void MovePlayer(void) {
  // Movement
  float moveSpeedModifier = 1.0f;
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    moveSpeedModifier = 2.0f;
  }

  player.speed = player.baseSpeed * moveSpeedModifier;
  Vector2 moveDirection = {0.0f, 0.0f};

  if (IsKeyDown(KEY_W))
    moveDirection.y -= 1.0f;
  if (IsKeyDown(KEY_S))
    moveDirection.y += 1.0f;
  if (IsKeyDown(KEY_A))
    moveDirection.x -= 1.0f;
  if (IsKeyDown(KEY_D))
    moveDirection.x += 1.0f;

  if (moveDirection.x != 0.0f || moveDirection.y != 0.0f) {
    moveDirection = Vector2Normalize(moveDirection);
  }

  // Calculate new position
  Vector2 newPosition = {player.position.x + moveDirection.x * player.speed,
                         player.position.y + moveDirection.y * player.speed};

  // Only update position if there's no collision
  player.position.x = ClampFloat(newPosition.x, player.size / 2,
                                 tileMap.width * tileMap.tileHeight - player.size / 2);

  player.position.y = ClampFloat(newPosition.y, player.size / 2,
                                 tileMap.height * tileMap.tileWidth - player.size / 2);
  // Camera
  camera.target = player.position;
}

static void HandleInput(void) {
  switch (state) {
  case FREE_ROAM:
    MovePlayer();
    if (IsKeyPressed(KEY_B))
      state = BATTLE_SCENE;
    if (IsKeyPressed(KEY_ESCAPE))
      state = PAUSED;
    break;

  case BATTLE_SCENE:
    if (IsKeyPressed(KEY_B))
      state = FREE_ROAM;
    break;

  case PAUSED:
    if (IsKeyPressed(KEY_ESCAPE))
      state = FREE_ROAM;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
      nextMenuItem(&currentMenuIndex);
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
      prevMenuItem(&currentMenuIndex);
    } else if (IsKeyPressed(KEY_ENTER)) {
      selectMenuItem();
    }
    break;

  case TITLE_SCREEN:
    break;
  }

  // Unconditional inputs
  if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) {
    if (IsKeyPressed(KEY_F4))
      CloseWindow();
  }
}

void InitGame(void) {
  state = FREE_ROAM;
  tileMap = LoadTiled("resources-test/desert.json");

  // Load the one mon I got
  image = LoadImage("resources/froge-front-1.png");
  froge.frontTexture = LoadTextureFromImage(image);
  UnloadImage(image);
  image = LoadImage("resources/froge-back-1.png");
  froge.backTexture = LoadTextureFromImage(image);
  UnloadImage(image);
  froge.scale = 0.4f;
  froge.tint = WHITE;

  player.position = (Vector2){(float)screen.width / 2, (float)screen.height / 2};
  player.baseSpeed = 5.0f;
  player.speed = player.baseSpeed;
  player.size = 30;

  camera.target = player.position;
  camera.offset = (Vector2){screen.width / 2.0f, screen.height / 2.0f};
  camera.zoom = 1.0f;
}

void UpdateGame(void) { HandleInput(); }

void DrawGame(void) {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  switch (state) {
  case FREE_ROAM:
    BeginMode2D(camera);
    DrawTiled(tileMap, 0, 0, WHITE);

    // Draw player
    DrawCircleV(player.position, player.size / 2, RED);

    EndMode2D();
    DrawText("Press B to enter the Battle Scene!", 50, 50, 20, DARKGRAY);
    break;
  case BATTLE_SCENE:
    BattleScene();
    break;
  case PAUSED:
    PauseMenu();
    break;
  case TITLE_SCREEN:
    break;
  }

  EndDrawing();
}
