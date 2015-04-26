/* Qconsoled file         */

#ifndef _FD_OPC_H
#define _FD_OPC_H
#include "defines.h"
int setnonblocking(int sock);
int qcd_close_pipe(int *tuberia);
int qcd_select_list(int a,int b,int c);
int pts_master_open();
int pts_slave_open(int master);
void msg_hijo(qcd_hijo *hijos,int num,char *buff);
int mycmp(char *s1,char *s2);
int b_print(char *buff_orig,char *buff_dest,int tipo,int dmax);

extern int grantpt __P ((int __fd));
extern int unlockpt __P ((int __fd));
extern char *ptsname __P ((int __fd));

#endif
