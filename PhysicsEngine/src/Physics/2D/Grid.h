#pragma once

#include <vector>

class GridCell
{
public:
	GridCell() = default;
	~GridCell() = default;

	std::vector<int> objects;
};