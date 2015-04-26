/* Qconsoled file         */
/* Operaciones sobre fd's */

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
                                                       
#include "defines.h"

/* 
   Funcion: int setnonblocking(int sock);
   Descripcion: Dado un descriptor valido, lo convierte a un descriptor no
   bloqueante, devuelve OK si todo va bien y NONBLOCK_FAIL cuando hay error.
*/   
int setnonblocking(int sock)
{
 int opts;
 opts = fcntl(sock,F_GETFL);
 if (opts < 0) return(NONBLOCK_FAIL);
 
 opts = (opts | O_NONBLOCK);
 if (fcntl(sock,F_SETFL,opts) < 0) return(NONBLOCK_FAIL);
 
 return(OK);
}

/*
   Funcion: qcd_close_pipe(int *tuberia);
   Descripcion: dada la variable de una tuberia (un array de 2 enteros los
   cuales son los extremos de una tuberia) los cierra; devuelve OK si todo
   va bien y CLOSE_ERROR cuando hay error.
*/
int qcd_close_pipe(int *tuberia)
{
 if(close(tuberia[0])<0) return(CLOSE_ERROR);
 if(close(tuberia[1])<0) return(CLOSE_ERROR);
 return(OK); 
}

/*
   Funcion: qcd_select_list(fd_set *leer,qcd_hijo *hijos,int sock_alto);
   Descripcion: Dado una lista de descriptores, un array de tuberias del
   tipo int variable[x][2], añade al fd_set las tuberias activas y devuelve
   el descriptor mas alto.
*/
int qcd_select_list(fd_set *leer,qcd_hijo *hijos,int sock_alto)
{
 int x;
 for (x=0;x<QC_MAXCONNECT;x++)
 {
  if(hijos[x].pipe[0]!=0)
  {
   FD_SET(hijos[x].pipe[0],leer);
   if(hijos[x].pipe[0]>sock_alto) sock_alto=hijos[x].pipe[0];
  }
 }
 return(sock_alto);
}                         
                                                                                                                                                                                                                                                         