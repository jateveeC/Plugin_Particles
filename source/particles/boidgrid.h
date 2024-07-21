#pragma once

#define MAPSIZE_X 1024 * 100
#define MAPSIZE_Z 1024 * 100
#define MAPSIZE_MINY -128 * 256
#define MAPSIZE_MAXY 127 * 256

struct BaseParticle;

struct GridCell
{
	int count;
	BaseParticle* boids;
};

class BoidGrid
{
public:
	static void NewInstance(int xsize, int ysize, int zsize);
	static const BoidGrid* GetInstance();

	void SetupGrid(int xsize, int ysize, int zsize);
	void DeconstructGrid();

	void UpdateCells();

private:

	static BoidGrid* instance;

	GridCell*** gridCells = nullptr;
	int xCells = 0, yCells = 0, zCells = 0;

	BoidGrid(int xsize, int ysize, int zsize)
	{
		SetupGrid(xsize, ysize, zsize);
	}
	
	~BoidGrid()
	{
		DeconstructGrid();
	}
};