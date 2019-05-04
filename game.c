#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Game of Life Rules
 * 		1. Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 * 		2. Any live cell with two or three live neighbours lives on to the next generation.
 * 		3. Any live cell with more than three live neighbours dies, as if by overcrowding.
 * 		4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */

typedef enum {
	CellState_dead,
	CellState_alive,
} CellState;

typedef struct {
	uint16_t x;
	uint16_t y;
} Coord;

typedef uint16_t ** Grid;

#define LOG(x) printf(x)
//#define LOG(x)

// //// Tests for Cell in CellState_alive
// /**
//  * Rule 1: Any live cell with fewer than two live neighbours dies, as if caused by under-population.
//  * @param grid
//  * @param point
//  * @return Returns @c CellState, whether cell continues to live or dies.
//  */
// CellState underPopulation(char ** grid, Coord * point) {
// 	return Result_live;
// }

// /**
//  * Rule 2: Any live cell with two or three live neighbours lives on to the next generation.
//  * @param grid
//  * @param point
//  * @return Returns @c CellState, whether cell continues to live or dies.
//  */
// CellState canContinueLiving(char ** grid, Coord * point) {
// 	return Result_live;
// }

// *
//  * Rule 3: Any live cell with more than three live neighbours dies, as if by overcrowding.
//  * @param grid
//  * @param point
//  * @return Returns @c CellState, whether cell continues to live or dies.
 
// CellState overCrowding(char ** grid, Coord * point) {
// 	return Result_live;
// }

// //// Tests for Cell in CellState_dead
// /**
//  * Rule 4: Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
//  *
//  */
// CellState reproduce(char ** grid, Coord * point) {
// 	return Result_live;
// }

CellState getCellState(uint16_t * g, uint16_t size, uint16_t x, uint16_t y) {
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	return (grid[x][y] == 1) ? CellState_alive : CellState_dead;
}

uint16_t getNeighborLiveCount(uint16_t * g, uint16_t size, uint16_t x, uint16_t y) {
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	uint16_t liveCount = 0;
	
	if (x == 0) {
		liveCount += grid[x+1][y];
	} else if (x == size-1) {
		liveCount += grid[x-1][y];
	} else {
		liveCount += grid[x+1][y];
		liveCount += grid[x-1][y];
	}
	if (y == 0) {
		liveCount += grid[x][y+1];
	} else if (y == size-1) {
		liveCount += grid[x][y-1];
	} else {
		liveCount += grid[x][y+1];
		liveCount += grid[x][y-1];
	}
	if (x == 0 && y == 0) {
		liveCount += grid[x+1][y+1];
	} else if (x == 0 && y == size - 1) {
		liveCount += grid[x+1][y-1];
	} else if (x == size -1 && y == 0) {
		liveCount += grid[x-1][y+1];
	} else if (x == size -1 && y == size - 1) {
		liveCount += grid[x-1][y-1];
	} else if (x == 0) {
		liveCount += grid[x+1][y-1];
		liveCount += grid[x+1][y+1];
	} else if (x == size - 1) {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x-1][y+1];
	} else if (y == 0) {
		liveCount += grid[x-1][y+1];
		liveCount += grid[x+1][y+1];
	} else if (y == size - 1) {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x+1][y-1];
	} else {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x-1][y+1];
		liveCount += grid[x+1][y-1];
		liveCount += grid[x+1][y+1];
	}

	return liveCount;
}

// CellState processCellStateAlive(char ** grid, Coord * point) {
// 	return Result_live;
// }

// CellState processCellStateDead(char ** grid, Coord * point) {
// 	return Result_live;
// }

