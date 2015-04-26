/* Qconsoled file         */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "fd_opc.h"
#include "defines.h"

int iniciaserver(qcd_config *conf,int pts_master)
{
 int pid,pts_slave,x=0;
 char *mid,**sep,*tmp;
 if ((pid=fork()) == 0)
 {
  pts_slave=pts_slave_open(pts_master);
  dup2(pts_slave,0); /* entrada de datos por aki */ 
  dup2(pts_slave,1); /* salida de datos por aki */
  dup2(pts_slave,2); /* salida de errores tb por aki */
  close(pts_master); /* Estamos en un hijo, asi ke pake usar */
  close(pts_slave); /* esos fd's si aki no sirven ya pa na */
  /* Busca la primera / */
  mid=rindex(conf->program,47);
  /* La cambia por un 0 para poder hacer un chdir */
  mid[0]='\0';
  chdir(conf->program);
  /* Restituye la primera / */
  mid[0]=47;
  /* Vamos a hacer un indice con todos los argumentos disponibles */
  /* Contabilizamos los argumentos */
  tmp=conf->program;
  while((tmp=index(tmp+1,32))) x++;
  /* Asignamos memoria para el indice una vez sabido el numero de argumentos */
  sep=(char **)(malloc(x)+2); 
  /* Rellenamos el indice de argumentos con punteros a conf->program */
  sep[0]=conf->program;
  x=1;tmp=conf->program;
  while((tmp=index(tmp+1,32)))
  {
   tmp[0]='\0';
   sep[x]=tmp+1;
   tmp++;x++;   
  }
  sep[x]=NULL; 
  execv(sep[0],sep);
  fprintf(stderr,"Server could not be spawn\n");
  exit(0);        
 }        
 return(pid);
}
