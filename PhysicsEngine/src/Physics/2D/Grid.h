#pragma once

#include <vector>

class GridCell
{
public:
	GridCell() = default;
	~GridCell() = default;

	std::vector<int> objects;
};


class Grid
{
public:
	Grid() = default;
	~Grid() = default;

	void SetGridSize(int width, int height);
	void FlushGrid();

	inline int GetWidth() const { return m_InternalGrid[0].size(); }
	inline int GetHeight() const { return m_InternalGrid.size(); }

	std::vector<GridCell>& operator[](int i)
	{
		return m_InternalGrid[i];
	}
	const std::vector<GridCell>& operator[](int i) const
	{
		return m_InternalGrid[i];
	}

private:
	std::vector<std::vector<GridCell>> m_InternalGrid;
};