CellState processCell(uint16_t * g, uint16_t size, Coord * point) {
	CellState cellState = getCellState(g, size, point->x, point->y);
	CellState newCellState = cellState;

	// printf("\n\t(%d,%d) is", point->x, point->y);
	// if (cellState == CellState_alive) {
	// 	printf(" alive, ");
	// } else {
	// 	printf(" dead, ");
	// }
	

	uint16_t neighborLiveCount = getNeighborLiveCount(g, size, point->x, point->y);
	// printf("\n\t(%d,%d) neighborLiveCount = %d", point->x, point->y, neighborLiveCount);	

	switch (cellState) {
		case CellState_alive: {
			if (neighborLiveCount < 2) {
				newCellState = CellState_dead;
			} else if (neighborLiveCount > 3) {
				newCellState = CellState_dead;
			}
			break;
		}
		case CellState_dead: {
			if (neighborLiveCount == 3) {
				newCellState = CellState_alive;
			}
			break;
		}
	}
	// printf("\n\t(%d,%d) is now ", point->x, point->y);
	// if (newCellState == CellState_alive) {
	// 	printf(" alive, ");
	// } else {
	// 	printf(" dead, ");
	// }
	// printf("\n");
	return newCellState;

	// if (Result_die == underPopulation(grid, point)) {
	// 	return Result_die;
	// }
	// if (Result_die == overCrowding(grid, point)) {
	// 	return Result_die;
	// }
	// if (Result_die == underPopulation(grid, point)) {
	// 	return Result_die;
	// }
}

/**
 * Main entry point
 * 
 * @param grid Square grid - array of arrays with its values either 0 or 1 (0 means dead, 1 means alive)
 * @param size Size of grid
 * @param iterations Number of iterations
 */
void gameEntry(uint16_t * g, uint16_t * newg, uint16_t size, uint16_t iterations) {
	int i, j;
	// LOG("\nPrint table");
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	uint16_t (*newgrid)[size] = (uint16_t (*)[size]) newg;
	Coord coord;
	for (i = 0; i < size; i++) {
		// printf("\n");
		for (j = 0; j < size; j++) {
			// printf("%d", grid[i][j]);
			// printf("%d ", grid[i][j]);
			coord.x = i;
			coord.y = j;
			newgrid[i][j] = processCell(g, size, &coord);
		}
	}
}

void printGameGrid(uint16_t * g, uint16_t size) {
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	int i, j;
	for (i = 0; i < size; i++) {
		printf("\n");
		for (j = 0; j < size; j++) {
			printf("%d ", grid[i][j]);
		}
	}
}

