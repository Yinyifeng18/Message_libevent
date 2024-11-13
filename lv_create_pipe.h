/*
 * lv_create_pipe.h
 *
 *  Created on: 
 *      Author: Administrator
 */

#ifndef LV_CREATE_PIPE_H_
#define LV_CREATE_PIPE_H_


#define LV_PIPE_DATA_LEN                           1
#define LV_PIPE_DATA_MAX_LEN                       1

int lv_create_pipe();
int lv_get_read_pipe_fd();
int lv_get_write_pipe_fd();
int lv_write_data_to_pipe(char data);


#endif /* LV_CREATE_PIPE_H_*/
