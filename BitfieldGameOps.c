#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

/**
 * @relates struct GameOps
 * getCellState function
 */
CellState getCellState_Bitfield(uint16_t * grid, uint16_t size, Coord * cell) {
    return (grid[cell->x] & 1<<cell->y) ? CellState_alive : CellState_dead;
}

/**
 * @relates struct GameOps
 * getNeighborLiveCount function
 */
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
 * @relates struct GameOps
 * gameRound function
 */
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

/**
 * @relates struct GameOps
 * printGameGrid function
 */
void printGameGrid_Bitfield(uint16_t * g, uint16_t size) {
    LOG("\nprintGameGrid_Bitfield, size=%d", size);
    int i, j;
    for (i = 0; i < size; i++) {
        LOG("\n");
        for (j = 0; j < size; j++) {
            LOG("%d ", (g[i] & 1<<j) ? 1 : 0);
        }
    }
}

/**
 * @relates struct GameOps
 * parseFile function
 */
int parseFile_Bitfield(char * file, uint16_t * psz) {
    LOG("\nparseFile_Bitfield_0");

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
    buff_0 = (uint16_t*)malloc(sizeof(uint16_t)*sz);
    buff_1 = (uint16_t*)malloc(sizeof(uint16_t)*sz);

    LOG("sz(buff_0)=%lu", sizeof(buff_0));
    LOG("sz(buff_0[0])=%lu", sizeof(buff_0[0]));

    uint16_t v;
    int i, j; // i: rows, j: columns
    for (i = 0; i < sz; i++) {
        LOG("\n");
        memset(&buff_0[i], 0, sizeof(buff_0[i]));
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
            if (v == 1) {
                buff_0[i] |= 1<<j;
            }

            LOG("%d ", v);
        }
    }

    printGameGrid_Bitfield(buff_0, sz);

    fclose (f);
    LOG("\nparseFile_Bitfield_End");
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
void clearBuff_Bitfield(uint16_t * buff, uint16_t size) {
    memset(buff, 0, sizeof(uint16_t)*size);
}

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

struct GameOps * getGameOps_Bitfield() {
    return &gameOpsBitfield;
}

