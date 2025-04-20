#include "core/game_engine.h"

std::vector<std::shared_ptr<Scene>> GameEngine::scenes;
int GameEngine::currSceneIdx = 0;
std::unordered_map<unsigned int, bool> GameEngine::keyPresses;
