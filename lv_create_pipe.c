#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include "lv_create_pipe.h"
static int sg_lvgl_pipe_fd[2] = {0};
static int sg_read_lvgl_message_fd;
static int sg_wirte_lvgl_message_fd;
/*******************************************************************
** 函数名:     lv_create_pipe
** 函数描述:   创建lvgl消息通知管道
** 参数:      
** 返回:       成功 0;
**            失败 -1;
** 注意:
********************************************************************/
int lv_create_pipe()
{
    int result = pipe(sg_lvgl_pipe_fd);
    if(result == -1)
    {
        printf("Create cloud send_list pipe failed\n");
        return -1;
    }
    sg_read_lvgl_message_fd = sg_lvgl_pipe_fd[0];
    sg_wirte_lvgl_message_fd = sg_lvgl_pipe_fd[1];
    return 0;
}
/*******************************************************************
** 函数名:     lv_get_read_pipe_fd
** 函数描述:   获取lvgl消息通知管道的读描述符
** 参数:      
** 返回:       成功 0;
**            失败 -1;
** 注意:
********************************************************************/
int lv_get_read_pipe_fd()
{
    return sg_read_lvgl_message_fd;
}
/*******************************************************************
** 函数名:     lv_get_write_pipe_fd
** 函数描述:   获取lvgl消息通知管道的写描述符
** 参数:      
** 返回:       成功 0;
**            失败 -1;
** 注意:
********************************************************************/
int lv_get_write_pipe_fd()
{
    return sg_wirte_lvgl_message_fd;
}
/*******************************************************************
** 函数名:     lv_get_write_pipe_fd
** 函数描述:   向lvgl消息通知管道写数据
** 参数:      
** 返回:       成功 0;
**            失败 -1;
** 注意:
********************************************************************/
int lv_write_data_to_pipe(char data)
{
    int ret = write(sg_wirte_lvgl_message_fd, &data, LV_PIPE_DATA_LEN);
    if(ret == -1)
    {
        perror("write");
        return -1;
    }
    return 0;

}

        