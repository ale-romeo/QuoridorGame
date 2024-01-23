#include "board.h"

extern uint8_t or_wall;
extern uint8_t new_x_pos;
extern uint8_t new_y_pos;
extern unsigned int P1Walls;
extern unsigned int P2Walls;
extern Mode mode;

Visit visited[7][7];
int score[7][7];
NPCMode NPC_mode;

bool valid_wall = true;

bool CheckWin(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER && PLAYER == PLAYER1 && j == 1)
				return true;
			if(board->cells[i][j].state == PLAYER && PLAYER == PLAYER2 && j == 13)
				return true;
		}
	}
	return false;
}

// Funzione per inizializzare la scacchiera di gioco
void initBoard(Board* board) {
	int i, j;
	double x, y = 33.75;
	
	//Inizializzazione delle caselle della scacchiera
	for (j = 1; j < DIM; j+=2) {
		x = 3.75;
		for (i = 1; i < DIM; i+=2) {
			board->cells[i][j].state = EMPTY;
			board->cells[i][j].x_pos = x;
			board->cells[i][j].y_pos = y;
			x += 33.75;
			if (j == 1 && i == 7){
				board->cells[i][j].state = PLAYER2;
			}
			else if (j == 13 && i == 7){
				board->cells[i][j].state = PLAYER1;
			}
		}
		y += 33.75;
	}

	//Inizializzazione dei muri orizzontali tra le caselle della scacchiera
	y = 1.875;
	for (j = 0; j < DIM; j+=2) {
		x = 1.875;
		for (i = 1; i < DIM; i+=2) {
			if(board->cells[i][j].state == UNKNOWN){
				if (j == 0 || j == 14)
					board->cells[i][j].state = WALL;
				else
					board->cells[i][j].state = NOWALL;
				board->cells[i][j].orWall = HORIZONTAL;
				board->cells[i][j].x_pos = x;
				board->cells[i][j].y_pos = y;
				x += 33.75;
			}
		}
		y += 33.75;
	}
	
	//Inizializzazione dei muri verticali tra le caselle della scacchiera
	y = 1.875;
	for (j = 1; j < DIM; j+=2) {
		x = 1.875;
		for (i = 0; i < DIM; i+=2) {
			if(board->cells[i][j].state == UNKNOWN){
				if (i == 0 || i == 14)
					board->cells[i][j].state = WALL;
				else
					board->cells[i][j].state = NOWALL;
				board->cells[i][j].orWall = VERTICAL;
				board->cells[i][j].x_pos = x;
				board->cells[i][j].y_pos = y;
				x += 33.75;
			}
		}
		y += 33.75;
	}
	
	//Inizializzazione dei muri agli incroci tra le caselle della scacchiera
	y = 1.875;
	for (j = 0; j < DIM; j+=2) {
		x = 1.875;
		for (i = 0; i < DIM; i+=2) {
			if(board->cells[i][j].state == UNKNOWN){
				if (i == 0 || i == 14 || j == 0 || j == 14)
					board->cells[i][j].state = WALL;
				else
					board->cells[i][j].state = NOWALL;
				board->cells[i][j].orWall = CROSS;
				board->cells[i][j].x_pos = x;
				board->cells[i][j].y_pos = y;
				x += 33.75;
			}
		}
		y += 33.75;
	}
}

void resetBoard(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			board->cells[i][j].state = UNKNOWN;
		}
	}
}

