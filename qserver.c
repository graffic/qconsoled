/* Qconsoled file         */

#include <unistd.h>
#include "defines.h"

int iniciaserver(int *tuberia1,int *tuberia2)
{
 int pid;
 if ((pid=fork()) == 0)
 {
  /* cerramos stdin stdout y sterr */
  close(0);close(1);close(2);
  dup(tuberia1[0]); /* entrada de datos por aki */
  /* lo que entra por tuberia1[0] sale por tuberia1[1] */
  dup(tuberia2[1]); /* salida de datos por aki */
  /* Sale por tuberia2[1] lo que entra por tuberia2[0] */
  dup(tuberia2[1]); /* salida de errores tb por aki */
  close(tuberia1[0]);close(tuberia2[1]);
  chdir(QSV_DIR);
  execlp(QSV_RUTA,QSV_PARAM,NULL);
  exit(127);        
 }        
 return(pid)                         ;
}