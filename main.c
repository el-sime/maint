#include "raylib.h"
#include <stdio.h>
#include <string.h>

#include "constants.h"

// This work uses "Monostilus" by Hsyenoh05 https://opengameart.org/content/monostilus

//----------------------------------------------
// Functions
//----------------------------------------------
void AppendOutput(const char out[]);
void Init();
void Update();
void Draw();

void DrawTextTerm(const char *text, int posX, int posY, int fontSize, Color color);
int MeasureTextTerm(const char *text, int fontSize);
void ParseCommand(char input[MAX_INPUT_CHARS + 1]);
int CountLines(const char *s);

//----------------------------------------------
// Globals
//----------------------------------------------

static const int screenWidth = 1280;
static const int screenHeight = 800;
static bool isRunning = false;
static char input[MAX_INPUT_CHARS + 1] = "\0";
static char prompt[MAX_INPUT_CHARS + 1] = "\0";
static char output[MAX_OUTPUT_LINE_LENGHT * MAX_OUTPUT_LINES + 1] = "\0";

static int titleRecHeight; // = TITLE_FONT_SIZE + INTERN_PADDING * 2;
static int titleRecWidth;  // = screenWidth - GLOBAL_PADDING * 2;
static int outputRecX;     // = GLOBAL_PADDING;
static int outputRecY;     // = GLOBAL_PADDING + titleRecHeight + INTERN_PADDING;
static int maxOutputLines;

static int promptX;
static int promptY;

static int statusBarX;
static int statusBarY;
static int statusBarWidth;
static int statusBarHeight;

static Vector2 outputSize;

static int inputCount = 0;

int frameCount = 0;

Font font = {0};

void AppendOutput(const char out[])
{
    if (strlen(output) > 0)
    {
        printf("#%s#\n", output);
        strcat(output, "\n");
    }
    strcat(output, out);
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
    outputRecX = GLOBAL_PADDING;
    outputRecY = GLOBAL_PADDING + titleRecHeight + INTERN_PADDING;
    promptX = GLOBAL_PADDING;
    outputSize = MeasureTextEx(font, output, (float)FONT_SIZE, (float)(FONT_SIZE / 10));
    promptY = (int)outputSize.y + outputRecY + LINES_SPACING;

    statusBarWidth = screenWidth - GLOBAL_PADDING * 2;
    statusBarHeight = FONT_SIZE + INTERN_PADDING * 2;
    statusBarX = GLOBAL_PADDING;
    statusBarY = screenHeight - GLOBAL_PADDING - statusBarHeight;
    isRunning = true;

    int maxOutputHeight = screenHeight - titleRecHeight - statusBarHeight - (FONT_SIZE);

    strcpy(prompt, "mant@nbhost $ ");
}

void Update()
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

    frameCount++;
    if (frameCount == TARGET_FPS)
        frameCount = 0;
}

void Draw()
{

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawRectangle(GLOBAL_PADDING, GLOBAL_PADDING, titleRecWidth, titleRecHeight, FGCOLOR);
    DrawTextTerm("MB Remote Maintenance Terminal", GLOBAL_PADDING + INTERN_PADDING, GLOBAL_PADDING + INTERN_PADDING, TITLE_FONT_SIZE, BGCOLOR);
    DrawTextTerm(output, outputRecX, outputRecY, FONT_SIZE, FGCOLOR);
    DrawTextTerm(prompt, promptX, promptY, FONT_SIZE, FGCOLOR);
    DrawTextTerm(input, promptX + MeasureTextTerm(prompt, FONT_SIZE), promptY, FONT_SIZE, FGCOLOR);
    if (((frameCount / 20) % 2) == 0)
        DrawTextTerm("_", promptX + MeasureTextTerm(input, FONT_SIZE) + MeasureTextTerm(prompt, FONT_SIZE) + 2, promptY, FONT_SIZE, FGCOLOR);

    DrawRectangle(statusBarX, statusBarY, statusBarWidth, statusBarHeight, FGCOLOR);
    DrawTextTerm("STATUS", statusBarX + INTERN_PADDING, statusBarY + INTERN_PADDING, FONT_SIZE, BGCOLOR);
    EndDrawing();
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