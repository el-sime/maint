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

#endif