#ifndef _INET_H_
#define _INET_H_
#include "defines.h"
void muere_cliente(qcd_hijo *hijos);
int nueva_conexion(int enchufe,qcd_hijo *hijos,int shared);
void cliente(int shared,int num);
int qcd_net_init(void);
#endif