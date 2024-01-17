#include "button.h"
#include "lpc17xx.h"
#include <string.h>
#include "../CAN/CAN.h"
#include "../timer/timer.h"

extern int down_int0;
extern int down_key1;
extern int down_key2;

extern Board mainBoard;
extern unsigned int P1Walls;
extern unsigned int P2Walls;
extern StateCell CurrentPlayer;
extern StateCell OpponentPlayer;

extern bool wall_warning;
extern bool int0_pressed;

extern Mode mode;
extern BoardType selectedBoardType;
extern GameMode selectedGameMode;

extern uint32_t opponentMoveUint;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	NVIC_DisableIRQ(EINT0_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 20);
	down_int0=1;
	int0_pressed = true;
	
	if(opponentMoveUint == 0x0){
		CurrentPlayer = PLAYER1;
		OpponentPlayer = PLAYER2;
	}
	if(opponentMoveUint == 0xFF000000){
		CurrentPlayer = PLAYER2;
		OpponentPlayer = PLAYER1;
	}
	SendPing();
	mode = MENU;
	
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	int playerWalls = (CurrentPlayer == PLAYER1) ? P1Walls : P2Walls;
	NVIC_DisableIRQ(EINT1_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 22);
	down_key1=1;
	mode = (mode == MOVEPLAYER)? MOVEWALL : MOVEPLAYER;
	
	if(mode == MOVEPLAYER){
		NVIC_DisableIRQ(EINT2_IRQn);
		LPC_PINCON->PINSEL4 &= ~(1 << 24);
		
		LCD_ClearMovingWalls(&mainBoard);
		UnsetMovingWalls(&mainBoard);
		calcPossMovesPlayer(&mainBoard, CurrentPlayer);
		LCD_ShowPossMoves(&mainBoard);
	}
	if(mode == MOVEWALL){
		wall_warning = (playerWalls == 0) ? true : false;
		if(wall_warning){
			LCD_WallWarning(Gold, RichGreen);
			mode = MOVEPLAYER;
		}
		if(!wall_warning){
			LCD_DeselMove(&mainBoard);
			LCD_ClearPossMoves(&mainBoard);
			(&mainBoard)->cells[8][7].wallState = MOVING;
			(&mainBoard)->cells[8][8].wallState = MOVING;
			(&mainBoard)->cells[8][9].wallState = MOVING;
			LCD_DrawMovingWall(&mainBoard, Brown);
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 24);
		}
	}
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 24);
	down_key2=1;
	RotateMovingWall(&mainBoard);
	LCD_DrawMovingWall(&mainBoard, Brown);
	LCD_ClearMovedWall(&mainBoard);
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