#define TEST_GRID_SIZE_1 (5)
uint16_t emptyGrid [TEST_GRID_SIZE_1][TEST_GRID_SIZE_1];
uint16_t testGrid_1 [TEST_GRID_SIZE_1][TEST_GRID_SIZE_1] = {
	{0, 1, 1, 1, 0},
	{0, 0, 0, 0, 0},
	{0, 1, 1, 1, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
};

/**
 *
 */
int main(int argc, char ** argv) {
	LOG("\nWelcome to the Game of Life!");
	LOG("\n\n");
	int i, j;
	uint16_t sz;
	//memset(emptyGrid, 0, sizeof(emptyGrid));

	FILE *f = fopen(argv[1], "r");
	fscanf (f, "%hd", &sz);    
	printf ("size = %d, build table ", sz);
	// uint16_t **buff_0 = (uint16_t**)malloc(sizeof(uint16_t*)*sz);
	// uint16_t **buff_1 = (uint16_t**)malloc(sizeof(uint16_t*)*sz);
	uint16_t *buff_x = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
	uint16_t *buff_0 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
	uint16_t *buff_1 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
	for (i = 0; i < sz; i++) {
		printf("\n");
		// buff_0[i] = (uint16_t*)malloc(sizeof(uint16_t)*sz);
		// buff_1[i] = (uint16_t*)malloc(sizeof(uint16_t)*sz);
		for (j = 0; j < sz; j++) {
			// fscanf (f, "%hd", &buff_0[i][j]);
			fscanf (f, "%hd", &buff_0[i*sz + j]);
			// printf("%d ", buff_0[i][j]);
			printf("%d ", buff_0[i*sz + j]);
		}
	}
	// printf ("\nsizeof(buff_0) = %lu", sizeof(buff_0));
	// printf ("\nsizeof(testGrid_1) = %lu", sizeof(testGrid_1));
	// printf ("\nsizeof(buff_0[0]) = %lu", sizeof(buff_0[0]));
	// printf ("\nsizeof(testGrid_1[0]) = %lu", sizeof(buff_0[0]));
	// printf ("\naddr buff_0[0][0]) = %p", &buff_0[0][0]);
	// printf ("\naddr buff_0[sz-1][sz-1]) = %p", &buff_0[sz-1][sz-1]);
	// printf ("\naddr testGrid_1[0][0]) = %p", &testGrid_1[0][0]);
	// printf ("\naddr testGrid_1[sz-1][sz-1]) = %p", &testGrid_1[TEST_GRID_SIZE_1-1][TEST_GRID_SIZE_1-1]);
	// printf("\n[i,j]\tbuff_0\testGrid_1\tbuff_x");
	// for (i = 0; i < sz; i++) {
	// 	for (j = 0; j < sz; j++) {
	// 		printf("\n[%d][%d]\t%p ", i, j, &buff_0[i][j]);
	// 		printf("\t%p ", &testGrid_1[i][j]);
	// 		printf("\t%p ", &buff_x[i*sz +j]);
	// 	}
	// }
	// printf("\nlen(buff_0) = %ld", &buff_0[sz-1][sz-1] - &buff_0[0][0]);
	// printf("\nlen(buff_0 row) = %ld", &buff_0[0][sz-1] - &buff_0[0][0]);
	// printf("\nlen(buff_0 col) = %ld", &buff_0[1][0] - &buff_0[0][0]);
	// printf("\nlen(testGrid_1 row) = %ld", &testGrid_1[0][sz-1] - &testGrid_1[0][0]);
	// printf("\nlen(testGrid_1 col) = %ld", &testGrid_1[1][0] - &testGrid_1[0][0]);
	// printf("\nlen(testGrid_1) = %ld", &testGrid_1[sz-1][sz-1] - &testGrid_1[0][0]);
	// printf("\nlen(buff_0) = %ld", &buff_0[sz*sz-1] - &buff_0[0]);

	printf("\n-----1--------------\n");
	printGameGrid(buff_0, sz);
	printf("\n-----2--------------\n");
	memset(buff_1, 0, sizeof(uint16_t)*sz*sz);

	printGameGrid(buff_1, sz);
	printf("\n-------------------\n");
	// while (!feof (f))
	// {  
	//   printf ("%d ", i);
	//   fscanf (f, "%d", &i);      
	// }
	fclose (f); 


	LOG("\nPrint table");
	//char * 
	
	printGameGrid(testGrid_1[0], TEST_GRID_SIZE_1);
	// for (i = 0; i < TEST_GRID_SIZE_1; i++) {
	// 	printf("\n");
	// 	for (j = 0; j < TEST_GRID_SIZE_1; j++) {
	// 		printf("%d ", testGrid_1[i][j]);
	// 	}
	// }

	gameEntry(buff_0, buff_1, sz, 1);
	// gameEntry(testGrid_1[0], emptyGrid[0], TEST_GRID_SIZE_1, 1);
	printf("\nAfter one round\n");
	// printGameGrid(emptyGrid[0], TEST_GRID_SIZE_1);
	printGameGrid(buff_1, sz);
	// for (i = 0; i < TEST_GRID_SIZE_1; i++) {
	// 	printf("\n");
	// 	for (j = 0; j < TEST_GRID_SIZE_1; j++) {
	// 		printf("%d ", emptyGrid[i][j]);
	// 	}
	// }
	// memset(testGrid_1, 0, sizeof(testGrid_1));
	memset(buff_0, 0, sz*sz*sizeof(uint16_t));
	// gameEntry(emptyGrid[0], testGrid_1[0], TEST_GRID_SIZE_1, 1);
	gameEntry(buff_1, buff_0, sz, 1);
	printf("\nAfter second round\n");
	// printGameGrid(testGrid_1[0], TEST_GRID_SIZE_1);
	printGameGrid(buff_0, sz);
	// for (i = 0; i < TEST_GRID_SIZE_1; i++) {
	// 	printf("\n");
	// 	for (j = 0; j < TEST_GRID_SIZE_1; j++) {
	// 		printf("%d ", testGrid_1[i][j]);
	// 	}
	// }

	LOG("\n\n");
}