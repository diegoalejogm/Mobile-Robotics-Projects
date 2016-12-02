#pragma once
#include "Configuration.h"
#include <vector>
class PathPlanning {
public:
	static void Init();
	static std::vector<Configuration>& getRobotPath(int robotNumber);
};