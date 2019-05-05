#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

/**
 * Game of Life Rules
 * 	1. Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 * 	2. Any live cell with two or three live neighbours lives on to the next generation.
 * 	3. Any live cell with more than three live neighbours dies, as if by overcrowding.
 * 	4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */

uint16_t *buff_0;
uint16_t *buff_1;

struct GameOps * pGameOps;

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
		LOG("\n\nAfter round %d", i+1);
		pGameOps->printGameGrid(dst, size);
		tmp = src;
		src = dst;
		dst = tmp;
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
	// int i, j;
	uint16_t sz, numIterations;

	if (argc < 3) {
		LOG("\nRun program in this format: ./a.out test_file num_iterations");
		LOG("\nEg ./a.out tc_1 2\n\n");
		return 1;
	}

#ifdef GAME_LARGE
	LOG("\nSetting up Large Game");
	pGameOps = getGameOps_Large();
#else
	LOG("\nSetting up Bitfield Game");
	pGameOps = getGameOps_Bitfield();
#endif

	LOG("\nParsing game in file: %s", argv[1]);
	numIterations = atoi(argv[2]);

	if (pGameOps->parseFile(argv[1], &sz)) {
		return 1;
	}

	LOG("\n\nInitial grid");
	pGameOps->printGameGrid(buff_0, sz);

	LOG("\n\nWill run game for %d iterations.", numIterations);

	pGameOps->playGame(buff_0, buff_1, sz, numIterations);

	free(buff_0);
	free(buff_1);

	LOGW("\n\nGAME OVER\n\n");

	return 0;
}