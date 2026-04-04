#include "raylib.h"
#define BGCOLOR CLITERAL (Color){8,20,8, 255}
#define FGCOLOR CLITERAL (Color){60,200,60, 255}

const int screenWidth = 1280;
const int screenHeight = 800;

int outerBorder = 8;
int outerBorderWidth = 4;
int innerPadding = 4;



int main (int agrc, char *argv[])
{
	int outputRecX = outerBorder + outerBorderWidth + innerPadding;
	int outputRecY = outerBorder + outerBorderWidth + innerPadding;
	int outputRecWidth = screenWidth - (outputRecX * 2);
	int outputRecHeight = screenHeight - (outputRecY * 2);
	Rectangle outputRectangle = {outputRecX, outputRecY, outputRecWidth, outputRecHeight};
	InitWindow(screenWidth, screenHeight, "MB Remote maintenance terminal");
	SetTargetFPS(60);
	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BGCOLOR);
		DrawRectangle(8,8,screenWidth - 16, screenHeight - 16, FGCOLOR);
		DrawRectangle(12,12,screenWidth - 24, screenHeight - 24, BGCOLOR);
		DrawRectangleLinesEx(outputRectangle, 4, FGCOLOR);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
