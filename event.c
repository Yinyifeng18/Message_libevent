
 
/*------------------------------- Includes ----------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <event2/event.h>
#include "list_module.h"

//#include "debug.h"
#include "event.h"

/*------------------- Global Definitions and Declarations -------------------*/
 
/*---------------------- Constant / Macro Definitions -----------------------*/
 
 
/*----------------------- Type Declarations ---------------------------------*/

//事件信息结构体
typedef struct
{
    list_head_node_t *list_head_node;//链表头
}event_msg_t;

//模块事件结构体
typedef struct
{
    uint32_t event_num;//事件数量
    event_msg_t *event_msg;//事件信息
}event_module_t;

/*----------------------- Variable Declarations -----------------------------*/

static event_module_t *event_module = NULL;//模块事件信息存储指针
static list_head_node_t * s_delay_trigger_list = NULL;
/*----------------------- Function Prototype --------------------------------*/
 
 
/*----------------------- Function Implement --------------------------------*/

/*******************************************************************
 ** 函数名:       event_run
 ** 函数描述:    事件分发函数
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
static int32_t event_run(evt_module_e module_id,uint32_t event_id,void*p)
{
    list_head_node_t *list_head_node;
    node_msg_t *node_msg;
    int num = 0;
    evt_node_t *listen_node;
    
    list_head_node = event_module[module_id].event_msg[event_id].list_head_node;
//    debug("list_head_node:%d\n",list_node_amount(list_head_node));
//    debug("module_id:%d,event_id:%d,event_run :%p\n",module_id,event_id,list_head_node);
    /*查询监听者*/
    list_msg_foreach_begin(list_head_node,node_msg)
    listen_node = (evt_node_t *)node_msg->payload;
    if(listen_node->evt_cb != NULL)listen_node->evt_cb(listen_node,p);//调用监听者回调函数
    num++;
    list_msg_foreach_end();
    return num;
}

/*******************************************************************
 ** 函数名:       event_reg
 ** 函数描述:    事件注册函数
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_reg(evt_module_e module_id,uint32_t event_num)
{
    uint32_t index;
    
    if(module_id >= EVT_MODEULE_NUM)return -1;
    event_module[module_id].event_num = event_num;
    /*创建事件存储空间*/
    event_module[module_id].event_msg = (event_msg_t *)malloc(sizeof(event_msg_t)*event_num);
    memset((uint8_t*)event_module[module_id].event_msg,0,sizeof(event_msg_t)*event_num);
    printf("[%s-%d]事件注册函数:module_id=%d,event_num=%d\n",__FUNCTION__,__LINE__,module_id,event_num);

    for(index = 0;index < event_num;index++)//创建事件监听链表
    {
        event_module[module_id].event_msg[index].list_head_node = task_head_create(NULL);
    }
    return 0;
}

/*******************************************************************
 ** 函数名:       event_listen
 ** 函数描述:    事件监听函数注册
 ** 参数:
 ** 返回:
 ** 注意:    listen_node为用户自行malloc生成和维护,不建议修改，
             取消事件监听后自行释放
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_listen(evt_node_t *listen_node)
{
    node_msg_t *node_msg;

    if(listen_node == NULL || listen_node->module_id >= EVT_MODEULE_NUM)return -1;
    if(event_module[listen_node->module_id].event_num <= listen_node->event_id)return -1;
    node_msg = list_node_add(event_module[listen_node->module_id].event_msg[listen_node->event_id].list_head_node,0);//往事件监听链表里添加节点
    // printf("module_id:%d,add:%p\n",listen_node->module_id,event_module[listen_node->module_id].event_msg[listen_node->event_id].list_head_node);

    // printf("event_listen:module_id:%d,add:%p\n", listen_node->module_id,
    //         event_module[listen_node->module_id].event_msg[listen_node->event_id].list_head_node);

    node_msg->payload = (void *)listen_node;
    node_msg->payload_len = sizeof(evt_node_t);
    return 0;
}

/*******************************************************************
 ** 函数名:       event_unlisten
 ** 函数描述:    取消事件监听
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_unlisten(evt_node_t *listen_node)
{
    evt_node_t *node;
    node_msg_t *node_msg;

    if(listen_node == NULL || listen_node->module_id >= EVT_MODEULE_NUM)return -1;
    if(event_module[listen_node->module_id].event_num <= listen_node->event_id)return -1;

    /*查询监听者*/
    printf("event_unlisten:module_id=%d,event_num=%d\n",
            listen_node->module_id, listen_node->event_id);

    list_msg_foreach_begin(event_module[listen_node->module_id].event_msg[listen_node->event_id].list_head_node,node_msg)
    
    node = (evt_node_t *)node_msg->payload;
    if(listen_node != node) continue;
    list_cur_node_del(event_module[listen_node->module_id].event_msg[listen_node->event_id].list_head_node);
    return 0;
    
    list_msg_foreach_end();
    return -1;
}

