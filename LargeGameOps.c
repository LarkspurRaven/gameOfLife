#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

/**
 * @relates struct GameOps
 * getCellState function
 */
CellState getCellState_Large(uint16_t * g, uint16_t size, Coord * cell) {
    uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
    return (grid[cell->x][cell->y] == 1) ? CellState_alive : CellState_dead;
}

/**
 * @relates struct GameOps
 * getNeighborLiveCount function
 */
uint16_t getNeighborLiveCount_Large(uint16_t * g, uint16_t size, Coord * cell) {
    uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
    uint16_t liveCount = 0;
    int16_t x = (int16_t)cell->x; // Conversion ok for this cell representation
    int16_t y = (int16_t)cell->y; // Conversion ok for this cell representation

    int dx, dy;

    for (dx = -1; dx <= 1; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            // Check for self
            if (dx == 0 && dy == 0) {
                // Ignore self, continue
                continue;
            }
            // Check boundary conditions
            if (x + dx < 0 || x + dx >= size
                    || y + dy < 0 || y + dy >= size) {
                // Off Grid, continue
                continue;
            }
            liveCount += grid[x+dx][y+dy];
        }
    }
    return liveCount;
}

/**
 * @relates struct GameOps
 * gameRound function
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

/**
 * @relates struct GameOps
 * printGameGrid function
 */
void printGameGrid_Large(uint16_t * g, uint16_t size) {
    LOG("\nprintGameGrid_Large, size=%d", size);
    uint16_t (*grid)[size] = (uint16_t (*)[size]) g;
    int i, j;
    for (i = 0; i < size; i++) {
        LOG("\n");
        for (j = 0; j < size; j++) {
            LOG("%d ", grid[i][j]);
        }
    }
}

/**
 * @relates struct GameOps
 * parseFile function
 */
int parseFile_Large(char * file, uint16_t * psz) {
    LOG("\nparseFile");

    char dirName [50] = "./tests/";
    if (strlen(file) + strlen(dirName) > 50) {
        LOGW("\nFilename too long");
        return 1;
    }

    FILE *f = fopen(strcat(dirName, file), "r");

    uint16_t sz;

    // Read size of grid, lenght = width = size
    fscanf(f, "%hd", &sz);
    *psz = sz;
    LOG("\nBuild table of size %d x %d ", sz, sz);

    // Allocate sufficient memory for ping and pong buffers
    buff_0 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);
    buff_1 = (uint16_t*)malloc(sizeof(uint16_t)*sz*sz);

    uint16_t v;
    int i, j;
    for (i = 0; i < sz; i++) {
        LOG("\n");
        for (j = 0; j < sz; j++) {
            if (feof(f)) {
                LOG("\nError! End of file reached before building game grid. Input file likely \
                    not formated correctly.");
                LOG("\nExpect grid dimension on first line, followed by n x n grid\n\n");
                goto close;
            }
            fscanf(f, "%hd", &v);
            if (v != 0 && v != 1) {
                LOG("\nError! Invalid Game Vallue for cell (%d,%d) = %d",
                    i, j, v);
                LOG("\nValid values are 0 or 1.\n\n");
                goto close;
            }
            buff_0[i*sz + j] = v;
            LOG("%d ", buff_0[i*sz + j]);
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

/**
 * @relates struct GameOps
 * clearBuff function
 */
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