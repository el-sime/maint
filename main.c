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

void AppendOutput(const char out[]);
void Init();
void Update();
void Draw();

void DrawOutputLines();
void DrawScreenOverlay();
void DrawTextTerm(const char *text, int posX, int posY, int fontSize, Color color);
int MeasureTextTerm(const char *text, int fontSize);
int LineHeight(int fontSize);
void ParseCommand(char input[MAX_INPUT_CHARS + 1]);
int CountLines(const char *s);

void SetStatus(const char *text);

//----------------------------------------------
// Globals
//----------------------------------------------
typedef enum Context
{
	TERM = 0,
	READ
} Context;

const char user[] = "maint";
char host[MAX_HOST_NAME] = "\0";

static Context currentContext = TERM;
static Context previousContext = TERM;
static int screenWidth = SCREEN_WIDTH;
static int screenHeight = SCREEN_HEIGHT;

static bool isRunning = false;
static char input[MAX_INPUT_CHARS + 1] = "\0";
static char prompt[MAX_INPUT_CHARS + 1] = "\0";
static char output[MAX_OUTPUT_LINE_LENGHT * MAX_OUTPUT_LINES + 1] = "\0";

static int titleRecHeight; // = TITLE_FONT_SIZE + INTERN_PADDING * 2;
static int titleRecWidth;  // = screenWidth - GLOBAL_PADDING * 2;
static int outputRecX;	   // = GLOBAL_PADDING;
static int outputRecY;	   // = GLOBAL_PADDING + titleRecHeight + INTERN_PADDING;
static int maxOutputLines;
static int maxOutputHeight;

static int modulesBarWidth;
static int modulesBarHeight;

static int outputStartLine = 0;
static int outputEndLine = 0;

static int promptX;
static int promptY;

static int statusBarX;
static int statusBarY;
static int statusBarWidth;
static int statusBarHeight;
static char statusText[MAX_STATUS_TEXT] = "\0";

static Vector2 outputSize;

static int inputCount = 0;

static Color fgColor; // = FGCOLOR_A;
static Color bgColor; // = FGCOLOR_A;

static int palette = 1; // TODO (maybe?) enum

int frameCount = 0;
float inputTimer = 0;
bool animateBackground = false;

Font font = {0};

RenderTexture2D renderTarget = {0};
Shader crtShader = {0};
int crtOffsetLoc = 0;
bool crtEnabled = true;

void AppendOutput(const char out[])
{
	if (strlen(output) > 0)
	{
		strcat(output, "\n");
	}

	strcat(output, out);
	int lines = CountLines(output);
	if (lines > maxOutputLines)
	{
		int n = lines - maxOutputLines;
		char *start = output;
		for (int i = 0; i < n; i++)
		{
			start = strchr(start, '\n');
			if (start)
				start++;
			else
				break;
		}
		if (start)
			memmove(output, start, strlen(start) + 1);
	}
}

// DrawOutput

int main(int agrc, char *argv[])
{
	Init();

	while (isRunning)
	{
		Update();

		Draw();
	}

	UnloadFont(font);
	UnloadRenderTexture(renderTarget);
	UnloadShader(crtShader);
	CloseWindow();
	return 0;
}

