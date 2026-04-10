#ifndef UI_H
#define UI_H

#include "constants.h"

#pragma region TYPES

typedef struct UIElement
{
    int positionX;
    int positionY;
    int width;
    int height;
    bool visible;
} UIElement;

#pragma endregion

#pragma region GLOBAL FUNCTIONS
void SetScale(int newScale);
void SetPrompt(char str[MAX_PROMPT_CHARS]);
float getScale(void);
void InitUI(void);
void ToggleUIElement(UIElement *element);
void UpdateUI(void);
void DrawUI(void);

#pragma endregion

#pragma region GLOBALS
static float scale = 1.f;
#pragma endregion

#pragma region TITLEBAR
static UIElement titleBar;
static void InitTitleBar(void);
static void DrawTitleBar(void);

#pragma endregion

#pragma region NAVIGATIONBAR
#pragma endregion

#pragma region OUTPUT
#pragma endregion

#pragma region INPUT
static char prompt[MAX_PROMPT_CHARS] = "\0";
#pragma endregion

#pragma region STATUSBAR
#pragma endregion

#endif