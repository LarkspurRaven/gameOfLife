#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

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

void clearBuff_Large(uint16_t * buff, uint16_t size) {
	memset(buff, 0, size*size*sizeof(uint16_t));
}

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

struct GameOps * getGameOps_Large() {
	return &gameOpsLarge;
}