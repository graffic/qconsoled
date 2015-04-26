/*
  Qconsoled Quake Console Daemon
  Copyright (C) 1998 graffic
       
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
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <sys/ipc.h> /* shmget() shmat() shmctl() */
#include <sys/shm.h> /* shmget() shmat() shmctl() */
                        
#include "inet.h"
#include "defines.h"
#include "conf.h"
#include "qserver.h"
#include "fd_opc.h"
#include "senyal.h"


/* Definiciones exteriores para el uso con las señales */
qcd_config* tmp_conf;
qcd_hijo* tmp_hijos;
int qsv_pid;

int main()
{
 int tam,enchufe,codigo,cliente;
 int pts_master,cl_out[2],sock_alto;
 char buff[4098],*tmp,tmp2[4098];
 fd_set leer;
 qcd_hijo *hijos;
 qcd_config conf;
 struct sockaddr_in dir;
 int x,y,z;
 qsv_pid=0;
 fprintf(stderr,"%s-Welcome/Bienvenido/Nomeacuerdo to Qconsoled\n",REF_QCON);

 /* Leemos la configuracion */
 qcd_initconf(&conf);
 tmp_conf=&conf;
 
 /* Una vez ke Tenemos todo asignado, Ya podemos modificar lo ke hace SIGTERM
 para que libere la memoria ke tenemos en caso de terminacion. Tb ignoramos
 sigpipe, pq la obtendremos mazo de veces y no nos valdra para mucho util */
 qcd_initsignal(); 

 /* Tuberias para comunicarse con el server */
 pts_master=pts_master_open();
 /* Tuberias de comunicacion con los hijos */
 if ((pipe(cl_out))==-1)
 {
  perror("Creando tuberias al programa");
  exit(-1);
 }
  /* Memoria Compartida */
 if((conf.id_mem=shmget(IPC_PRIVATE,conf.qc_consoles*sizeof(qcd_hijo),IPC_CREAT|0600))==-1)
 {
  perror("Compartiendo memoria");
  exit(1);
 }
 /* iniciamos el server */
 if((qsv_pid=iniciaserver(&conf,pts_master))<0)
 {
  perror("iniciaserver()");
  exit(-1);
 }
 /* iniciamos la red */
 enchufe=qcd_net_init(&conf);

 if((hijos=(qcd_hijo *)shmat(conf.id_mem,0,0)) == (qcd_hijo *)-1)
 {
  perror("Atandonos a memoria compartida");
  exit(1);
 }
 tmp_hijos=hijos;
       
 /* Dejamos a 0 las tuberias */
 for(x=0;x<conf.qc_consoles;x++) 
 {
  pipe(hijos[x].llegan);
  hijos[x].salen=cl_out[1];
  hijos[x].pid=0;
  hijos[x].socket=-1;
 }
  
 sock_alto=qcd_select_list(enchufe,pts_master,cl_out[0]);
 fprintf(stderr,"%s-Inicializacion completa.. server_pid: %d\n",REF_QCON,qsv_pid);
 
 while (1)
 {
  /* ponemos lo q keremos observar */
  FD_ZERO(&leer);
  FD_SET(pts_master,&leer); /* datos del server */
  FD_SET(cl_out[0],&leer); /* datos de clientes */
  FD_SET(enchufe,&leer); /* conexion nueva */
  /* usamos el select */
  while (select(sock_alto+1,&leer,NULL,NULL,NULL)<0);
 
  /* Si el servidor envia datos */
  if(FD_ISSET(pts_master,&leer))
  {
   tam=read(pts_master,buff,4098);
   if((tam<0))
   {
    close(pts_master); /* Nos aseguramos de ke este chapado */
    pts_master=pts_master_open();
    qsv_pid=0;
    sock_alto=qcd_select_list(enchufe,pts_master,cl_out[0]);        
   }
   else
   {
    fprintf(stderr,"%s%s-MsjSaliente(%d):\n%s",REF_QCON,REF_QSV,tam,buff);
 /* formateamos lo ke vamos a enviar */
    tam=b_print(buff,tmp2,MSG_DAT,3500);
/* Enviamos datos a los ke esten conectados */
    for(x=0;x<conf.qc_consoles;x++)
     if(hijos[x].socket != -1) write(hijos[x].llegan[1],tmp2,tam);
    memset(tmp2,'\0',4096);
   }
   memset(buff,'\0',4096); 
  }
  /* Alguna conexion nueva */
  if(FD_ISSET(enchufe,&leer))
  {
   nueva_conexion(enchufe,hijos,&conf);
  }
  
  /* Miramos los datos recibidos de los clientes (hijos) */
  if(FD_ISSET(cl_out[0],&leer))
  {
   tam=read(cl_out[0],buff,4098);
#ifdef DEBUG__
   fprintf(stderr,"%s%s-MensajeEntrante:\n%s\n",REF_QCON,REF_QCL,buff);
#endif
   codigo=atoi(buff);
   cliente=atoi(index(buff,32));
   tmp=index(buff,58)+1;
   switch(codigo)
   {
    case MSG_DAT:
     if(qsv_pid>0) 
     {
      sprintf(tmp2,"%d :[Client %d] %s",MSG_INF,cliente+1,tmp);
      msg_hijo(hijos,conf.qc_consoles,tmp2);
      write(pts_master,tmp,strlen(tmp));    
     }
     else
     {
      sprintf(tmp2,"%d :OFF",MSG_SRV);
      msg_hijo(hijos,conf.qc_consoles,tmp2);
     }
     break;
    case MSG_SRV:
     if(!strncmp(tmp,"ON",2))
     {
      if(qsv_pid==0)
      {
        if((qsv_pid=iniciaserver(&conf,pts_master))<0) {perror("iniciaserver()");exit(10);}
        sprintf(buff,"%d :ON\n",MSG_SRV);
        msg_hijo(hijos,conf.qc_consoles,buff);
      }
      else      {
       sprintf(buff,"%d :ON\n",MSG_SRV_ALREADY);
       msg_hijo(hijos,conf.qc_consoles,buff); 
      }
     } 
     else
     {
      if(qsv_pid==0)
      {
       sprintf(buff,"%d :OFF\n",MSG_SRV_ALREADY);
       msg_hijo(hijos,conf.qc_consoles,buff);
      }
      else
      {
       kill(qsv_pid,SIGKILL);
       sprintf(buff,"%d :OFF\n",MSG_SRV);
       msg_hijo(hijos,conf.qc_consoles,buff);
      }
     }
     break;
    case MSG_INF:
     z=0;tam=0;
     tam=sprintf((buff+tam),"%d :-------INFO---------\n",MSG_INF);
     y=sizeof(dir);
     for(x=0;x<conf.qc_consoles;x++) {
      if(hijos[x].socket != -1) 
      {
       getpeername(hijos[x].socket,(struct sockaddr *)&dir,&y);
       tam+=sprintf(buff+tam,"%d :Client:%d From:%s:%d\n",MSG_INF,x+1,inet_ntoa(dir.sin_addr),ntohs(dir.sin_port));
       z++;
      } }
     sprintf(buff+tam,"%d :Statics: %d/%d - SV: %s\n%d :-------INFO---------\n",MSG_INF,z,conf.qc_consoles,(qsv_pid==0)?"OFF":"ON",MSG_INF);
     write(hijos[cliente].llegan[1],buff,strlen(buff));
     
     break;
   }
   memset(buff,'\0',4096);
   memcpy(tmp2,buff,4096);
  }
 }
 return(0);
}