/*******************************************************************
 ** 函数名:       event_trigger
 ** 函数描述:    事件触发函数
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_trigger(evt_module_e module_id,uint32_t event_id,void*p)
{   
    if(module_id >= EVT_MODEULE_NUM)
    {
        printf("[%s:%d] trigger error\n", __func__, __LINE__);
        return -1;
    }

    if(event_module[module_id].event_num <= event_id)
    {
        printf("[%s:%d] trigger error\n", __func__, __LINE__);
        return -1;
    }
    // printf("[%s-%d]事件触发函数:module_id=%d,event_id=%d\n",__FUNCTION__,__LINE__,module_id,event_id);

    return event_run(module_id,event_id,p);
}

static int32_t event_timeout(uint8_t status,void *payload,uint32_t payload_len)
{
    evt_node_t *trigger_node = (evt_node_t *)payload;

    //debug("%s\n",__func__);
    event_run(trigger_node->module_id,trigger_node->event_id,trigger_node->p);
    if(trigger_node->evt_cb != NULL)trigger_node->evt_cb(trigger_node,trigger_node->p);
    
return LIST_CB_NONE;
}

/*******************************************************************
 ** 函数名:       event_delay_trigger
 ** 函数描述:    事件延迟触发函数
 ** 参数:
 ** 返回:
 ** 注意:        trigger_node为用户自行生成和维护，完成事件后自行释放
                 在触发后不可改变内部参数
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_delay_trigger(evt_node_t *trigger_node,uint32_t delay)
{
    node_msg_t *node_msg;

    if(trigger_node == NULL)return -1;
    if(trigger_node->module_id >= EVT_MODEULE_NUM)return -1;
    if(event_module[trigger_node->module_id].event_num <= trigger_node->event_id)return -1;

    if(delay == 0)
    {
        return event_run(trigger_node->module_id,trigger_node->event_id,trigger_node->p);
    }
    else
    {
        node_msg = list_node_add(s_delay_trigger_list,delay);
        node_msg->retry_times = 0;
        node_msg->payload = (void*)trigger_node;
        node_msg->payload_len = sizeof(evt_node_t);
        node_msg->task_signal = event_timeout;
    }
    return 0;
}

/*******************************************************************
 ** 函数名:       event_untigger
 ** 函数描述:    取消事件触发函数
 ** 参数:
 ** 返回:       -1:失败，0:触发取消成功
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
int32_t event_untigger(evt_node_t *trigger_node)
{
    evt_node_t *node;
    node_msg_t *node_msg;

    if(trigger_node == NULL)return -1;
    
    /*查询监听者*/
    list_msg_foreach_begin(s_delay_trigger_list,node_msg)
    
    node = (evt_node_t *)node_msg->payload;
    if(trigger_node != node) continue;
    list_cur_node_del(s_delay_trigger_list);
    return 0;
    
    list_msg_foreach_end();
    return -1;
}

/*******************************************************************
 ** 函数名:       event_init
 ** 函数描述:    事件管理模块初始化函数
 ** 参数:
 ** 返回:
 ** 注意:
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void event_init(struct event_base *base)
{
    /**创建模块事件空间**/
    event_module =(event_module_t *)malloc(sizeof(event_module_t)*EVT_MODEULE_NUM); 
    memset((uint8_t*)event_module,0,sizeof(event_module_t)*EVT_MODEULE_NUM);

    /*创建延时触发事件链表*/
    s_delay_trigger_list = task_head_create(base);
}

/*---------------------------------------------------------------------------*/




