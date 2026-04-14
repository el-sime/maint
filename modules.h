#ifndef MODULES_H
#define MODULES_H
#include <stdbool.h>
#include "constants.h"
//----------------------------------------------------------------------------------
#pragma region Types 
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum InterfaceType
{
    INT_UNKNOWKN = -1,
    CLI = 0,
    MINCLI,
    READ,
    MESS
} InterfaceType;

typedef enum ModuleId
{
    MOD_UNKNOWN = -1,
    TERM = 0,
    GATEWAY,
    AUXPOW,
    DBAY,

} ModuleId;

typedef enum HealthState
{
    OFFLINE = 0,
    REPAIRING,
    ONLINE,
    FUBAR
} HealthState;

typedef enum ComponentId
{
    COMP_UNKNOWN = -1,
    POWER = 0,
    NETWORK,
    FS,
    HELP,
    MANUAL
} ComponentId;

typedef struct Component
{
    ComponentId id;
    const char name[MAX_COMPONENT_NAME];
    int health;
    int repairRate;
    HealthState state;
} Component;

typedef struct Module
{
    ModuleId id;
    const char hostname[MAX_HOST_NAME];
    const char welcome[MAX_OUTPUT_LINES * MAX_OUTPUT_LINE_LENGHT];

} Module;
#pragma endregion
//----------------------------------------------------------------------------------
#pragma region GloVar
// Global Variables  Declaration (shared by all Modules)
//----------------------------------------------------------------------------------
extern ModuleId currentModule;
extern InterfaceType currentInterface;

#pragma endregion

//----------------------------------------------------------------------------------
#pragma region GloFun 
// Global Module Functions
//----------------------------------------------------------------------------------
Module *LoadModule(ModuleId id);
bool ConnectToModule(ModuleId id);

#pragma endregion

#endif