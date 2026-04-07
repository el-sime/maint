#include "raylib.h"
#include <stdio.h>
#include <string.h>
#define BGCOLOR CLITERAL(Color){8, 20, 8, 255}
#define FGCOLOR CLITERAL(Color){60, 200, 60, 255}

#define MAX_INPUT_CHARS 64
#define MAX_OUTPUT_LINE 64
#define MAX_OUTPUT_LINES 50
#define MAX_OUTPUT_LINE_LENGHT 100
#define FONT_SIZE 18
#define TITLE_FONT_SIZE 22

//----------------------------------------------
// Functions
//----------------------------------------------
void AppendOutput(const char out[]);
void Init();

//----------------------------------------------
// Globals
//----------------------------------------------

static const int screenWidth = 1280;
static const int screenHeight = 800;
static bool isRunning = false;
static char input[MAX_INPUT_CHARS + 1] = "\0";
static char prompt[MAX_INPUT_CHARS + 1] = "\0";
static char output[MAX_OUTPUT_LINE_LENGHT * MAX_OUTPUT_LINES + 1] = "\0";
int titleX = 8;
int titleY = 4;
int outerBorderX = 8;
int outerBorderY = 8 + 4 + TITLE_FONT_SIZE;
int outerBorderWidth = 4;
int innerPadding = 4;

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

void AppendOutput(const char out[])
{
	sprintf(output, "%s\n%s", output, out);
}

// DrawOutput

int main(int agrc, char *argv[])
{
	InitWindow(screenWidth, screenHeight, "MB Remote maintenance terminal");
	SetTargetFPS(60);
	int frameCount = 0;
	isRunning = true;
	strcat(output, "");
	int inputCount = 0;

	int outputRecX = titleX;
	int outputRecY = titleY + TITLE_FONT_SIZE + 4;
	int promptX = outputRecX;

	Vector2 outputSize = MeasureTextEx(GetFontDefault(), output, (float)FONT_SIZE, (float)(FONT_SIZE / 10));

	int promptY = (int)outputSize.y + outputRecY;
	// int outputRecWidth = screenWidth - (outputRecX * 2);
	// int outputRecHeight = screenHeight - (outputRecY * 2);
	// Rectangle outputRectangle = {outputRecX, outputRecY, outputRecWidth, outputRecHeight};
	strcpy(prompt, "mant@nbhost $ ");

	while (isRunning)
	{
		isRunning = !WindowShouldClose();
		int key = GetCharPressed();
		while (key > 0)
		{
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
			inputCount--;
			if (inputCount < 0)
				inputCount = 0;
			input[inputCount] = '\0';
		}

		if (IsKeyPressed(KEY_ENTER))
		{

			strcat(output, "\n");
			strcat(output, input);

			ParseCommand(input);

			inputCount = 0;
			input[inputCount] = '\0';
		}
		outputSize = MeasureTextEx(GetFontDefault(), output, (float)FONT_SIZE, (float)(FONT_SIZE / 10));
		promptY = (int)outputSize.y + outputRecY;

		frameCount++;
		if (frameCount == 60)
			frameCount = 0;

		BeginDrawing();
		ClearBackground(BGCOLOR);
		DrawRectangle(titleX, titleY, screenWidth - titleX * 2, TITLE_FONT_SIZE + 4, FGCOLOR);
		DrawText("MB Remote Maintenance Terminal", titleX + 2, titleY + 2, TITLE_FONT_SIZE, BGCOLOR);
		// DrawRectangle(outerBorderX, outerBorderY, screenWidth - (outerBorderX * 2), screenHeight - outerBorderY - 8, FGCOLOR);
		// DrawRectangle(outerBorderX + outerBorderWidth, outerBorderY + outerBorderWidth, screenWidth - (outerBorderX + outerBorderWidth) * 2, screenHeight - outerBorderY - 8 - outerBorderWidth, BGCOLOR);
		DrawText(output, outputRecX, outputRecY, FONT_SIZE, FGCOLOR);
		DrawText(prompt, promptX, promptY, FONT_SIZE, FGCOLOR);
		DrawText(input, promptX + MeasureText(prompt, FONT_SIZE), promptY, FONT_SIZE, FGCOLOR);

		if (((frameCount / 20) % 2) == 0)
			DrawText("_", promptX + MeasureText(input, FONT_SIZE) + MeasureText(prompt, FONT_SIZE) + 2, promptY, FONT_SIZE, FGCOLOR);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
