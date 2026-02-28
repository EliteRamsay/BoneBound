#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include "../project.h"

bool SaveFileExists(int slot);
void SaveGame(int slot);
bool LoadGame(int slot);

#endif