/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include <stdio.h>
#include <string.h>

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

Mode mode;
unsigned int turn = 0;
unsigned int timer;
unsigned int P1Walls;
unsigned int P2Walls;
unsigned int cnt_tim2;

bool wall_warning;
bool opponent_ready;
bool int0_pressed;

BoardType selectedBoardType;
GameMode selectedGameMode;

Board mainBoard;
StateCell CurrentPlayer = UNKNOWN;
StateCell OpponentPlayer = UNKNOWN;
Move playerMove;
Move opponentMove;

uint32_t playerMoveUint;
uint32_t opponentMoveUint;
extern uint8_t or_wall;
extern uint8_t new_x_pos;
extern uint8_t new_y_pos;


void StartGame(uint16_t Color, uint16_t bkColor) {
	selectedBoardType = None;
	selectedGameMode = NA;
	turn = 0;
	P1Walls = 8;
	P2Walls = 8;
	wall_warning = false;
	opponent_ready = false;
	int0_pressed = false;
	opponentMoveUint = 0xFF000000;
	CurrentPlayer = UNKNOWN;
	OpponentPlayer = UNKNOWN;
	mode = FROZEN;
	
	LCD_Clear(bkColor);
	GUI_Text(74, 120, (uint8_t *) " Press INT0 ", Color, bkColor);
	GUI_Text(46, 150, (uint8_t *) " to start the game ", Color, bkColor);
	
	NVIC_EnableIRQ(EINT0_IRQn);
	LPC_PINCON->PINSEL4    |= (1 << 20);
}

void EndGame(StateCell Winner, uint16_t Color, uint16_t bkColor){
	turn = 0;
	P1Walls = 8;
	P2Walls = 8;
	wall_warning = false;
	opponent_ready = false;
	int0_pressed = false;
	opponentMoveUint = 0x0;
	disable_timer(0);
	LCD_Clear(bkColor);
	
	if((selectedBoardType == TwoBoard && selectedGameMode == Human) || selectedGameMode == NPC){
		if(Winner == CurrentPlayer)
			GUI_Text(95, 120, (uint8_t *) "YOU WON", Color, bkColor);
		if(Winner != CurrentPlayer)
			GUI_Text(92, 120, (uint8_t *) "YOU LOST", Color, bkColor);
	}
	
	if(selectedBoardType == SingleBoard && selectedGameMode == Human){
		if(Winner == PLAYER1)
			GUI_Text(65, 120, (uint8_t *) " PLAYER 1 WON ", Color, bkColor);
		if(Winner == PLAYER2)
			GUI_Text(65, 120, (uint8_t *) " PLAYER 2 WON ", Color, bkColor);
	}
	
	selectedBoardType = None;
	selectedGameMode = NA;
	CurrentPlayer = UNKNOWN;
	OpponentPlayer = UNKNOWN;
	mode = FROZEN;
	resetBoard(&mainBoard);
	
	GUI_Text(74, 150, (uint8_t *) " Press INT0 ", Color, bkColor);
	GUI_Text(38, 180, (uint8_t *) " to restart the game ", Color, bkColor);
	
	NVIC_EnableIRQ(EINT0_IRQn);
	LPC_PINCON->PINSEL4    |= (1 << 20);
}

void Game_SingleBoard_Human(void){
	if(CheckWin(&mainBoard, PLAYER1))
		EndGame(PLAYER1, Gold, RichGreen);
	else if(CheckWin(&mainBoard, PLAYER2))
		EndGame(PLAYER2, Gold, RichGreen);
	else{
		NVIC_EnableIRQ(EINT1_IRQn);
		LPC_PINCON->PINSEL4    |= (1 << 22);
		mode = MOVEPLAYER;
		
		timer = 20;
		LCD_SetTimer(timer--, Gold, RichGreen);
		LCD_SetTimer(timer--, Gold, RichGreen);
		
		CurrentPlayer = (turn == 0) ? PLAYER1 : PLAYER2;
		calcPossMovesPlayer(&mainBoard, CurrentPlayer);
		LCD_ShowPossMoves(&mainBoard);
		
		while (timer > 0)
			enable_timer(0);
		
		if(mode==MOVEWALL){
			LCD_ClearMovingWalls(&mainBoard);
			UnsetMovingWalls(&mainBoard);
		}
		if(mode==MOVEPLAYER){
			LCD_ClearPossMoves(&mainBoard);
			LCD_ClearSelMove(&mainBoard);
			LCD_MakeEmpty(&mainBoard);
			LCD_DrawPlayer(&mainBoard, CurrentPlayer);
		}
		if(wall_warning){
			LCD_WallWarning(RichGreen, RichGreen);
			wall_warning = false;
		}
		
		NVIC_EnableIRQ(EINT1_IRQn);
		LPC_PINCON->PINSEL4    |= (1 << 22);
		NVIC_DisableIRQ(EINT2_IRQn);
		LPC_PINCON->PINSEL4 &= ~(1 << 24);
		turn = (turn == 0) ? 1 : 0;
	}
}

