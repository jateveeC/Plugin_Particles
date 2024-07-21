#include "boidgrid.h"
#include "particle.h"

void BoidGrid::SetupGrid(int xsize, int ysize, int zsize)
{
	xCells = (MAPSIZE_X + 1023) / xsize;
	zCells = (MAPSIZE_Z + 1023) / zsize;
	yCells = (MAPSIZE_MAXY - MAPSIZE_MINY + 1023) / ysize;

	if (xCells <= 0)
		xCells = 1;
	if (yCells <= 0)
		yCells = 1;
	if (zCells <= 0)
		zCells = 1;

	gridCells = new GridCell** [xCells];
	for (int i = 0; i < xCells; ++i)
	{
		gridCells[i] = new GridCell* [yCells];

		for (int j = 0; j < yCells; ++j)
			gridCells[i][j] = new GridCell [zCells];
	}
}


void BoidGrid::DeconstructGrid()
{
	if (instance)
	{
		for (int i = 0; i < instance->xCells; ++i)
		{
			for (int j = 0; j < instance->yCells; ++j)
				delete[] instance->gridCells[i][j];

			delete[] instance->gridCells[i];
		}

		delete[] instance->gridCells;

		delete instance;
	}
}


void BoidGrid::NewInstance(int x, int y, int z)
{
	if (!instance)
	{
		instance = new BoidGrid(x, y, z);
	}
}


const BoidGrid* BoidGrid::GetInstance()
{
	return instance;
}


void BoidGrid::UpdateCells()
{
	
}
