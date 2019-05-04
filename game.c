#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Game of Life Rules
 * 	1. Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 * 	2. Any live cell with two or three live neighbours lives on to the next generation.
 * 	3. Any live cell with more than three live neighbours dies, as if by overcrowding.
 * 	4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */

/// Define valid cell states
typedef enum {
	CellState_dead,
	CellState_alive,
} CellState;

/// Defined @c Coord struct to define point coordinate; currenly 2d (x,y)
typedef struct {
	uint16_t x;
	uint16_t y;
} Coord;

#define LOG(x) printf(x)
//#define LOG(x)

static uint16_t *buff_0;
static uint16_t *buff_1;

struct GameOps {
	CellState (*getCellState)(uint16_t * g, uint16_t size, Coord * cell);
	uint16_t (*getNeighborLiveCount)(uint16_t * g, uint16_t size, Coord * cell);
	void (*playGame)(uint16_t * ping, uint16_t * pong, uint16_t size, uint16_t iterations);
	void (*printGameGrid)(uint16_t * g, uint16_t size);
	void (*gameRound)(uint16_t * src, uint16_t * dst, uint16_t size);
	CellState (*processCell)(uint16_t * g, uint16_t size, Coord * cell);
	int (*parseFile)(char * file, uint16_t * psz);
	void (*clearBuff)(uint16_t * buff, uint16_t size);
};

struct GameOps * pGameOps;

/**
 * Retreive the @c CellState encoding for a particular coordinate.
 *
 * @param g The Cell Game Grid, flattened 2d array
 * @param size Size of game, specifying equal length and width grid
 * @param cell The Cell at @c Coord to retreive state for.
 * @return Return @c CellState encoding for a particular coordinate.
 */
CellState getCellState_Large(uint16_t * g, uint16_t size, Coord * cell) {
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	return (grid[cell->x][cell->y] == 1) ? CellState_alive : CellState_dead;
}

CellState getCellState_Bitfield(uint16_t * grid, uint16_t size, Coord * cell) {
	return (grid[cell->x] & 1<<cell->y) ? CellState_alive : CellState_dead;
}

/**
 * Calculate the number of neighbors of a Cell that are alive. Checks up/down,
 * left/right, and across diagonals.
 *
 * @param g The Cell Game Grid, flattened 2d array
 * @param size Size of game, specifying equal length and width grid
 * @return Return live neighbor count of a Cell at @c Coord @p cell.
 */
uint16_t getNeighborLiveCount_Large(uint16_t * g, uint16_t size, Coord * cell) {
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	uint16_t liveCount = 0;
	uint16_t x = cell->x;
	uint16_t y = cell->y;
	
	// Check x-axis, left/right
	// grid[x+/-1][y]
	if (x == 0) {
		liveCount += grid[x+1][y];
	} else if (x == size-1) {
		liveCount += grid[x-1][y];
	} else {
		liveCount += grid[x+1][y];
		liveCount += grid[x-1][y];
	}
	
	// Check y-axis, up/down
	// grid[x][y+/-1]
	if (y == 0) {
		liveCount += grid[x][y+1];
	} else if (y == size-1) {
		liveCount += grid[x][y-1];
	} else {
		liveCount += grid[x][y+1];
		liveCount += grid[x][y-1];
	}

	// Check diagonals
	// upper left corner
	if (x == 0 && y == 0) {
		liveCount += grid[x+1][y+1];
	} 
	// lower left corner
	else if (x == 0 && y == size - 1) {
		liveCount += grid[x+1][y-1];
	}
	// upper right corner
	else if (x == size -1 && y == 0) {
		liveCount += grid[x-1][y+1];
	}
	// lower right corner
	else if (x == size -1 && y == size - 1) {
		liveCount += grid[x-1][y-1];
	}
	// left edge (no diags on left)
	else if (x == 0) {
		liveCount += grid[x+1][y-1];
		liveCount += grid[x+1][y+1];
	} 
	// right edge (no diags on right)
	else if (x == size - 1) {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x-1][y+1];
	} 
	// top edge (no diags above)
	else if (y == 0) {
		liveCount += grid[x-1][y+1];
		liveCount += grid[x+1][y+1];
	}
	// bottom edge (no diags below)
	else if (y == size - 1) {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x+1][y-1];
	}
	// no edge or corner, get all diags
	else {
		liveCount += grid[x-1][y-1];
		liveCount += grid[x-1][y+1];
		liveCount += grid[x+1][y-1];
		liveCount += grid[x+1][y+1];
	}

	return liveCount;
}

