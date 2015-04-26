/* Qconsoled file         */
/* Operaciones sobre fd's */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <string.h>

#include "defines.h"
#include "fd_opc.h"

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
int qcd_select_list(int a,int b,int c)
{
 if(a>b) 
 {
 if (a>c) return(a);
 else return(c);
 }
 if(b>c) return(b);
 else return(c);	 
}                         

/*
  Funcion: pty_master_open()
  Descripcion: nos abre la pseudotermial maestra y la deja lista para su
  uso; devuelve el fd de la terminal abierta.
*/
int pts_master_open()
{
 int fd;
 struct termios attr;
 if ((fd=open("/dev/ptmx",O_RDWR)) == -1)
 {
  perror("Error abriendo /dev/ptmx");
  exit(ERROR);
 }
 grantpt(fd);
 unlockpt(fd);
 tcgetattr(fd,&attr);
 cfmakeraw (&attr);
 tcsetattr(fd,TCSAFLUSH,&attr);
 return(fd);   
}
/*
   Funcion: int pty_slave_open(int master)
   Descripcion: dado el fd de una pseudoterminal master, abre su esclava, y
   devuelve su fd
*/
int pts_slave_open(int master)
{
 char *nombre;
 int fd;
 nombre=(char *)ptsname(master);
 if((fd=open(nombre,O_RDWR))==-1)
 {
  perror("Abriendo esclava de /dev/ptmx");
  exit(ERROR);
 }
 return(fd);
}
/* Funcion: msg_hijo(qcd_hijo *hijos,int num,char *buff)
   Descripcion: Envia el mensaje contendio en buff a los hijos activos en hijos
*/
void msg_hijo(qcd_hijo *hijos,int num,char *buff)
{
 int x;
 for(x=0;x<num;x++)
  if(hijos[x].socket!=-1) write(hijos[x].llegan[1],buff,strlen(buff));
}

/* Compara 2 cadenas terminadas en \0 y la primera ademas e indiferentemente
   en \n y \r 
char *s1 <-> Cadena terminada en \0 \n o \r
char *s2 <-> Cadena terminada en \0
devuelve <-> 0 si son iguales o un numero si son distintas 
*/
int mycmp(char *s1,char *s2)
{
 register unsigned char *str1 = (unsigned char *)s1;
 register unsigned char *str2 = (unsigned char *)s2;
 register int     res;
 while ((res = toupper(*str1) - toupper(*str2)) == 0)
 {
  if (*str1 == '\0') return 0;
  str1++;
  str2++;
  if (((*str1 == '\n') || (*str1 == '\r')) && (*str2=='\0')) return 0;
 }
 return (res);
}

/* Imprime un buffer en otro buffer pero formateando cada linea
como si de un ircd se tratase 
char *buff_orig <-> Buffer desde donde se desean formatear los datos
char *buff_dest <-> Buffer donde se almaceranan los datos
int tipo        <-> Tipo de formato
int dmax	<-> Tamanyo del buffer de destino
devuelve 	<-> la cantidad de datos impresos
*/
int b_print(char *buff_orig,char *buff_dest,int tipo,int dmax)
{
  register unsigned char *lact=(unsigned char *)buff_orig;
  register unsigned char *lprox=(unsigned char *)buff_orig;
  register int tam=0;
  while( ((lprox=strchr(lact,'\n')) != NULL) && (tam<dmax))
  {
  	lprox[0]='\0';
     	tam+=sprintf(buff_dest+tam,"%d :%s\n",tipo,lact);
     	lact=lprox+1;                               
  }
  return(tam+1);
}
