#include "raylib.h"
#include "constants.h"
#include "screens.h"

static const float CRT_TURNON_SPEED = 2.5f; // screen-widths/heights per second

static bool isFinished;
static int frames = 0;
static Rectangle background = {0};
static Vector2 backgroundStartSize = {1.f, 4.f};
static float showLogoTime = 0.f;

enum bgAnimationState
{
    INIT = 0,
    HORIZONTAL,
    VERTICAL,
    DONE
};
static enum bgAnimationState state = INIT;

void InitLogo(void)
{
    isFinished = false;
    frames = 0;
    state = INIT;
    background.width = backgroundStartSize.x;
    background.height = backgroundStartSize.y;
    background.x = (float)screenWidth / 2.f - background.width / 2.f;
    background.y = (float)screenHeight / 2.f - background.height / 2.f;
    bgColor = BLACK;
    return;
}
void UpdateLogo(float deltaTime)
{
    frames++;
    bgColor = BLACK;
    switch (state)
    {
    case INIT:
        state = HORIZONTAL;
        break;
    case HORIZONTAL:
        background.width += (float)screenWidth * CRT_TURNON_SPEED * deltaTime;
        if (background.width >= (float)screenWidth)
        {
            background.width = (float)screenWidth;
            state = VERTICAL;
        }
        background.x = (float)screenWidth / 2.f - background.width / 2.f;
        break;
    case VERTICAL:
        background.height += (float)screenHeight * CRT_TURNON_SPEED * deltaTime;
        if (background.height >= (float)screenHeight)
        {
            background.height = (float)screenHeight;
            state = DONE;
        }
        background.y = (float)screenHeight / 2.f - background.height / 2.f;
        break;
    case DONE:
        bgColor = palette == 1 ? BGCOLOR_A : BGCOLOR_G;
        showLogoTime += deltaTime;
        break;
    }

    isFinished = (showLogoTime >= 2.f);

    return;
}
void DrawLogo(void)
{
    const char sime[] = "el_sime";
    Color rectColor = state == DONE ? bgColor : fgColor;
    DrawRectangleRec(background, rectColor);
    if (state == DONE)
        DrawText(sime, GetScreenWidth() / 2 - MeasureText(sime, 36) / 2, GetScreenHeight() / 2 - 36 / 2, 36, fgColor);
    return;
}
void CleanupLogo(void)
{
    return;
}

GameScreen EndLogo(void)
{
    return isFinished ? GAMEPLAY : LOGO;
}