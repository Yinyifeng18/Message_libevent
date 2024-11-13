#ifndef _EVENT_H_     
#define _EVENT_H_    
 
 
/*------------------------------- Includes ----------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <event2/event.h>
#include "evt.h"
/*----------------------------- Global Defines ------------------------------*/



/*----------------------------- Global Typedefs -----------------------------*/



typedef struct _evt_node_t
{
    evt_module_e module_id;
    uint32_t event_id;
    void (*evt_cb)(struct _evt_node_t *evt_node,void *p);
    void *p;//用户参数 可以为NULL
}evt_node_t;

typedef void (*evt_func_cb)(evt_node_t *evt_node,void *p);
/*----------------------------- External Variables --------------------------*/
 

/*------------------------ Global Function Prototypes -----------------------*/

/*******************************************************************
 ** 函数名:       event_init
 ** 函数描述:    事件管理模块初始化函数
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void event_init(struct event_base *base);//事件管理模块初始化函数

/*******************************************************************
 ** 函数名:       event_reg
 ** 函数描述:    事件注册函数
 ** 参数:       module_id:模块ID
                event_num:该模块事件数量
 ** 返回:       -1:失败，0：成功
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_reg(evt_module_e module_id,uint32_t event_num);//事件注册函数

/*******************************************************************
 ** 函数名:       event_listen
 ** 函数描述:    事件监听函数注册
 ** 参数:        listen_node:监听结构体参数
 ** 返回:        -1:失败           0:成功  
 ** 注意:    listen_node为用户自行malloc生成和维护,不建议修改，
             取消事件监听后自行释放
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_listen(evt_node_t *listen_node);//事件监听函数注册

/*******************************************************************
 ** 函数名:       event_unlisten
 ** 函数描述:    取消事件监听
 ** 参数:       listen_node:监听结构体参数
 ** 返回:       -1:失败           0:成功
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_unlisten(evt_node_t *listen_node);

/*******************************************************************
 ** 函数名:       event_trigger
 ** 函数描述:    事件触发函数
 ** 参数:
 ** 返回:       -1:失败，>=0:监听数量
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_trigger(evt_module_e module_id,uint32_t event_id,void*arg);//事件触发函数

/*******************************************************************
 ** 函数名:       event_delay_trigger
 ** 函数描述:    事件延迟触发函数
 ** 参数:        module_id:模块ID
                event_num:该模块事件数量
                delay    :延时ms
 ** 返回:
 ** 注意:        trigger_node为用户自行生成和维护，完成事件后自行释放
                 在触发后不可改变内部参数
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_delay_trigger(evt_node_t *trigger_node,uint32_t delay);

/*******************************************************************
 ** 函数名:       event_untigger
 ** 函数描述:    取消事件触发函数
 ** 参数:       trigger_node:调用event_delay_trigger的trigger_node
 ** 返回:       -1:失败，0:触发取消成功
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_untigger(evt_node_t *trigger_node);

#endif //