uint16_t getNeighborLiveCount_Bitfield(uint16_t * grid, uint16_t size, Coord * cell) {
	uint16_t liveCount = 0;
	uint16_t x = cell->x;
	uint16_t y = cell->y;

	// Check x-axis, left/right
	// grid[x+/-1][y]
	if (x == 0) {
		liveCount += (grid[x+1] & 1<<y) ? 1 : 0;
	} else if (x == size-1) {
		liveCount += (grid[x-1] & 1<<y) ? 1 : 0;
	} else {
		liveCount += (grid[x+1] & 1<<y) ? 1 : 0;
		liveCount += (grid[x-1] & 1<<y) ? 1 : 0;
	}
	
	// Check y-axis, up/down
	// grid[x][y+/-1]
	if (y == 0) {
		liveCount += (grid[x] & 1<<(y+1)) ? 1 : 0;
	} else if (y == size-1) {
		liveCount += (grid[x] & 1<<(y-1)) ? 1 : 0;
	} else {
		liveCount += (grid[x] & 1<<(y+1)) ? 1 : 0;
		liveCount += (grid[x] & 1<<(y-1)) ? 1 : 0;
	}

	// Check diagonals
	// upper left corner
	if (x == 0 && y == 0) {
		liveCount += (grid[x+1] & 1<<(y+1)) ? 1 : 0;
	} 
	// lower left corner
	else if (x == 0 && y == size - 1) {
		liveCount += (grid[x+1] & 1<<(y-1)) ? 1 : 0;
	}
	// upper right corner
	else if (x == size -1 && y == 0) {
		liveCount += (grid[x-1] & 1<<(y+1)) ? 1 : 0;
	}
	// lower right corner
	else if (x == size -1 && y == size - 1) {
		liveCount += (grid[x-1] & 1<<(y-1)) ? 1 : 0;
	}
	// left edge (no diags on left)
	else if (x == 0) {
		liveCount += (grid[x+1] & 1<<(y+1)) ? 1 : 0;
		liveCount += (grid[x+1] & 1<<(y-1)) ? 1 : 0;
	} 
	// right edge (no diags on right)
	else if (x == size - 1) {
		liveCount += (grid[x-1] & 1<<(y-1)) ? 1 : 0;
		liveCount += (grid[x-1] & 1<<(y+1)) ? 1 : 0;
	} 
	// top edge (no diags above)
	else if (y == 0) {
		liveCount += (grid[x-1] & 1<<(y+1)) ? 1 : 0;
		liveCount += (grid[x+1] & 1<<(y+1)) ? 1 : 0;
	}
	// bottom edge (no diags below)
	else if (y == size - 1) {
		liveCount += (grid[x-1] & 1<<(y-1)) ? 1 : 0;
		liveCount += (grid[x+1] & 1<<(y-1)) ? 1 : 0;
	}
	// no edge or corner, get all diags
	else {
		liveCount += (grid[x-1] & 1<<(y-1)) ? 1 : 0;
		liveCount += (grid[x-1] & 1<<(y+1)) ? 1 : 0;
		liveCount += (grid[x+1] & 1<<(y-1)) ? 1 : 0;
		liveCount += (grid[x+1] & 1<<(y+1)) ? 1 : 0;
	}

	return liveCount;
}

/**
 * Calculate new @c CellState for a Cell at @c Coord @p cell
 *
 * @param g The Cell Game Grid, flattened 2d array
 * @param size Size of game, specifying equal length and width grid
 * @param cell The Cell at @c Coord to udpate state for.
 * @return Return @c CellState encoding for a particular coordinate.
 */
