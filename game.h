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

CellState processCell(uint16_t * g, uint16_t size, Coord * cell);

void playGame(uint16_t * ping, uint16_t * pong, uint16_t size, uint16_t iterations);

extern uint16_t *buff_0;
extern uint16_t *buff_1;
extern struct GameOps * pGameOps;

struct GameOps * getGameOps_Bitfield();
struct GameOps * getGameOps_Large();

#endif /* _GAME_H */