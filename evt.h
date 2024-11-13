#ifndef _EVT_H_     
#define _EVT_H_    
 
 
/*------------------------------- Includes ----------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "evt_sys.h"

/*----------------------------- Global Defines ------------------------------*/

/*----------------------------- Global Typedefs -----------------------------*/
typedef enum
{
    //模块ID
    EVT_SYS,//系统模块
    

    EVT_MODEULE_NUM//模块数量
}evt_module_e;

void evt_config_init();

#endif //
