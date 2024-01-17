/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h"

extern unsigned int timer;
extern unsigned int cnt_tim2;


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	if(timer != 0){
		timer--;
		LCD_SetTimer(timer, Gold, RichGreen);
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
	if(cnt_tim2 != 0)
		cnt_tim2--;
	if(cnt_tim2 == 50){
		GUI_Text(40, 260, (uint8_t *) "Press INT0 to restart", Gold, RichGreen);
		NVIC_EnableIRQ(EINT0_IRQn);
		LPC_PINCON->PINSEL4    |= (1 << 20);
	}
	if((cnt_tim2-1)%3 == 2){
		GUI_Text(160, 160, (uint8_t *) ".", Gold, RichGreen);
		GUI_Text(168, 160, (uint8_t *) ".", RichGreen, RichGreen);
		GUI_Text(176, 160, (uint8_t *) ".", RichGreen, RichGreen);
	}
	if((cnt_tim2-1)%3 == 1)
		GUI_Text(168, 160, (uint8_t *) ".", Gold, RichGreen);
	if((cnt_tim2-1)%3 == 0)
		GUI_Text(176, 160, (uint8_t *) ".", Gold, RichGreen);
	
	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
