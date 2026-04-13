#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen
{
    UNKNOWN = -1,
    LOGO = 0,
    TITLE,
    OPTIONS,
    GAMEPLAY,
    ENDING
} GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxCoin;

extern int screenWidth;
extern int screenHeight;
extern bool isRunning;

extern int palette;   // TODO (maybe?) enum
extern Color fgColor; // = FGCOLOR_A;
extern Color bgColor; // = FGCOLOR_A;

extern int frameCount;

//----------------------------------------------------------------------------------
// LOGO
//----------------------------------------------------------------------------------
void InitLogo(void);
void UpdateLogo(float deltaTime);
void DrawLogo(void);
void CleanupLogo(void);
GameScreen EndLogo(void);

//----------------------------------------------------------------------------------
// GAMEPLAY (Terminal)
//----------------------------------------------------------------------------------
void InitGameplay(void);
void UpdateGameplay(float deltaTime);
void DrawGameplay(void);
void CleanupGameplay(void);
GameScreen EndGameplay(void);
#endif