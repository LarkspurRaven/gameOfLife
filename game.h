#ifndef _GAME_H
#define _GAME_H

#define LOGW(f_, ...) printf((f_), ##__VA_ARGS__)

#ifdef REDUCE_LOG
#define LOG(f_, ...)
#else
#define LOG(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

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

/// Defines Game Operations that must be implemented
struct GameOps {
    /**
     * Retreive the @c CellState encoding for a particular coordinate.
     *
     * @param g The Cell Game Grid, flattened 2d array
     * @param size Size of game, specifying equal length and width grid
     * @param cell The Cell at @c Coord to retreive state for.
     * @return Return @c CellState encoding for a particular coordinate.
     */
    CellState (*getCellState)(uint16_t * g, uint16_t size, Coord * cell);

    /**
     * Calculate the number of neighbors of a Cell that are alive. Checks up/down,
     * left/right, and across diagonals.
     *
     * @param g The Cell Game Grid
     * @param size Size of game, specifying equal length and width grid
     * @return Return live neighbor count of a Cell at @c Coord @p cell.
     */
    uint16_t (*getNeighborLiveCount)(uint16_t * g, uint16_t size, Coord * cell);

    /**
     * Main entry point for playing the game.
     *
     * @param grid Square grid - array of arrays with its values either 0 or 1 (0 means dead,
     *      1 means alive).
     * @param size Size of grid.
     * @param iterations Number of iterations.
     */
    void (*playGame)(uint16_t * ping, uint16_t * pong, uint16_t size, uint16_t iterations);

    /**
     * Print the game grid.
     *
     * @param g The Cell Game Grid.
     * @param size Size of grid.
     */
    void (*printGameGrid)(uint16_t * g, uint16_t size);

    /**
     * Plays one round of the game, iterating over each cell of the grid.
     *
     * @param src Square grid - array of arrays with its values either 0 or 1 (0 means dead,
     * 1 means alive).
     * @param dst New grid state will be stored here after the round is completed.
     * @param size Size of grid.
     */
    void (*gameRound)(uint16_t * src, uint16_t * dst, uint16_t size);

    /**
     * Calculate new @c CellState for a Cell at @c Coord @p cell.
     *
     * @param g The Cell Game Grid, flattened 2d array.
     * @param size Size of game, specifying equal length and width grid.
     * @param cell The Cell at @c Coord to udpate state for.
     * @return Return @c CellState encoding for a particular coordinate.
     */
    CellState (*processCell)(uint16_t * g, uint16_t size, Coord * cell);

    /**
     * Parse an input file that represents the game to consturct a game grid.
     *
     * @param file The input file to parse.
     * @param psize Pointer used for parseFile() to set game size.
     * @return Returns 0 for success, other for error.
     */
    int (*parseFile)(char * file, uint16_t * psize);

    /**
     * Clears a buffer representing the game grid.
     *
     * @param buff The game grid buffer to clear.
     * @param size Size of game, specifying equal length and width grid
     */
    void (*clearBuff)(uint16_t * buff, uint16_t size);
};

/// Shared implementation for GameOps method processCell
CellState processCell(uint16_t * g, uint16_t size, Coord * cell);

/// Shared implementation for GameOps method playGame
void playGame(uint16_t * ping, uint16_t * pong, uint16_t size, uint16_t iterations);

/// Global data
extern uint16_t *buff_0;
extern uint16_t *buff_1;
extern struct GameOps * pGameOps;

///
struct GameOps * getGameOps_Bitfield();
struct GameOps * getGameOps_Large();

#endif /* _GAME_H */