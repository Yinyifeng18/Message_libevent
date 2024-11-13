/******************************************************************************
* Copyright 2016-2021 Leelen.co
* FileName: 	 event.c 
* Desc:
* 
* 
* Author: 	 Liu Yongfei
* Date: 	 2017/03/09
* Notes: 
* 
* -----------------------------------------------------------------
* Histroy: v1.0   2017/03/09, Liu Yongfei create this file
* 
******************************************************************************/
 
 
/*------------------------------- Includes ----------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "event.h"
#include "evt.h"

/*------------------- Global Definitions and Declarations -------------------*/
 
/*---------------------- Constant / Macro Definitions -----------------------*/
 
 
/*----------------------- Type Declarations ---------------------------------*/



/*----------------------- Variable Declarations -----------------------------*/

/*----------------------- Function Prototype --------------------------------*/
 
 
/*----------------------- Function Implement --------------------------------*/

void evt_config_init()
{
    event_reg(EVT_SYS, EVT_SYS_MAX_ID); 
}






