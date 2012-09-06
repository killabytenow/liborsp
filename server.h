#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "rspfd.h"
#include "rspmsg.h"

//typedef int (*RSPSRV_QHANDLER_FUNC)(RSPMSG *m, RSPMSG *r);
//typedef int (*RSPSRV_CMD_HANDLER)(RSPMSG *m, RSPMSG *r);
//
//typedef struct _tag_RSPSRV_QHANDLER {
//  char                 *question;
//  RSPSRV_QHANDLER_FUNC *handler;
//} RSPSRV_QHANDLER;
//
typedef struct _tag_RSPSERVER {
  RSPFD *fd;
  int arch;
//
//  RSPSRV_CMD_HANDLER *s_interrupt;
//  RSPSRV_CMD_HANDLER *s_detach;
//  RSPSRV_CMD_HANDLER *s_kill;
//  RSPSRV_CMD_HANDLER *s_remote_start;
//  RSPSRV_CMD_HANDLER *s_write_registers;
//  RSPSRV_CMD_HANDLER *s_set_thread;
//  RSPSRV_CMD_HANDLER *s_last_signal;
//  RSPSRV_CMD_HANDLER *s_continue;
//  RSPSRV_CMD_HANDLER *s_continue_with_signal;
//  RSPSRV_CMD_HANDLER *s_read_registers;
} RSPSERVER;

int rsp_server_command_parse(RSPFD *fd, RSPMSG *m);
int rsp_server_command_receive(RSPFD *fd, RSPMSG *msg);
int rsp_server_command_receive_and_ack(RSPFD *fd, RSPMSG *msg);
int rsp_client_process_command(RSPSERVER *s, RSPMSG *m);

#endif