//Calcola le mosse possibili del PLAYER sulla board
void calcPossMovesPlayer(Board* board, StateCell PLAYER){
	StateCell OPPONENT = (PLAYER == PLAYER1) ? PLAYER2 : PLAYER1;
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == EMPTY)
					board->cells[i-2][j].state = POSSMOVE;
				if(board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == EMPTY)
					board->cells[i+2][j].state = POSSMOVE;
				if(board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == EMPTY)
					board->cells[i][j-2].state = POSSMOVE;
				if(board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == EMPTY)
					board->cells[i][j+2].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == NOWALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT)
					board->cells[i-4][j].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == WALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT && board->cells[i-2][j+1].state == NOWALL)
					board->cells[i-2][j+2].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == WALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT && board->cells[i-2][j-1].state == NOWALL)
					board->cells[i-2][j-2].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == NOWALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT)
					board->cells[i+4][j].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == WALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT && board->cells[i+2][j+1].state == NOWALL)
					board->cells[i+2][j+2].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == WALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT && board->cells[i+2][j-1].state == NOWALL)
					board->cells[i+2][j-2].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == NOWALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT)
					board->cells[i][j-4].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == WALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT && board->cells[i+1][j-2].state == NOWALL)
					board->cells[i+2][j-2].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == WALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT && board->cells[i-1][j-2].state == NOWALL)
					board->cells[i-2][j-2].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == NOWALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT)
					board->cells[i][j+4].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == WALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT && board->cells[i+1][j+2].state == NOWALL)
					board->cells[i+2][j+2].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == WALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT && board->cells[i-1][j+2].state == NOWALL)
					board->cells[i-2][j+2].state = POSSMOVE;
				return;
			}
		}
	}
}

//Calcola le mosse possibili del PLAYER sulla testBoard
void testcalcPossMovesPlayer(testBoard* board, StateCell PLAYER){
	StateCell OPPONENT = (PLAYER == PLAYER1) ? PLAYER2 : PLAYER1;
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == EMPTY)
					board->cells[i-2][j].state = POSSMOVE;
				if(board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == EMPTY)
					board->cells[i+2][j].state = POSSMOVE;
				if(board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == EMPTY)
					board->cells[i][j-2].state = POSSMOVE;
				if(board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == EMPTY)
					board->cells[i][j+2].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == NOWALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT)
					board->cells[i-4][j].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == WALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT && board->cells[i-2][j+1].state == NOWALL)
					board->cells[i-2][j+2].state = POSSMOVE;
				if(i-3>0 && board->cells[i-3][j].state == WALL && board->cells[i-1][j].state == NOWALL && board->cells[i-2][j].state == OPPONENT && board->cells[i-2][j-1].state == NOWALL)
					board->cells[i-2][j-2].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == NOWALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT)
					board->cells[i+4][j].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == WALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT && board->cells[i+2][j+1].state == NOWALL)
					board->cells[i+2][j+2].state = POSSMOVE;
				if(i+3<DIM && board->cells[i+3][j].state == WALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == OPPONENT && board->cells[i+2][j-1].state == NOWALL)
					board->cells[i+2][j-2].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == NOWALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT)
					board->cells[i][j-4].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == WALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT && board->cells[i+1][j-2].state == NOWALL)
					board->cells[i+2][j-2].state = POSSMOVE;
				if(j-3>0 && board->cells[i][j-3].state == WALL && board->cells[i][j-1].state == NOWALL && board->cells[i][j-2].state == OPPONENT && board->cells[i-1][j-2].state == NOWALL)
					board->cells[i-2][j-2].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == NOWALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT)
					board->cells[i][j+4].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == WALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT && board->cells[i+1][j+2].state == NOWALL)
					board->cells[i+2][j+2].state = POSSMOVE;
				if(j+3<DIM && board->cells[i][j+3].state == WALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == OPPONENT && board->cells[i-1][j+2].state == NOWALL)
					board->cells[i-2][j+2].state = POSSMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa LEFT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerLeft(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i-2][j].state == POSSMOVE)
					board->cells[i-2][j].state = SELMOVE;
				else if(board->cells[i-4][j].state == POSSMOVE)
					board->cells[i-4][j].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa DOWNLEFT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerDownLeft(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i-2][j+2].state == POSSMOVE)
					board->cells[i-2][j+2].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa DOWNLEFT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerDownRight(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i+2][j+2].state == POSSMOVE)
					board->cells[i+2][j+2].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa DOWN del PLAYER: POSSMOVE->SELMOVE