void Game_TwoBoard_Human(void){	
	if((turn == 0 && CurrentPlayer == PLAYER1) || (turn == 1 && CurrentPlayer == PLAYER2)){
		ResetMove(&playerMove, CurrentPlayer);
		
		if(CheckWin(&mainBoard, OpponentPlayer))
			EndGame(OpponentPlayer, Gold, RichGreen);
		else{
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 22);
			mode = MOVEPLAYER;
			
			timer = 20;
			LCD_SetTimer(timer--, Gold, RichGreen);
			LCD_SetTimer(timer--, Gold, RichGreen);
			
			calcPossMovesPlayer(&mainBoard, CurrentPlayer);
			LCD_ShowPossMoves(&mainBoard);
			
			while (timer > 0)
				enable_timer(0);
			
			if(mode==MOVEWALL){
				LCD_ClearMovingWalls(&mainBoard);
				UnsetMovingWalls(&mainBoard);
			}
			if(mode==MOVEPLAYER){
				LCD_ClearPossMoves(&mainBoard);
				LCD_ClearSelMove(&mainBoard);
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, CurrentPlayer);
			}
			if(wall_warning){
				LCD_WallWarning(RichGreen, RichGreen);
				wall_warning = false;
			}
			
			playerMoveUint = createMoveUint(&playerMove);
			SendMove(playerMoveUint);
			
			opponentMoveUint = 0x0;
			NVIC_DisableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4 &= ~(1 << 22);
			NVIC_DisableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4 &= ~(1 << 24);
			turn = (turn == 0) ? 1 : 0;
		}
	}
	if((turn == 0 && CurrentPlayer == PLAYER2) || (turn == 1 && CurrentPlayer == PLAYER1)){
		if(CheckWin(&mainBoard, CurrentPlayer))
			EndGame(CurrentPlayer, Gold, RichGreen);
		else{
			
			timer = 20;
			LCD_SetTimer(timer--, Gold, RichGreen);
			LCD_SetTimer(timer--, Gold, RichGreen);
			
			while(opponentMoveUint == 0x0 || opponentMoveUint == 0xFF000000)
				enable_timer(0);
			
			opponentMove = getMoveFromUint(opponentMoveUint);
						
			MakeOpponentMove(&mainBoard, &opponentMove);
			if((&opponentMove)->moveType == 0 && (&opponentMove)->wallOrientation != 1){
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, OpponentPlayer);
			}
			if((&opponentMove)->moveType == 1){
				LCD_DrawWallNPC(&mainBoard, Brown);
				LCD_updatePlayerWalls(OpponentPlayer, Gold, RichGreen);
			}
			turn = (turn == 0) ? 1 : 0;
		}
	}
}

void Game_SingleBoard_NPC(void){
	enable_timer(1);
	if((turn == 0 && CurrentPlayer == PLAYER1) || (turn == 1 && CurrentPlayer == PLAYER2)){
		if(CheckWin(&mainBoard, OpponentPlayer))
			EndGame(OpponentPlayer, Gold, RichGreen);
		else{
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 22);
			mode = MOVEPLAYER;
			
			timer = 20;
			LCD_SetTimer(timer, Gold, RichGreen);
			LCD_SetTimer(--timer, Gold, RichGreen);
			
			calcPossMovesPlayer(&mainBoard, CurrentPlayer);
			LCD_ShowPossMoves(&mainBoard);
			
			while (timer > 0)
				enable_timer(0);
			
			if(mode==MOVEWALL){
				LCD_ClearMovingWalls(&mainBoard);
				UnsetMovingWalls(&mainBoard);
			}
			if(mode==MOVEPLAYER){
				LCD_ClearPossMoves(&mainBoard);
				LCD_ClearSelMove(&mainBoard);
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, CurrentPlayer);
			}
			if(wall_warning){
				LCD_WallWarning(RichGreen, RichGreen);
				wall_warning = false;
			}
			
			NVIC_DisableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4 &= ~(1 << 22);
			NVIC_DisableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4 &= ~(1 << 24);
			turn = (turn == 0) ? 1 : 0;
		}
	}
	if((turn == 0 && CurrentPlayer == PLAYER2) || (turn == 1 && CurrentPlayer == PLAYER1)){
		if(CheckWin(&mainBoard, CurrentPlayer))
			EndGame(CurrentPlayer, Gold, RichGreen);
		else{
			srand(LPC_TIM1->TC);
			MakeNPCMove(&mainBoard, OpponentPlayer);
			if(mode == MOVEPLAYER){
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, OpponentPlayer);
			}
			if(mode == MOVEWALL){
				LCD_DrawWallNPC(&mainBoard, Brown);
				LCD_updatePlayerWalls(OpponentPlayer, Gold, RichGreen);
			}
			turn = (turn == 0) ? 1 : 0;
		}
	}
	disable_timer(1);
}

