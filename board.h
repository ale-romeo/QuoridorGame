#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "CAN/CAN.h"

#define	DIM				15

// Definizione delle costanti per gli shift
#define PLAYER_ID_SHIFT     24
#define MOVE_TYPE_SHIFT     20
#define WALL_ORIENTATION_SHIFT  16
#define NEW_X_POS_SHIFT     8
#define NEW_Y_POS_SHIFT     0

// Definizione delle costanti per le maschere
#define PLAYER_ID_MASK     0xFF
#define MOVE_TYPE_MASK     0xF
#define WALL_ORIENTATION_MASK 0xF
#define NEW_X_POS_MASK     0xFF
#define NEW_Y_POS_MASK     0xFF

typedef enum {
	None,
	SingleBoard,
	TwoBoard
} BoardType;

typedef enum {
	NA,
	Human,
	NPC
} GameMode;

typedef enum {
	FROZEN,
	MENU,
	MOVEPLAYER,
	MOVEWALL
} Mode;

typedef enum {
	NOTVISITED,
	VISITED
} Visit;

typedef enum {
	NOTMOVING,
	MOVEDAWAY,
	MOVING
} StateWall;

typedef enum {
	UNKNOWN,
  EMPTY,
	MKEMPTY,
	POSSMOVE,
	SELMOVE,
  PLAYER1,
	PLAYER2,
	NOWALL,
  WALL
} StateCell;

typedef enum {
	NONE,
	CROSS,
	VERTICAL,
	HORIZONTAL
} OrientationWall;

// Struttura dati per rappresentare una casella della scacchiera
typedef struct {
	StateCell state;
	double x_pos;
	double y_pos;
	OrientationWall orWall;
	StateWall wallState;
} Cell;

typedef struct {
	StateCell state;
} testCell;

// Struttura dati per rappresentare la scacchiera di gioco
typedef struct {
  Cell cells[DIM][DIM];
} Board;

typedef struct {
  testCell cells[DIM][DIM];
} testBoard;


bool CheckWin(Board* board, StateCell PLAYER);
void initBoard(Board* board);
void resetBoard(Board* board);
void calcPossMovesPlayer(Board* board, StateCell PLAYER);
void testcalcPossMovesPlayer(testBoard* board, StateCell PLAYER);
void MovePlayerUp(Board* board, StateCell PLAYER);
void MovePlayerRight(Board* board, StateCell PLAYER);
void MovePlayerLeft(Board* board, StateCell PLAYER);
void MovePlayerDown(Board* board, StateCell PLAYER);
void MovePlayerDownLeft(Board* board, StateCell PLAYER);
void MovePlayerDownRight(Board* board, StateCell PLAYER);
void MovePlayerUpLeft(Board* board, StateCell PLAYER);
void MovePlayerUpRight(Board* board, StateCell PLAYER);
void MoveWallUp(Board* board);
void MoveWallLeft(Board* board);
void MoveWallRight(Board* board);
void MoveWallDown(Board* board);
void UnsetPossMoves(testBoard *board);
void UnsetSelMoves(testBoard *board);
void RotateMovingWall(Board* board);
void UnsetMovingWalls(Board *board);
void UnsetMovedWalls(Board *board);
void ConfirmMovePlayer(Board* board, StateCell PLAYER);
void testConfirmMovePlayer(testBoard* board, StateCell PLAYER);
bool ConfirmMoveWall(Board* board);
void testConfirmMoveWall(Board* board);
void copyBoard(testBoard* dest, Board* source);
void copytestBoard(testBoard* dest, testBoard* source);
bool explore_possMoves(testBoard* board, StateCell PLAYER);
bool CheckValidWall(Board* board);
void bestPathNPC(testBoard* board, StateCell NPC, int *best_distance, int depth);
void MakeOpponentMove(Board* board, Move* opponentMove);
void MakeNPCMove(Board* board, StateCell NPC);
void ResetMove(Move* move, StateCell player);
