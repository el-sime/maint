#include "raylib.h"
#include "constants.h"
#include "screens.h"

typedef enum Context
{
    TERM = 0,
    GATEWAY,
    AUXPOWER
} Context;

typedef enum Mode
{
    CLI = 0,
    READER,
    MESSAGES,
    FILES
} Mode;

//----------------------------------------------
// Module variables
//----------------------------------------------
static Context currentContext = TERM;
static Context previousContext = TERM;
static char input[MAX_INPUT_CHARS + 1] = "\0";
static char prompt[MAX_INPUT_CHARS + 1] = "\0";
static char output[MAX_OUTPUT_LINE_LENGHT * MAX_OUTPUT_LINES + 1] = "\0";

static int titleRecHeight; // = TITLE_FONT_SIZE + INTERN_PADDING * 2;
static int titleRecWidth;  // = screenWidth - GLOBAL_PADDING * 2;
static int outputRecX;     // = GLOBAL_PADDING;
static int outputRecY;     // = GLOBAL_PADDING + titleRecHeight + INTERN_PADDING;
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

static const char user[] = "maint";

static char host[MAX_HOST_NAME] = "\0";

//----------------------------------------------
// Module functions
//----------------------------------------------
static void AppendOutput(const char out[]);
static void DrawOutputLines();
static void DrawScreenOverlay();
static void DrawTextTerm(const char *text, int posX, int posY, int fontSize, Color color);
static int MeasureTextTerm(const char *text, int fontSize);
static int LineHeight(int fontSize);
static void ParseCommand(char input[MAX_INPUT_CHARS + 1]);
static int CountLines(const char *s);

void SetStatus(const char *text);

void InitGameplay(void)
{
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
}
void UpdateGameplay(float deltaTime)
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

    strcpy(statusText, TextFormat("NOT CONNECTED"));

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
}
void DrawGameplay(void)
{
    DrawRectangle(GLOBAL_PADDING, GLOBAL_PADDING, titleRecWidth, titleRecHeight, fgColor);
    DrawTextTerm("MB Remote Maintenance Terminal", GLOBAL_PADDING + INTERN_PADDING, GLOBAL_PADDING + INTERN_PADDING, TITLE_FONT_SIZE, bgColor);
    DrawTextTerm(output, outputRecX, outputRecY, FONT_SIZE, fgColor);
    DrawTextTerm(prompt, promptX, promptY, FONT_SIZE, fgColor);
    DrawTextTerm(input, promptX + MeasureTextTerm(prompt, FONT_SIZE), promptY, FONT_SIZE, fgColor);
    if (((frameCount / 20) % 2) == 0)
        DrawTextTerm("_", promptX + MeasureTextTerm(input, FONT_SIZE) + MeasureTextTerm(prompt, FONT_SIZE) + 2, promptY, FONT_SIZE, fgColor);

    DrawRectangle(statusBarX, statusBarY, statusBarWidth, statusBarHeight, fgColor);
    DrawTextTerm(statusText, statusBarX + INTERN_PADDING, statusBarY + INTERN_PADDING, FONT_SIZE, bgColor);
}
void CleanupGameplay(void) {}
GameScreen EndGameplay()
{
    return GAMEPLAY;
}
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