void MovePlayerDown(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i][j+2].state == POSSMOVE)
					board->cells[i][j+2].state = SELMOVE;
				else if(board->cells[i][j+4].state == POSSMOVE)
					board->cells[i][j+4].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa UP del PLAYER: POSSMOVE->SELMOVE
void MovePlayerUp(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i][j-2].state == POSSMOVE)
					board->cells[i][j-2].state = SELMOVE;
				else if(board->cells[i][j-4].state == POSSMOVE)
					board->cells[i][j-4].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa DOWNLEFT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerUpLeft(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i-2][j-2].state == POSSMOVE)
					board->cells[i-2][j-2].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa DOWNLEFT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerUpRight(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i+2][j-2].state == POSSMOVE)
					board->cells[i+2][j-2].state = SELMOVE;
				return;
			}
		}
	}
}

//Seleziona la mossa RIGHT del PLAYER: POSSMOVE->SELMOVE
void MovePlayerRight(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(board->cells[i+2][j].state == POSSMOVE)
					board->cells[i+2][j].state = SELMOVE;
				else if(board->cells[i+4][j].state == POSSMOVE)
					board->cells[i+4][j].state = SELMOVE;
				return;
			}
		}
	}
}

//Conferma la mossa selezionata dal PLAYER: PLAYER->MKEMPTY, SELMOVE->PLAYER
void ConfirmMovePlayer(Board* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				board->cells[i][j].state = MKEMPTY;
			}
		}
	}
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == SELMOVE){
				board->cells[i][j].state = PLAYER;
				new_x_pos = (i - 1) / 2;
				new_y_pos = (j - 1) / 2;
				return;
			}
		}
	}
}

//Conferma la mossa selezionata dal PLAYER su testBoard: PLAYER->MKEMPTY, SELMOVE->PLAYER
void testConfirmMovePlayer(testBoard* board, StateCell PLAYER){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				board->cells[i][j].state = EMPTY;
			}
		}
	}
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == SELMOVE){
				board->cells[i][j].state = PLAYER;
				return;
			}
		}
	}
}

//Muove il muro MOVING UP: MOVING->MOVEDAWAY
void MoveWallUp(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(j-2>0){
					if(board->cells[i][j].orWall == VERTICAL){
						board->cells[i][j+1].wallState = MOVEDAWAY;
						board->cells[i][j+2].wallState = MOVEDAWAY;
						board->cells[i][j-1].wallState = MOVING;
						board->cells[i][j-2].wallState = MOVING;
						return;
					}
					if(board->cells[i][j].orWall == HORIZONTAL){
						board->cells[i][j].wallState = MOVEDAWAY;
						board->cells[i+1][j].wallState = MOVEDAWAY;
						board->cells[i+2][j].wallState = MOVEDAWAY;
						board->cells[i][j-2].wallState = MOVING;
						board->cells[i+1][j-2].wallState = MOVING;
						board->cells[i+2][j-2].wallState = MOVING;
						return;
					}
				}
			}
		}
	}
}

//Muove il muro MOVING LEFT: MOVING->MOVEDAWAY
void MoveWallLeft(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(i-2>0){
					if(board->cells[i][j].orWall == VERTICAL){
						board->cells[i][j].wallState = MOVEDAWAY;
						board->cells[i][j+1].wallState = MOVEDAWAY;
						board->cells[i][j+2].wallState = MOVEDAWAY;
						board->cells[i-2][j].wallState = MOVING;
						board->cells[i-2][j+1].wallState = MOVING;
						board->cells[i-2][j+2].wallState = MOVING;
						return;
					}
					if(board->cells[i][j].orWall == HORIZONTAL){
						board->cells[i+1][j].wallState = MOVEDAWAY;
						board->cells[i+2][j].wallState = MOVEDAWAY;
						board->cells[i-1][j].wallState = MOVING;
						board->cells[i-2][j].wallState = MOVING;
						return;
					}
				}
			}
		}
	}
}