CellState processCell(uint16_t * g, uint16_t size, Coord * cell) {
	CellState cellState = pGameOps->getCellState(g, size, cell);
	CellState newCellState = cellState;

	uint16_t neighborLiveCount = pGameOps->getNeighborLiveCount(g, size, cell);

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
	return newCellState;
}

/**
 * Plays one round of the game, iterating over each cell of the grid.
 * 
 * @param src Square grid - array of arrays with its values either 0 or 1 (0 means dead,
 * 1 means alive)
 * @param dst New grid state will be stored here after the round is completed.
 * @param size Size of grid
 */
void gameRound_Large(uint16_t * src, uint16_t * dst, uint16_t size) {
	int i, j;
	// LOG("\nPrint table");
	uint16_t (*grid)[size] = (uint16_t (*)[size]) src;
	uint16_t (*newgrid)[size] = (uint16_t (*)[size]) dst;
	Coord cell;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			cell.x = i;
			cell.y = j;
			newgrid[i][j] = pGameOps->processCell(src, size, &cell);
		}
	}
}

void gameRound_Bitfield(uint16_t * src, uint16_t * dst, uint16_t size) {
	int i, j;

	Coord cell;
	CellState newCellState;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			cell.x = i;
			cell.y = j;
			newCellState = pGameOps->processCell(src, size, &cell);
			if (newCellState == CellState_alive) {
				dst[i] |= 1<<j;
			}
		}
	}
}

void printGameGrid_Large(uint16_t * g, uint16_t size) {
	printf("\nprintGameGrid_Large, size=%d", size);
	uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
	int i, j;
	for (i = 0; i < size; i++) {
		printf("\n");
		for (j = 0; j < size; j++) {
			printf("%d ", grid[i][j]);
		}
	}
}

void printGameGrid_Bitfield(uint16_t * g, uint16_t size) {
	printf("\nprintGameGrid_Bitfield, size=%d", size);
	int i, j;
	for (i = 0; i < size; i++) {
		printf("\n");
		for (j = 0; j < size; j++) {
			printf("%d ", (g[i] & 1<<j) ? 1 : 0);
		}
	}
}

/**
 * Main entry point
 * 
 * @param grid Square grid - array of arrays with its values either 0 or 1 (0 means dead,
 * 		1 means alive)
 * @param size Size of grid
 * @param iterations Number of iterations
 */
void playGame(uint16_t * ping, uint16_t * pong, uint16_t size, uint16_t iterations) {
	uint16_t *src, *dst, *tmp;
	src = ping;
	dst = pong;
	int i;
	for (i = 0; i < iterations; i++) {
		pGameOps->clearBuff(dst, size);
		pGameOps->gameRound(src, dst, size);
		printf("\n\nAfter round %d", i+1);
		pGameOps->printGameGrid(dst, size);
		tmp = src;
		src = dst;
		dst = tmp;
	}
}

int parseFile_Large(char * file, uint16_t * psz) {
	printf("\nparseFile");

	FILE *f = fopen(file, "r");
	uint16_t sz;

	// Read size of grid, lenght = width = size
	fscanf(f, "%hd", &sz);
	*psz = sz;
	printf("\nBuild table of size %d x %d ", sz, sz);

	// Allocate sufficient memory for ping and pong buffers
	buff_0 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
	buff_1 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
	
	uint16_t v;
	int i, j;
	for (i = 0; i < sz; i++) {
		printf("\n");
		for (j = 0; j < sz; j++) {
			if (feof(f)) {
				printf("\nError! End of file reached before building game grid. Input file likely \
					not formated correctly.");
				printf("\nExpect grid dimension on first line, followed by n x n grid\n\n");
				goto close;
			}
			fscanf(f, "%hd", &v);
			if (v != 0 && v != 1) {
				printf("\nError! Invalid Game Vallue for cell (%d,%d) = %d",
					i, j, v);
				printf("\nValid values are 0 or 1.\n\n");
				goto close;
			}
			buff_0[i*sz + j] = v;
			printf("%d ", buff_0[i*sz + j]);
		}
	}	
	fclose (f);
	return 0;
close:
	fclose(f);
	free(buff_0);
	free(buff_1);
	return 1;
}