void Init()
{
	InitWindow(screenWidth, screenHeight, "MB Remote maintenance terminal");
	SetTargetFPS(TARGET_FPS);
	SetTextLineSpacing(LINES_SPACING);
	font = LoadFontEx("assets/cmunss.ttf", FONT_SIZE, 0, 250);

	titleRecHeight = TITLE_FONT_SIZE + INTERN_PADDING * 2;
	titleRecWidth = screenWidth - GLOBAL_PADDING * 2;
	modulesBarWidth = screenWidth - GLOBAL_PADDING * 2;
	modulesBarHeight = FONT_SIZE + INTERN_PADDING * 2;
	outputRecX = GLOBAL_PADDING;
	outputRecY = GLOBAL_PADDING + titleRecHeight + INTERN_PADDING + modulesBarHeight;
	promptX = GLOBAL_PADDING;
	outputSize = MeasureTextEx(font, output, (float)FONT_SIZE, (float)(FONT_SIZE / 10));
	promptY = (int)outputSize.y + outputRecY + LINES_SPACING;

	statusBarWidth = screenWidth - GLOBAL_PADDING * 2;
	statusBarHeight = FONT_SIZE + INTERN_PADDING * 2;
	statusBarX = GLOBAL_PADDING;
	statusBarY = screenHeight - GLOBAL_PADDING - statusBarHeight;
	isRunning = true;

	maxOutputHeight = screenHeight - outputRecY - statusBarHeight - (FONT_SIZE + INTERN_PADDING * 2);
	maxOutputLines = (int)floor(maxOutputHeight / LineHeight(FONT_SIZE));
	strcpy(statusText, "NOT CONNECTED");
	strcpy(prompt, "mant@mbhost $ ");

	renderTarget = LoadRenderTexture(screenWidth, screenHeight);
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

void Update()
{
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
	inputTimer += GetFrameTime();
	strcpy(statusText, TextFormat("NOT CONNECTED"));
	isRunning = !WindowShouldClose();
	int key = GetCharPressed();
	while (key > 0)
	{
		inputTimer = 0.f;
		animateBackground = false;
		if ((key >= 32) && (key <= 125) && (inputCount < MAX_INPUT_CHARS))
		{
			input[inputCount] = (char)key;
			input[inputCount + 1] = '\0';
			inputCount++;
		}
		else
		{
			printf("key pressed%c\n", (char)(key));
		}

		key = GetCharPressed();
	}

	if (IsKeyPressed(KEY_BACKSPACE))
	{
		inputTimer = 0.f;
		animateBackground = false;
		inputCount--;
		if (inputCount < 0)
			inputCount = 0;
		input[inputCount] = '\0';
	}

	if (IsKeyPressed(KEY_ENTER))
	{
		inputTimer = 0.f;
		animateBackground = false;
		char cmd[MAX_OUTPUT_LINE_LENGHT + 1] = "\0";
		strcat(cmd, prompt);
		strcat(cmd, input);
		AppendOutput(cmd);
		ParseCommand(input);

		inputCount = 0;
		input[inputCount] = '\0';
	}
	outputSize = MeasureTextEx(font, output, (float)FONT_SIZE, (float)(FONT_SIZE / 10));
	promptY = (int)outputSize.y + outputRecY + LINES_SPACING;
	if (inputTimer >= SCREEN_ANIMATION_TRIGGER || SCREEN_ANIMATION_TRIGGER == 0)
	{
		animateBackground = true;
	}
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

	DrawRectangle(GLOBAL_PADDING, GLOBAL_PADDING, titleRecWidth, titleRecHeight, fgColor);
	DrawTextTerm("MB Remote Maintenance Terminal", GLOBAL_PADDING + INTERN_PADDING, GLOBAL_PADDING + INTERN_PADDING, TITLE_FONT_SIZE, bgColor);
	DrawTextTerm(output, outputRecX, outputRecY, FONT_SIZE, fgColor);
	DrawTextTerm(prompt, promptX, promptY, FONT_SIZE, fgColor);
	DrawTextTerm(input, promptX + MeasureTextTerm(prompt, FONT_SIZE), promptY, FONT_SIZE, fgColor);
	if (((frameCount / 20) % 2) == 0)
		DrawTextTerm("_", promptX + MeasureTextTerm(input, FONT_SIZE) + MeasureTextTerm(prompt, FONT_SIZE) + 2, promptY, FONT_SIZE, fgColor);

	DrawRectangle(statusBarX, statusBarY, statusBarWidth, statusBarHeight, fgColor);
	DrawTextTerm(statusText, statusBarX + INTERN_PADDING, statusBarY + INTERN_PADDING, FONT_SIZE, bgColor);

	EndTextureMode();

	// Update scrolling offset uniform and apply CRT shader
	float offsetPx = animateBackground
						 ? fmodf((inputTimer - SCREEN_ANIMATION_TRIGGER) * SCREEN_ANIMATION_SPEED, (SCREEN_LINES_HEIGHT * 2.f))
						 : 0.0f;
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

void DrawScreenOverlay()
{
	float offset = 0;
	if (animateBackground)
	{
		offset = fmodf((inputTimer - SCREEN_ANIMATION_TRIGGER) * SCREEN_ANIMATION_SPEED, (SCREEN_LINES_HEIGHT * 2.f));
	}
	float r = -offset;
	int stripe = 0;
	while (r < screenHeight)
	{
		Color slColor = (stripe % 2 == 0) ? SCREEN_LINES_BGCOLOR : SCREEN_LINES_FGCOLOR;
		DrawRectangleRec((Rectangle){0, r, (float)screenWidth, SCREEN_LINES_HEIGHT}, slColor);
		r += SCREEN_LINES_HEIGHT;
		stripe++;
	}
}

void DrawOutputLines()
{
	char *outputLine = "asdasd\0";
	int lh = LineHeight(FONT_SIZE);
	for (int y = 0; y < maxOutputLines; y++)
	{
		DrawText(outputLine, GLOBAL_PADDING, outputRecY + lh * y, FONT_SIZE, fgColor);
		// DrawLine(1, outputRecY + lh * y, screenWidth - 1, outputRecY + lh * y, LIGHTGRAY);
	}
}

void ParseCommand(char input[MAX_INPUT_CHARS + 1])
{
	if (strlen(input) == 0)
		return;
	char *verb = strtok(input, " \n");
	char *noun = strtok(NULL, " \n");
	char cmdOutput[MAX_OUTPUT_LINE_LENGHT + 1] = "\0";
	if (strcmp(verb, "quit") == 0)
	{
		isRunning = false;
	}
	else if (strcmp(verb, "clear") == 0)
	{
		output[0] = '\0';
	}
	else
	{
		// append out
		sprintf(cmdOutput, "Unknown command: %s", verb);
		AppendOutput(cmdOutput);
	}
}

int CountLines(const char *s)
{
	int count = 1;
	while (*s)
	{
		if (*s++ == '\n')
			count++;
	}
	return count;
}

void DrawTextTerm(const char *text, int posX, int posY, int fontSize, Color color)
{
	// Check if default font has been loaded
	if (font.texture.id != 0)
	{
		Vector2 position = {(float)posX, (float)posY};

		int defaultFontSize = 10; // Default Font chars height in pixel
		if (fontSize < defaultFontSize)
			fontSize = defaultFontSize;
		int spacing = fontSize / defaultFontSize;

		DrawTextEx(font, text, position, fontSize, (float)spacing, color);
	}
}

// Measure string width for default font
int MeasureTextTerm(const char *text, int fontSize)
{
	Vector2 textSize = {0.0f, 0.0f};

	// Check if default font has been loaded
	if (font.texture.id != 0)
	{
		int defaultFontSize = 10; // Default Font chars height in pixel
		if (fontSize < defaultFontSize)
			fontSize = defaultFontSize;
		int spacing = fontSize / defaultFontSize;

		textSize = MeasureTextEx(font, text, (float)fontSize, (float)spacing);
	}

	return (int)textSize.x;
}

int LineHeight(int fontSize)
{
	Vector2 textSize = {0.0f, 0.0f};
	const char *text = "a";
	// Check if default font has been loaded
	if (font.texture.id != 0)
	{
		int defaultFontSize = 10; // Default Font chars height in pixel
		if (fontSize < defaultFontSize)
			fontSize = defaultFontSize;
		int spacing = fontSize / defaultFontSize;

		textSize = MeasureTextEx(font, text, (float)fontSize, (float)spacing);
	}

	return (int)textSize.y + LINES_SPACING;
}