//Muove il muro MOVING RIGHT: MOVING->MOVEDAWAY
void MoveWallRight(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(i+2<DIM-1){
					if(board->cells[i][j].orWall == VERTICAL){
						board->cells[i][j].wallState = MOVEDAWAY;
						board->cells[i][j+1].wallState = MOVEDAWAY;
						board->cells[i][j+2].wallState = MOVEDAWAY;
						board->cells[i+2][j].wallState = MOVING;
						board->cells[i+2][j+1].wallState = MOVING;
						board->cells[i+2][j+2].wallState = MOVING;
						return;
					}
					if(board->cells[i][j].orWall == HORIZONTAL){
						if(i+4<DIM-1){
							board->cells[i][j].wallState = MOVEDAWAY;
							board->cells[i+1][j].wallState = MOVEDAWAY;
							board->cells[i+3][j].wallState = MOVING;
							board->cells[i+4][j].wallState = MOVING;
							return;
						}
					}
				}
			}
		}
	}
}

//Muove il muro MOVING DOWN: MOVING->MOVEDAWAY
void MoveWallDown(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(j+2<DIM-1){
					if(j+4<DIM-1){
						if(board->cells[i][j].orWall == VERTICAL){
							board->cells[i][j].wallState = MOVEDAWAY;
							board->cells[i][j+1].wallState = MOVEDAWAY;
							board->cells[i][j+3].wallState = MOVING;
							board->cells[i][j+4].wallState = MOVING;
							return;
						}
					}
					if(board->cells[i][j].orWall == HORIZONTAL){
						board->cells[i][j].wallState = MOVEDAWAY;
						board->cells[i+1][j].wallState = MOVEDAWAY;
						board->cells[i+2][j].wallState = MOVEDAWAY;
						board->cells[i][j+2].wallState = MOVING;
						board->cells[i+1][j+2].wallState = MOVING;
						board->cells[i+2][j+2].wallState = MOVING;
						return;
					}
				}
			}
		}
	}
}

//Ruota il muro di 90°: MOVING->MOVEDAWAY
void RotateMovingWall(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(board->cells[i][j].orWall == VERTICAL){
					board->cells[i][j].wallState = MOVEDAWAY;
					board->cells[i][j+2].wallState = MOVEDAWAY;
					board->cells[i-1][j+1].wallState = MOVING;
					board->cells[i+1][j+1].wallState = MOVING;
					return;
				}
				if(board->cells[i][j].orWall == HORIZONTAL){
					board->cells[i][j].wallState = MOVEDAWAY;
					board->cells[i+2][j].wallState = MOVEDAWAY;
					board->cells[i+1][j-1].wallState = MOVING;
					board->cells[i+1][j+1].wallState = MOVING;
					return;
				}
			}
		}
	}
}

//Unset delle mosse possibili sulla testBoard: POSSMOVE->EMPTY
void UnsetPossMoves(testBoard *board){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == POSSMOVE)
				board->cells[i][j].state = EMPTY;
		}
	}
}

//Unset delle mosse possibili sulla testBoard: POSSMOVE->EMPTY
void UnsetSelMoves(testBoard *board){
	int i, j;
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == SELMOVE){
				board->cells[i][j].state = POSSMOVE;
				return;
			}
		}
	}
}

//Unset dei muri in movimento: MOVING->NOTMOVING
void UnsetMovingWalls(Board *board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				board->cells[i][j].wallState = NOTMOVING;
			}
		}
	}
}

//Unset dei muri appena mossi: MOVEDAWAY->NOTMOVING
void UnsetMovedWalls(Board *board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVEDAWAY){
				board->cells[i][j].wallState = NOTMOVING;
			}
		}
	}
}

