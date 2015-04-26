#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h> /* shmget() shmat() shmctl() */
#include <sys/shm.h> /* shmget() shmat() shmctl() */
#include <netinet/in.h>


#include "fd_opc.h"
#include "defines.h"

void muere_cliente(qcd_hijo *hijos)
{              
 int pid;
 pid=hijos->pid;
 hijos->pid=0;
 qcd_close_pipe(hijos->pipe);
 hijos->pipe[0]=0;
 hijos->pipe[1]=0;
 close(hijos->socket);
 hijos->socket=-1;
 kill(pid,9);
}

void cliente(int shared,int num)
{
 char buff[4096];
 fd_set leer;
 int tam;
 qcd_hijo *tmp,*hijos;
 
 /* memoria compartida */
 if((tmp=(qcd_hijo *)shmat(shared,0,0)) < 0)
 {
  perror("Imposible atarnos a la memoria compartida");
  exit(-1);
 }
 hijos=&tmp[num];
 close(0);close(1);close(2);
 sprintf(buff,"%s\n\nPassword: (Function not aviable.. yet)\n",QC_WELCOME);
 if(write(hijos->socket,buff,strlen(buff)) < 0) muere_cliente(hijos);
 while(1)
 {
  FD_ZERO(&leer);
  FD_SET(hijos->socket,&leer);
  select(hijos->socket+1,&leer,NULL,NULL,NULL);
  if((tam=read(hijos->socket,buff,4096)) < 1) muere_cliente(hijos);
  write(hijos->pipe[1],buff,tam);
 }
 exit(1);
}

int nueva_conexion(int enchufe,qcd_hijo *hijos,int shared)
{
 int newsock;
 int x,pid;
 if ((newsock=accept(enchufe,NULL,NULL))<0) return(ACCEPT_ERROR);
 if (setnonblocking(newsock) < 0) return(NONBLOCK_FAIL);
 for(x=0;x<QC_MAXCONNECT;x++)
 {
  /* tenemos un hueco libre :) */
  if(hijos[x].socket < 0)
  {
   hijos[x].socket=newsock;
   pipe(hijos[x].pipe);
   if((pid=fork())==0) cliente(shared,x);   
   else 
   {
    hijos[x].pid=pid;
    return(OK);   
   }
  }
 }
 return(OK);
}

int qcd_net_init(void)
{
 int rebindear,enchufe;
 struct sockaddr_in qconsoled_addr;
 if((enchufe=socket(AF_INET,SOCK_STREAM,0)) < 0)
 {
  perror("Creando socket principal");
  exit(NETINIT_ERROR);
 }
 setsockopt(enchufe,SOL_SOCKET, SO_REUSEADDR, &rebindear,sizeof(rebindear));
 setnonblocking(enchufe);
 memset(&qconsoled_addr,'\0',sizeof(qconsoled_addr));
 /* datos para el socket */
 qconsoled_addr.sin_family=AF_INET;
 qconsoled_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 qconsoled_addr.sin_port=htons(QC_PORT);
 if(bind(enchufe,(struct  sockaddr  *)&qconsoled_addr,sizeof(qconsoled_addr)) <0)
 {
  perror("Bind al socket");
  close(enchufe);
  exit(NETINIT_ERROR);
 }
 listen(enchufe,1);
 return(enchufe);                               
}
