#include <string.h>
#include "raylib.h"
#include "ui.h"

void InitUI(void)
{
    return;
}

void SetPrompt(char str[MAX_PROMPT_CHARS])
{
    char hostname[] = "mbhost";
    if (str == "")
    {
        strcpy(str, TextFormat("maint@%s $ ", hostname));
    }
    strcpy(prompt, str);
}

static void InitTitleBar(void)
{
    titleBar.positionX = GLOBAL_PADDING * scale;
    titleBar.positionY = GLOBAL_PADDING * scale;
    titleBar.width = (SCREEN_WIDTH - GLOBAL_PADDING * 2) * scale;
    titleBar.height = (TITLE_FONT_SIZE + INTERN_PADDING * 2) * scale;
    titleBar.visible = true;
}

static void DrawTitleBar(void)
{
    DrawRectangle(titleBar.positionX, titleBar.positionY, titleBar.width, titleBar.height, FGCOLOR_A);
}