//Conferma del muro: MOVING->NOTMOVING, NOWALL->WALL
bool ConfirmMoveWall(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(board->cells[i][j].state == WALL || !valid_wall)
					return false;
			}
		}
	}
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(board->cells[i][j].orWall == HORIZONTAL){
					if(board->cells[i][j].state == NOWALL && board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == NOWALL){
						board->cells[i][j].wallState = NOTMOVING;
						board->cells[i+1][j].wallState = NOTMOVING;
						board->cells[i+2][j].wallState = NOTMOVING;
						board->cells[i][j].state = WALL;
						board->cells[i+1][j].state = WALL;
						board->cells[i+2][j].state = WALL;
						or_wall = 1;
						new_x_pos = (i - 1) / 2;
						new_y_pos = (j - 2) / 2;
						return true;
					}
				}
				if(board->cells[i][j].orWall == VERTICAL){
					if(board->cells[i][j].state == NOWALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == NOWALL){
						board->cells[i][j].wallState = NOTMOVING;
						board->cells[i][j+1].wallState = NOTMOVING;
						board->cells[i][j+2].wallState = NOTMOVING;
						board->cells[i][j].state = WALL;
						board->cells[i][j+1].state = WALL;
						board->cells[i][j+2].state = WALL;
						or_wall = 0;
						new_x_pos = (i - 2) / 2;
						new_y_pos = (j - 1) / 2;
						return true;
					}
				}
			}
		}
	}
	return false;
}

//Conferma del muro sulla Board: MOVING->NOTMOVING, NOWALL->WALL
void testConfirmMoveWall(Board* board){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			if(board->cells[i][j].wallState == MOVING){
				if(board->cells[i][j].orWall == HORIZONTAL){
					board->cells[i][j].wallState = NOTMOVING;
					board->cells[i+1][j].wallState = NOTMOVING;
					board->cells[i+2][j].wallState = NOTMOVING;
					board->cells[i][j].state = WALL;
					board->cells[i+1][j].state = WALL;
					board->cells[i+2][j].state = WALL;
					return;
				}
				if(board->cells[i][j].orWall == VERTICAL){
					board->cells[i][j].wallState = NOTMOVING;
					board->cells[i][j+1].wallState = NOTMOVING;
					board->cells[i][j+2].wallState = NOTMOVING;
					board->cells[i][j].state = WALL;
					board->cells[i][j+1].state = WALL;
					board->cells[i][j+2].state = WALL;
					return;
				}
			}
		}
	}
}

//Copia di una board su una testBoard
void copyBoard(testBoard* dest, Board* source){
	int i, j;
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM; j++)
			dest->cells[i][j].state = source->cells[i][j].state;
	}
}

//Algoritmo ricorsivo di ricerca delle mosse possibili di un PLAYER
bool explore_possMoves(testBoard* board, StateCell PLAYER){
	int i, j;
	bool winnable = false;
	testBoard copy;
	memcpy(&copy, board, sizeof(testBoard));
	
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == PLAYER){
				if(j==1 && PLAYER == PLAYER1)
					return true;
				if(j==13 && PLAYER == PLAYER2)
					return true;
				visited[(i-1)/2][(j-1)/2] = VISITED;
			}
		}
	}
	
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == POSSMOVE && visited[(i-1)/2][(j-1)/2] == NOTVISITED){
				(&copy)->cells[i][j].state = SELMOVE;
				UnsetPossMoves(&copy);
				testConfirmMovePlayer(&copy, PLAYER);
				testcalcPossMovesPlayer(&copy, PLAYER);
				winnable = explore_possMoves(&copy, PLAYER);
				if(winnable)
					return true;
			}
		}
	}
	return false;
}