int parseFile_Bitfield(char * file, uint16_t * psz) {
	printf("\nparseFile_Bitfield_0");

	FILE *f = fopen(file, "r");
	uint16_t sz;

	// Read size of grid, lenght = width = size
	fscanf(f, "%hd", &sz);
	*psz = sz;
	printf("\nBuild table of size %d x %d ", sz, sz);

	// Allocate sufficient memory for ping and pong buffers
	buff_0 = (uint16_t*)malloc(sizeof(uint16_t)*sz);
	buff_1 = (uint16_t*)malloc(sizeof(uint16_t)*sz);
	
	printf("sz(buff_0)=%lu", sizeof(buff_0));
	printf("sz(buff_0[0])=%lu", sizeof(buff_0[0]));

	uint16_t v;
	int i, j; // i: rows, j: columns
	for (i = 0; i < sz; i++) {
		printf("\n");
		memset(&buff_0[i], 0, sizeof(buff_0[i]));
		for (j = 0; j < sz; j++) {
			if (feof(f)) {
				printf("\nError! End of file reached before building game grid. Input file likely \
					not formated correctly.");
				printf("\nExpect grid dimension on first line, followed by n x n grid\n\n");
				goto close;
			}
			fscanf(f, "%hd", &v);
			if (v != 0 && v != 1) {
				printf("\nError! Invalid Game Vallue for cell (%d,%d) = %d",
					i, j, v);
				printf("\nValid values are 0 or 1.\n\n");
				goto close;
			}
			if (v == 1) {
				buff_0[i] |= 1<<j;
			}
			
			printf("%d ", v);
		}
	}

	printGameGrid_Bitfield(buff_0, sz);
	// for (i = 0; i < sz; i++) {
	// 	printf("\n0x%x ", buff_0[i]);
	// }

	fclose (f);
	printf("\nparseFile_Bitfield_End");
	return 0;
close:
	fclose(f);
	free(buff_0);
	free(buff_1);
	return 1;
}

void clearBuff_Bitfield(uint16_t * buff, uint16_t size) {
	memset(buff, 0, sizeof(uint16_t)*size);
}

void clearBuff_Large(uint16_t * buff, uint16_t size) {
	memset(buff, 0, size*size*sizeof(uint16_t));
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

struct GameOps gameOpsLarge = {
	.getCellState = getCellState_Large,
	.getNeighborLiveCount = getNeighborLiveCount_Large,
	.playGame = playGame,
	.printGameGrid = printGameGrid_Large,
	.gameRound = gameRound_Large,
	.processCell = processCell,
	.parseFile = parseFile_Large,
	.clearBuff = clearBuff_Large,
};

struct GameOps gameOpsBitfield = {
	.getCellState = getCellState_Bitfield,
	.getNeighborLiveCount = getNeighborLiveCount_Bitfield,
	.playGame = playGame,
	.printGameGrid = printGameGrid_Bitfield,
	.gameRound = gameRound_Bitfield,
	.processCell = processCell,
	.parseFile = parseFile_Bitfield,
	.clearBuff = clearBuff_Bitfield,
};

/**
 *
 */
int main(int argc, char ** argv) {
	LOG("\nWelcome to the Game of Life!");
	LOG("\n\n");
	// int i, j;
	uint16_t sz, numIterations;

	// pGameOps = &gameOpsLarge;
	pGameOps = &gameOpsBitfield;

	if (argc < 3) {
		printf("\nRun program in this format: ./a.out test_file num_iterations");
		printf("\nEg ./a.out tc_1 2\n\n");
		return 1;
	}
	printf("\nParsing game in file: %s", argv[1]);
	numIterations = atoi(argv[2]);

	if (pGameOps->parseFile(argv[1], &sz)) {
		return 1;
	}

	printf("\n\nInitial grid");
	pGameOps->printGameGrid(buff_0, sz);

	printf("\n\nWill run game for %d iterations.", numIterations);

	pGameOps->playGame(buff_0, buff_1, sz, numIterations);

	free(buff_0);
	free(buff_1);

	LOG("\n\n");
	return 0;
}