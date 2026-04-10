#ifndef MODULES_H
#define MODULES_H

#include "constants.h"

typedef enum Components
{
    POWER,
    NETWORK,
    FS
} ComponentID;

struct Component
{
    ComponentID name;
    int integrity;
};

struct Module
{
    char name[MAX_MODULE_NAME_LENGHT];
};

#endif