#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "constants.h"

#include "screens.h"

//----------------------------------------------
// Functions
//----------------------------------------------

// void SetPrompt(const char[MAX_PR])

void Init();
void Update();
void Draw();
void Cleanup();

static void InitShader(void);
static void SwitchScreen();
static void InitScreen(GameScreen screen);
static void CleanupScreen(GameScreen screen);
//----------------------------------------------
// Globals
//----------------------------------------------

Font font = {0};

RenderTexture2D renderTarget = {0};

int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;
bool isRunning = false;
int palette = 0;
int frameCount = 0;

Color fgColor = {0};
Color bgColor = {0};

currentScreen = LOGO;

static Shader crtShader = {0};
static int crtOffsetLoc = 0;
static bool crtEnabled = true;

int main(int agrc, char *argv[])
{
	Init();

	while (isRunning)
	{
		Update();

		Draw();
	}
	Cleanup();

	return 0;
}

void Init()
{
	InitWindow(screenWidth, screenHeight, "MB Remote maintenance terminal");
	SetExitKey(KEY_NULL);
	SetTargetFPS(TARGET_FPS);
	font = LoadFontEx("assets/cmunss.ttf", FONT_SIZE, 0, 250);
	renderTarget = LoadRenderTexture(screenWidth, screenHeight);

	isRunning = true;
	switch (palette)
	{
	case 1:
		bgColor = BGCOLOR_A;
		fgColor = FGCOLOR_A;
		break;
	default:
		bgColor = BGCOLOR_G;
		fgColor = FGCOLOR_G;
	}
	SetTextLineSpacing(LINES_SPACING);
	InitShader();
	InitScreen(currentScreen);
}

void Update()
{
	isRunning = !WindowShouldClose();
	float deltaTime = GetFrameTime();
	switch (palette)
	{
	case 1:
		bgColor = BGCOLOR_A;
		fgColor = FGCOLOR_A;
		break;
	default:
		bgColor = BGCOLOR_G;
		fgColor = FGCOLOR_G;
	}

	switch (currentScreen)
	{
	case LOGO:
		UpdateLogo(deltaTime);
		break;
	case GAMEPLAY:
		UpdateGameplay(deltaTime);
		break;
	}

	SwitchScreen();
	frameCount++;
	if (frameCount == TARGET_FPS)
		frameCount = 0;

	if (IsKeyPressed(KEY_F1))
		crtEnabled = !crtEnabled;
	if (IsKeyPressed(KEY_F2))
		palette = palette == 1 ? 0 : 1;
}

void Draw()
{
	// Render scene to offscreen texture
	BeginTextureMode(renderTarget);
	ClearBackground(bgColor);
	switch (currentScreen)
	{
	case LOGO:
		DrawLogo();
		break;
	case GAMEPLAY:
		DrawGameplay();
		break;
	}

	EndTextureMode();

	// Update scrolling offset uniform and apply CRT shader
	float offsetPx = fmodf(GetTime() * SCREEN_ANIMATION_SPEED, (SCREEN_LINES_HEIGHT * 2.f));
	SetShaderValue(crtShader, crtOffsetLoc, &offsetPx, SHADER_UNIFORM_FLOAT);

	BeginDrawing();
	ClearBackground(BLACK);
	if (crtEnabled)
	{
		BeginShaderMode(crtShader);
		DrawTextureRec(renderTarget.texture,
					   (Rectangle){0, 0, (float)screenWidth, -(float)screenHeight},
					   (Vector2){0, 0}, WHITE);
		EndShaderMode();
	}
	else
	{
		DrawTextureRec(renderTarget.texture,
					   (Rectangle){0, 0, (float)screenWidth, -(float)screenHeight},
					   (Vector2){0, 0}, WHITE);
	}
	EndDrawing();
}

void Cleanup()
{
	CleanupScreen(currentScreen);

	UnloadFont(font);
	UnloadRenderTexture(renderTarget);
	UnloadShader(crtShader);
	CloseWindow();
}

static void InitShader()
{

	crtShader = LoadShader(0, "assets/crt.fs");
	crtOffsetLoc = GetShaderLocation(crtShader, "offset");
	float sh = (float)screenHeight;
	float lh = (float)SCREEN_LINES_HEIGHT;
	float la = CRT_SCANLINE_ALPHA;
	float curv = CRT_CURVATURE;
	float vig = CRT_VIGNETTE;
	SetShaderValue(crtShader, GetShaderLocation(crtShader, "screenHeight"), &sh, SHADER_UNIFORM_FLOAT);
	SetShaderValue(crtShader, GetShaderLocation(crtShader, "lineHeight"), &lh, SHADER_UNIFORM_FLOAT);
	SetShaderValue(crtShader, GetShaderLocation(crtShader, "lineAlpha"), &la, SHADER_UNIFORM_FLOAT);
	SetShaderValue(crtShader, GetShaderLocation(crtShader, "curvature"), &curv, SHADER_UNIFORM_FLOAT);
	SetShaderValue(crtShader, GetShaderLocation(crtShader, "vigStrength"), &vig, SHADER_UNIFORM_FLOAT);
}

static void SwitchScreen()
{
	GameScreen nextScreen;
	switch (currentScreen)
	{
	case LOGO:
		nextScreen = EndLogo();
		break;
	case GAMEPLAY:
		nextScreen = EndGameplay();
		break;
	}
	if (nextScreen != currentScreen)
	{
		CleanupScreen(currentScreen);
		InitScreen(nextScreen);
		currentScreen = nextScreen;
	}
}

static void InitScreen(GameScreen screen)
{
	switch (screen)
	{
	case LOGO:
		InitLogo();
		break;
	case GAMEPLAY:
		InitGameplay();
		break;
	}
}

static void CleanupScreen(GameScreen screen)
{
	switch (currentScreen)
	{
	case LOGO:
		CleanupLogo();
		break;
	case GAMEPLAY:
		CleanupGameplay();
		break;
	}
}