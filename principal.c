/*
  Qconsoled Quake Console Daemon
  Copyright (C) 1998 Javier Gonel
       
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
                 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
                                        
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
                                                    
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <sys/ipc.h> /* shmget() shmat() shmctl() */
#include <sys/shm.h> /* shmget() shmat() shmctl() */



#include "fd_opc.h"
#include "qserver.h"                         
#include "inet.h"
#include "defines.h"

int main()
{
 int qsv_pid,tam,enchufe;
 int tuberia1[2],tuberia2[2],sock_alto;
 char buff[4098];
 fd_set leer;
 qcd_hijo *hijos;
 
 int x,y,shared;

 /* Ignoramos el SIGCPIE por si acaso*/
 signal(SIGPIPE,SIG_IGN);
 
 /* Compartimos hijos[] */
 if((shared=shmget(IPC_PRIVATE,sizeof(qcd_hijo)*QC_MAXCONNECT,IPC_CREAT|0600))==-1)
 {
  perror("Imposible asignar zona de memoria compartida");
  exit(1);
 }
 /* Nos atamos a esa zona */
 if((hijos=(qcd_hijo *)shmat(shared,0,0)) == (qcd_hijo *)-1)
 {
  perror("Imposible atarnos a la memoria compartida");
  exit(1);
 }
 /* tuberias pa comunicarse con el cherver */               
 if ((pipe(tuberia1) || pipe(tuberia2))==-1)
 {
  perror("pipe()");
  exit(-1);
 }
 /* iniciamos el server */
 if((qsv_pid=iniciaserver(tuberia1,tuberia2))<0)
 {
  perror("iniciaserver()");
  exit(-1);
 }
 /* iniciamos la red */
 enchufe=qcd_net_init();
 
 /* Dejamos a 0 las tuberias */
 for(x=0;x<QC_MAXCONNECT;x++) for(y=0;y<2;y++) 
 			      {
 			       hijos[x].pipe[y]=0;
 			       hijos[x].pid=0;
 			       hijos[x].socket=-1;
 			      }
  
 /* Aqui se deberia dejar el server en segundo plano ya q a partir de ahora 
    se supone q todo esta correctamente inicializado y los errores no 
    deberian de ser criticos (<-- q iluso XD) */
 sock_alto=enchufe;
 printf("Inicializacion completa.. server_pid: %d\n",qsv_pid);
 
 /* Bueno chicos es hora de dejar esto en background */
// if((x=fork())==0) {
 while (1)
 {
  /* ponemos lo q keremos observar */
  FD_ZERO(&leer);
  FD_SET(tuberia2[0],&leer); /* datos del server */
  FD_SET(enchufe,&leer); /* conexion nueva */
  sock_alto=qcd_select_list(&leer,hijos,sock_alto);
  /* usamos el select */
  select(sock_alto+1,&leer,NULL,NULL,NULL);
  
  /* Si el servidor envia datos */
  if(FD_ISSET(tuberia2[0],&leer))
  {
   tam=read(tuberia2[0],buff,4098);
   for(x=0;x<QC_MAXCONNECT;x++) 
    if(hijos[x].pid!=0) 
     if(send(hijos[x].socket,buff,tam,0)<0) muere_cliente(&hijos[x]);   
  }
  /* Alguna conexion nueva */
  else if(FD_ISSET(enchufe,&leer))
  {
   nueva_conexion(enchufe,hijos,shared);
  }
  
  /* Miramos los datos recibidos de los clientes (hijos) */
  else
  {
   for(x=0;x<QC_MAXCONNECT;x++)
   {
    if(hijos[x].pipe[0]!=0)
    {
     if(FD_ISSET(hijos[x].pipe[0],&leer))
     {
      tam=read(hijos[x].pipe[0],buff,4098);
      write(tuberia1[1],buff,tam);
     }
    }
   } 
  }
 }
// } /* fork() */
 return(0);
}
