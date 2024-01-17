/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include <stdbool.h>

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down_int0=0;
volatile int down_key1=0;
volatile int down_key2=0;

volatile uint8_t or_wall;
volatile uint8_t new_x_pos;
volatile uint8_t new_y_pos;

extern unsigned int turn;
extern unsigned int timer;
extern unsigned int P1Walls;
extern unsigned int P2Walls;

extern Board mainBoard;
extern StateCell CurrentPlayer;
extern StateCell OpponentPlayer;
extern Move playerMove;
extern BoardType selectedBoardType;
extern GameMode selectedGameMode;

extern Mode mode;
extern uint32_t opponentMoveUint;

void RIT_IRQHandler (void)
{					
	static int J_select=0;
	static int J_down = 0;
	static int J_right = 0;
	static int J_left = 0;
	static int J_up = 0;
	static int J_down_left = 0;
	static int J_down_right = 0;
	static int J_up_left = 0;
	static int J_up_right = 0;
	static bool move_selected = false;
	static bool menu_sel_down = false;
	static bool menu_sel_up = false;
	static bool multiplayer_warning = false;
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		
		J_select++;
		switch(J_select){
			case 1:
				if(mode == MENU){
					if(selectedBoardType != None){
						if(menu_sel_up){
							LPC_RIT->RICTRL |= 0x1;
							selectedGameMode = Human;
						}
						if(menu_sel_down){
							LPC_RIT->RICTRL |= 0x1;
							selectedGameMode = NPC;
							if(selectedBoardType == SingleBoard){
								CurrentPlayer = PLAYER1;
								OpponentPlayer = PLAYER2;
							}
						}
						menu_sel_down = false;
						menu_sel_up = false;
						mode = MOVEPLAYER;
					}
					if(selectedBoardType == None){
						if(menu_sel_up){
							if(multiplayer_warning){
								multiplayer_warning = false;
								LCD_MultiplayerWarning(RichGreen, RichGreen);
							}
							selectedBoardType = SingleBoard;
						}
						if(menu_sel_down){
							if(opponentMoveUint == 0x0){
								multiplayer_warning = true;
								LCD_MultiplayerWarning(Red, RichGreen);
							}
							else
								selectedBoardType = TwoBoard;
						}
						menu_sel_down = (multiplayer_warning) ? true : false;
						menu_sel_up = false;
					}
				}
				if(mode == MOVEPLAYER){
					if(selectedBoardType == SingleBoard){
						if(selectedGameMode == Human){
							if(move_selected){
								LCD_ClearSelMove(&mainBoard);
								ConfirmMovePlayer(&mainBoard, CurrentPlayer);
								
								move_selected = false;
								disable_timer(0);
								timer = 0;
							}
						}
						if(selectedGameMode == NPC){
							if(move_selected){
								LCD_ClearSelMove(&mainBoard);
								ConfirmMovePlayer(&mainBoard, CurrentPlayer);
								
								move_selected = false;
								disable_timer(0);
								timer = 0;
							}
						}
					}
					if(selectedBoardType == TwoBoard){
						if(selectedGameMode == Human){
							if(move_selected){
								LCD_ClearSelMove(&mainBoard);
								ConfirmMovePlayer(&mainBoard, CurrentPlayer);
								(&playerMove)->playerID = (CurrentPlayer == PLAYER1) ? 0 : 1;
								(&playerMove)->moveType = 0;
								(&playerMove)->wallOrientation = 0;
								(&playerMove)->newXPos = new_x_pos;
								(&playerMove)->newYPos = new_y_pos;
								
								move_selected = false;
								disable_timer(0);
								timer = 0;
							}
						}
					}
				}
				if(mode == MOVEWALL){
					if(selectedBoardType == SingleBoard){
						if(selectedGameMode == Human){
							move_selected = false;
							if(ConfirmMoveWall(&mainBoard)){
								LCD_updatePlayerWalls(CurrentPlayer, Gold, RichGreen);
										
								disable_timer(0);
								timer = 0;
							}
						}
						if(selectedGameMode == NPC){
							move_selected = false;
							if(ConfirmMoveWall(&mainBoard)){
								LCD_updatePlayerWalls(CurrentPlayer, Gold, RichGreen);
								
								disable_timer(0);
								timer = 0;
							}
						}
					}
					if(selectedBoardType == TwoBoard){
						if(selectedGameMode == Human){
							move_selected = false;
							if(ConfirmMoveWall(&mainBoard)){
								LCD_updatePlayerWalls(CurrentPlayer, Gold, RichGreen);
								(&playerMove)->playerID = (CurrentPlayer == PLAYER1) ? 0 : 1;						
								(&playerMove)->moveType = 1;
								(&playerMove)->wallOrientation = or_wall;
								(&playerMove)->newXPos = new_x_pos;
								(&playerMove)->newYPos = new_y_pos;
								
								disable_timer(0);
								timer = 0;
							}
						}
					}
					
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_select=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0){
		
		J_down++;
		switch(J_down){
			case 1:
				if(mode == MENU){
					if(selectedBoardType == None){
						if(menu_sel_up){
							menu_sel_up = false;
							LCD_DrawSingleBoardBox(Gold, RichGreen);
						}
						LCD_DrawTwoBoardBox(Red, RichGreen);
					}
					if(selectedBoardType != None){
						if(menu_sel_up){
							menu_sel_up = false;
							LCD_DrawHumanBox(Gold, RichGreen);
						}
						LCD_DrawNPCBox(Red, RichGreen);
					}
					menu_sel_down = true;
				}
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerDown(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				if(mode == MOVEWALL){
					MoveWallDown(&mainBoard);
					LCD_DrawMovingWall(&mainBoard, Brown);
					LCD_ClearMovedWall(&mainBoard);
					UnsetMovedWalls(&mainBoard);
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_down=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){
		
		J_down_left++;
		switch(J_down_left){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerDownLeft(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_down_left=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){
		
		J_down_right++;
		switch(J_down_right){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerDownRight(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_down_right=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0){	
		J_left++;
		switch(J_left){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerLeft(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				if(mode == MOVEWALL){
					MoveWallLeft(&mainBoard);
					LCD_DrawMovingWall(&mainBoard, Brown);
					LCD_ClearMovedWall(&mainBoard);
					UnsetMovedWalls(&mainBoard);
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0){	
		J_right++;
		switch(J_right){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerRight(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				if(mode == MOVEWALL){
					MoveWallRight(&mainBoard);
					LCD_DrawMovingWall(&mainBoard, Brown);
					LCD_ClearMovedWall(&mainBoard);
					UnsetMovedWalls(&mainBoard);
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_right=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0){
		J_up++;
		switch(J_up){
			case 1:
				if(mode == MENU){
					if(selectedBoardType == None){
						if(menu_sel_down){
							menu_sel_down = false;
							LCD_DrawTwoBoardBox(Gold, RichGreen);
						}
						LCD_DrawSingleBoardBox(Red, RichGreen);
					}
					if(selectedBoardType != None){
						if(menu_sel_down){
							menu_sel_down = false;
							LCD_DrawNPCBox(Gold, RichGreen);
						}
						LCD_DrawHumanBox(Red, RichGreen);
					}
					menu_sel_up = true;
				}
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerUp(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				if(mode == MOVEWALL){
					MoveWallUp(&mainBoard);
					LCD_DrawMovingWall(&mainBoard, Brown);
					LCD_ClearMovedWall(&mainBoard);
					UnsetMovedWalls(&mainBoard);
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_up=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){
		
		J_up_left++;
		switch(J_up_left){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerUpLeft(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_up_left=0;
	}
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){
		
		J_up_right++;
		switch(J_up_right){
			case 1:
				if(mode == MOVEPLAYER){
					LCD_DeselMove(&mainBoard);
					MovePlayerUpRight(&mainBoard, CurrentPlayer);
					LCD_ShowSelMove(&mainBoard);
					move_selected = true;
				}
				break; 
			default:
				break;
		}
	}
	else{
			J_up_right=0;
	}
	
	/* button management */
	if(down_int0!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0)  /* INT0 pressed */
			down_int0++;
		else {	/* button released */
			down_int0=0;
			NVIC_EnableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			NVIC_DisableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4 &= ~(1 << 20);
		}
	}
	
	if(down_key1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0)  /* KEY1 pressed */
			down_key1++;
		else {	/* button released */
			down_key1=0;
			NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_key2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0)	/* KEY2 pressed */
			down_key2++;
		else {	/* button released */
			down_key2=0;
			NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
