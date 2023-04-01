#include "Grid.h"

void Grid::SetGridSize(int width, int height)
{
	m_InternalGrid = std::vector<std::vector<GridCell>>(height, std::vector<GridCell>(width));
}

void Grid::FlushGrid()
{
	for (auto& v : m_InternalGrid)
		for (GridCell& cell : v)
			cell.objects.clear();
}