//Funzione per verificare la validità del muro in movimento
bool CheckValidWall(Board* board){
	Board tcopy;
	testBoard copy;
	bool validP1, validP2;
	
	memcpy(&tcopy, board, sizeof(Board));
	testConfirmMoveWall(&tcopy);
	copyBoard(&copy, &tcopy);
	
	memset(visited, NOTVISITED, sizeof(visited));
	testcalcPossMovesPlayer(&copy, PLAYER1);
	validP1 = explore_possMoves(&copy, PLAYER1);
	memset(visited, NOTVISITED, sizeof(visited));
	UnsetPossMoves(&copy);
	testcalcPossMovesPlayer(&copy, PLAYER2);
	validP2 = explore_possMoves(&copy, PLAYER2);
	
	return (validP1 && validP2);
}

void MakeOpponentMove(Board* board, Move* opponentMove){
	int i, j;
	StateCell Opponent = (opponentMove->playerID == 0) ? PLAYER1 : PLAYER2;
	if(opponentMove->moveType == 0 && opponentMove->wallOrientation == 1 && opponentMove->newXPos == 0 && opponentMove->newXPos == 0)
		return;
	if(opponentMove->moveType == 0){
		for(i=1; i<DIM; i+=2){
			for(j=1; j<DIM; j+=2){
				if(board->cells[i][j].state == Opponent)
					board->cells[i][j].state = MKEMPTY;
			}
		}
		board->cells[2*(opponentMove->newXPos)+1][2*(opponentMove->newYPos)+1].state = Opponent;
	}
	if(opponentMove->moveType == 1){
		if(opponentMove->wallOrientation == 0){ //VERTICAL
			board->cells[2*(opponentMove->newXPos)+2][2*(opponentMove->newYPos)+1].wallState = MOVING;
			board->cells[2*(opponentMove->newXPos)+2][2*(opponentMove->newYPos)+2].wallState = MOVING;
			board->cells[2*(opponentMove->newXPos)+2][2*(opponentMove->newYPos)+3].wallState = MOVING;
		}
		if(opponentMove->wallOrientation == 1){ //HORIZONTAL
			board->cells[2*(opponentMove->newXPos)+1][2*(opponentMove->newYPos)+2].wallState = MOVING;
			board->cells[2*(opponentMove->newXPos)+2][2*(opponentMove->newYPos)+2].wallState = MOVING;
			board->cells[2*(opponentMove->newXPos)+3][2*(opponentMove->newYPos)+2].wallState = MOVING;
		}
	}
}

void bestPathNPC(testBoard* board, StateCell NPC, int *best_distance, int depth){
	int i, j, temp_bestPath;
	testBoard copy;
	for(i=0;i<DIM;i++){
		for(j=0;j<DIM;j++)
			(&copy)->cells[i][j].state = board->cells[i][j].state;
	}
		
	
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == NPC){
				score[(i-1)/2][(j-1)/2] = depth;
				if(j==1 && NPC == PLAYER1){
					*best_distance = (depth < *best_distance) ? depth : *best_distance;
					return;
				}
				if(j==13 && NPC == PLAYER2){
					*best_distance = (depth < *best_distance) ? depth : *best_distance;
					return;
				}
			}
		}
	}
	
	if(depth >= *best_distance)
		return;
	
	for(i=1; i<DIM; i+=2){
		for(j=1; j<DIM; j+=2){
			if(board->cells[i][j].state == POSSMOVE && (score[(i-1)/2][(j-1)/2] > depth+1 || score[(i-1)/2][(j-1)/2] == -1)){
				(&copy)->cells[i][j].state = SELMOVE;
				UnsetPossMoves(&copy);
				testConfirmMovePlayer(&copy, NPC);
				testcalcPossMovesPlayer(&copy, NPC);
				temp_bestPath = *best_distance;
				bestPathNPC(&copy, NPC, best_distance, depth+1);
				if(*best_distance < temp_bestPath && depth == 0){
					UnsetSelMoves(board);
					board->cells[i][j].state = SELMOVE;
				}
			}
		}
	}
	return;
}

void reset_testBoard(testBoard* board){
	UnsetSelMoves(board);
	UnsetPossMoves(board);
	memset(score, -1, sizeof(score));
}