void Game_TwoBoard_NPC(void){
	enable_timer(1);
	if((turn == 0 && CurrentPlayer == PLAYER1) || (turn == 1 && CurrentPlayer == PLAYER2)){
		if(CheckWin(&mainBoard, OpponentPlayer))
			EndGame(OpponentPlayer, Gold, RichGreen);
		else{
			srand(LPC_TIM1->TC);
			MakeNPCMove(&mainBoard, CurrentPlayer);
			if(mode == MOVEPLAYER){
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, CurrentPlayer);
				(&playerMove)->playerID = (CurrentPlayer == PLAYER1) ? 0 : 1;
				(&playerMove)->moveType = 0;
				(&playerMove)->wallOrientation = 0;
				(&playerMove)->newXPos = new_x_pos;
				(&playerMove)->newYPos = new_y_pos;
			}
			if(mode == MOVEWALL){
				LCD_DrawWallNPC(&mainBoard, Brown);
				LCD_updatePlayerWalls(CurrentPlayer, Gold, RichGreen);
				(&playerMove)->playerID = (CurrentPlayer == PLAYER1) ? 0 : 1;						
				(&playerMove)->moveType = 1;
				(&playerMove)->wallOrientation = or_wall;
				(&playerMove)->newXPos = new_x_pos;
				(&playerMove)->newYPos = new_y_pos;
			}
			playerMoveUint = createMoveUint(&playerMove);
			SendMove(playerMoveUint);
			opponentMoveUint = 0x0;
			
			turn = (turn == 0) ? 1 : 0;
		}
	}
	if((turn == 0 && CurrentPlayer == PLAYER2) || (turn == 1 && CurrentPlayer == PLAYER1)){
		if(CheckWin(&mainBoard, CurrentPlayer))
			EndGame(CurrentPlayer, Gold, RichGreen);
		else{
			timer = 20;
			LCD_SetTimer(timer--, Gold, RichGreen);
			LCD_SetTimer(timer--, Gold, RichGreen);
			
			while(opponentMoveUint == 0x0 || opponentMoveUint == 0xAA000000)
				enable_timer(0);
			
			opponentMove = getMoveFromUint(opponentMoveUint);
						
			MakeOpponentMove(&mainBoard, &opponentMove);
			if((&opponentMove)->moveType == 0 && (&opponentMove)->wallOrientation != 1){
				LCD_MakeEmpty(&mainBoard);
				LCD_DrawPlayer(&mainBoard, OpponentPlayer);
			}
			if((&opponentMove)->moveType == 1){
				LCD_DrawWallNPC(&mainBoard, Brown);
				LCD_updatePlayerWalls(OpponentPlayer, Gold, RichGreen);
			}
			turn = (turn == 0) ? 1 : 0;
		}
	}
	disable_timer(1);
}


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	CAN_Init();														/* CAN Initialization              */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/*RIT Initialization 50 msec          */
	init_timer(0, 0x017D7840);						/*Timer 0 initialized to 1 sec        */
	init_timer(1, 0x3B9ACA00);						/*Timer 1 initialized      */
	init_timer(2, 0x00BEBC20);						/*Timer 2 initialized to 0.5 sec      */
	LCD_Initialization();
	enable_RIT();
	NVIC_DisableIRQ(EINT0_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 20);
	NVIC_DisableIRQ(EINT1_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 22);
	NVIC_DisableIRQ(EINT2_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 24);

	StartGame(Gold, RichGreen);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);
	
  while (1)	
  {
		__ASM("wfi");
		if(mode == MENU){
			int0_pressed = false;
			LCD_DrawMenuBoardType(Gold, RichGreen);
			while(selectedBoardType == None);
			LCD_DrawMenuGameMode(selectedBoardType, Gold, RichGreen);
			while(selectedGameMode == NA);
			if(selectedBoardType == SingleBoard)
				StartMatch(&mainBoard);
			if(selectedBoardType == TwoBoard && selectedGameMode != NA){
				SendHandshake();
				if(!opponent_ready)
					LCD_WaitingRoom(OpponentPlayer, Gold, RichGreen);
				cnt_tim2 = 60;
				while(!opponent_ready && !int0_pressed && cnt_tim2 != 0)
					enable_timer(2);
				disable_timer(2);
				if(opponent_ready)
					StartMatch(&mainBoard);
				if(int0_pressed || cnt_tim2 == 0){
					selectedBoardType = None;
					selectedGameMode = NA;
					int0_pressed = false;
				}
			}
		}
		else{
			if(selectedBoardType == TwoBoard){
				if(selectedGameMode == Human)
					Game_TwoBoard_Human();
				if(selectedGameMode == NPC)
					Game_TwoBoard_NPC();
			}
			if(selectedBoardType == SingleBoard){
				if(selectedGameMode == Human)
					Game_SingleBoard_Human();
				if(selectedGameMode == NPC)
					Game_SingleBoard_NPC();
			}
		}
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
