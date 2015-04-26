/* Qconsoled file         */

#ifndef _FD_OPC_H
#define _FD_OPC_H
#include "defines.h"
int setnonblocking(int sock);
int qcd_close_pipe(int *tuberia);
int qcd_select_list(fd_set *leer,qcd_hijo *hijos,int sock_alto);

#endif