void MakeNPCMove(Board* board, StateCell NPC){
	int i, j, choice, x_player, y_player, sel_wall_x = 0, sel_wall_y = 0;
	int val_NPC = 50, val_player = 50, distance = 50, temp_distNPC, temp_distP;
	int NPCWalls = (NPC == PLAYER1) ? P1Walls : P2Walls;
	StateCell player = (NPC == PLAYER2) ? PLAYER1 : PLAYER2;
	testBoard testNPC;
	copyBoard(&testNPC, board);
	memset(score, -1, sizeof(score));
	
	if(NPCWalls == 0)
		choice = 1;
	else if(NPCWalls > 0){
		if(NPC_mode == EASY)
			choice = rand()%2;
		else if(NPC_mode == HARD){
			testcalcPossMovesPlayer(&testNPC, player);
			bestPathNPC(&testNPC, player, &val_player, 0);
			reset_testBoard(&testNPC);
			testcalcPossMovesPlayer(&testNPC, NPC);
			bestPathNPC(&testNPC, NPC, &val_NPC, 0);
			
			if(val_player	< val_NPC){
				choice = 0;
				temp_distP = val_player;
				temp_distNPC = val_NPC;
			}
			else{
				choice = 1;
				val_NPC = 50;
				reset_testBoard(&testNPC);
			}
		}
	}
	if(choice == 0){
		mode = MOVEWALL;
		
		for(i=1; i<DIM; i+=2){
			for(j=1; j<DIM; j+=2){
				if(board->cells[i][j].state == player){
					x_player = i;
					y_player = j;
				}
			}
		}
		
		for(i=0; i<DIM; i++){
			for(j=0; j<DIM; j++){
				if(board->cells[i][j].state == NOWALL){
					if(board->cells[i][j].orWall == HORIZONTAL){
						if(board->cells[i+1][j].state == NOWALL && board->cells[i+2][j].state == NOWALL){
							board->cells[i][j].wallState = MOVING;
							board->cells[i+1][j].wallState = MOVING;
							board->cells[i+2][j].wallState = MOVING;
							if(CheckValidWall(board)){
								if(NPC_mode == EASY){
									if((abs(i-x_player)+abs(j-y_player)) < distance){
										distance = abs(i-x_player)+abs(j-y_player);
										sel_wall_x = i;
										sel_wall_y = j;
									}
									else if((abs(i-x_player)+abs(j-y_player)) == distance && 
													((j > y_player && sel_wall_y-y_player > j-y_player && player == PLAYER2) || 
														(j < y_player && y_player-((sel_wall_y == y_player) ? -2 : sel_wall_y) > y_player-j && player == PLAYER1))){
										distance = abs(i-x_player)+abs(j-y_player);
										sel_wall_x = i;
										sel_wall_y = j;
									}
								}
								else if(NPC_mode == HARD){
									val_player = 50;
									val_NPC = 50;
									
									reset_testBoard(&testNPC);
									(&testNPC)->cells[i][j].state = WALL;
									(&testNPC)->cells[i+1][j].state = WALL;
									(&testNPC)->cells[i+2][j].state = WALL;
			
									testcalcPossMovesPlayer(&testNPC, player);
									bestPathNPC(&testNPC, player, &val_player, 0);
									reset_testBoard(&testNPC);
									testcalcPossMovesPlayer(&testNPC, NPC);
									bestPathNPC(&testNPC, NPC, &val_NPC, 0);
									
									(&testNPC)->cells[i][j].state = NOWALL;
									(&testNPC)->cells[i+1][j].state = NOWALL;
									(&testNPC)->cells[i+2][j].state = NOWALL;
									if(val_player > temp_distP && (val_NPC == temp_distNPC || val_NPC < val_player)){
										sel_wall_x = i;
										sel_wall_y = j;
										temp_distP = val_player;
									}
								}
							}
							board->cells[i][j].wallState = NOTMOVING;
							board->cells[i+1][j].wallState = NOTMOVING;
							board->cells[i+2][j].wallState = NOTMOVING;
						}
					}
					if(board->cells[i][j].orWall == VERTICAL){
						if(board->cells[i][j].state == NOWALL && board->cells[i][j+1].state == NOWALL && board->cells[i][j+2].state == NOWALL){
							board->cells[i][j].wallState = MOVING;
							board->cells[i][j+1].wallState = MOVING;
							board->cells[i][j+2].wallState = MOVING;
							if(CheckValidWall(board)){
								if(NPC_mode == EASY && (abs(i-x_player)+abs(j-y_player)) < distance){
									distance = abs(i-x_player)+abs(j-y_player);
									sel_wall_x = i;
									sel_wall_y = j;
								}
								else if(NPC_mode == HARD){
									val_player = 50;
									val_NPC = 50;
									
									reset_testBoard(&testNPC);
									(&testNPC)->cells[i][j].state = WALL;
									(&testNPC)->cells[i][j+1].state = WALL;
									(&testNPC)->cells[i][j+2].state = WALL;
			
									testcalcPossMovesPlayer(&testNPC, player);
									bestPathNPC(&testNPC, player, &val_player, 0);
									reset_testBoard(&testNPC);
									testcalcPossMovesPlayer(&testNPC, NPC);
									bestPathNPC(&testNPC, NPC, &val_NPC, 0);
									
									(&testNPC)->cells[i][j].state = NOWALL;
									(&testNPC)->cells[i][j+1].state = NOWALL;
									(&testNPC)->cells[i][j+2].state = NOWALL;
									if(val_player > temp_distP && (val_NPC == temp_distNPC || val_NPC < val_player)){
										sel_wall_x = i;
										sel_wall_y = j;
										temp_distP = val_player;
									}
								}
							}
							board->cells[i][j].wallState = NOTMOVING;
							board->cells[i][j+1].wallState = NOTMOVING;
							board->cells[i][j+2].wallState = NOTMOVING;
						}
					}
				}
			}
		}
		if(sel_wall_x == 0 && sel_wall_y == 0){
			choice = 1;
			val_NPC = 50;
			reset_testBoard(&testNPC);
		}
		else{
			if(board->cells[sel_wall_x][sel_wall_y].orWall == HORIZONTAL){
				board->cells[sel_wall_x][sel_wall_y].wallState = MOVING;
				board->cells[sel_wall_x+1][sel_wall_y].wallState = MOVING;
				board->cells[sel_wall_x+2][sel_wall_y].wallState = MOVING;
				or_wall = 1;
				new_x_pos = (sel_wall_x - 1) / 2;
				new_y_pos = (sel_wall_y - 2) / 2;
			}
			else if(board->cells[sel_wall_x][sel_wall_y].orWall == VERTICAL){
				board->cells[sel_wall_x][sel_wall_y].wallState = MOVING;
				board->cells[sel_wall_x][sel_wall_y+1].wallState = MOVING;
				board->cells[sel_wall_x][sel_wall_y+2].wallState = MOVING;
				or_wall = 0;
				new_x_pos = (sel_wall_x - 2) / 2;
				new_y_pos = (sel_wall_y - 1) / 2;
			}
		}
	}
	if(choice == 1){
		mode = MOVEPLAYER;
		testcalcPossMovesPlayer(&testNPC, NPC);
		bestPathNPC(&testNPC, NPC, &val_NPC, 0);
		for(i=1; i<DIM; i+=2){
			for(j=1; j<DIM; j+=2){
				if((&testNPC)->cells[i][j].state == SELMOVE){
					board->cells[i][j].state = SELMOVE;
					new_x_pos = (i - 1) / 2;
					new_y_pos = (j - 1) / 2;
				}
			}
		}
		ConfirmMovePlayer(board, NPC);
	}
}

