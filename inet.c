#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
                     

#include "fd_opc.h"
#include "defines.h"
#include "cliente_txt.h"

int dprintf(int d, const char *format, ...);

int nueva_conexion(int enchufe,qcd_hijo *hijos,qcd_config *conf)
{
 int newsock;
 int x,pid;
 socklen_t lonj_name;
 struct sockaddr_in nombre;
 lonj_name=sizeof(struct sockaddr_in);
 if ((newsock=accept(enchufe,(struct sockaddr *)&nombre,&lonj_name))<0) return(ACCEPT_ERROR);
 if (setnonblocking(newsock) < 0) return(NONBLOCK_FAIL);
 for(x=0;x<(conf->qc_consoles);x++)
 {
  /* tenemos un hueco libre :) */
  if(hijos[x].socket < 0)
  {
   hijos[x].socket=newsock;
   if((pid=fork())==0) 
   {
    hijos[x].pid=getpid();
    conf->id_mem=0;
    cliente_txt(conf,&hijos[x],x);   
   }
   else 
   {
    hijos[x].pid=pid;
    printf("%s-Conexion Aceptada desde %s:%d - PID %d -Hijo num:%d\n",REF_QCON,inet_ntoa(nombre.sin_addr),ntohs(nombre.sin_port),pid,x); 
    return(OK);   
   }
  }
 }
 /* Si hemos llegado aki es ke no habia ningun hueco libre asinke... */
 dprintf(newsock,"%s-Servidor Lleno / Server Full\n",REF_QCON);
 printf("%s-Conexion rechazada desde: %s:%d\n",REF_QCON,inet_ntoa(nombre.sin_addr),ntohs(nombre.sin_port));
 close(newsock);
 return(OK);
}

int qcd_net_init(qcd_config *conf)
{
 int rebindear,enchufe;
 struct sockaddr_in qconsoled_addr;
 if((enchufe=socket(AF_INET,SOCK_STREAM,0)) < 0)
 {
  perror("Creando socket principal");
  exit(NETINIT_ERROR);
 }
 setsockopt(enchufe,SOL_SOCKET, SO_REUSEADDR, &rebindear,sizeof(rebindear));
 //setnonblocking(enchufe);
 memset(&qconsoled_addr,'\0',sizeof(qconsoled_addr));
 /* datos para el socket */
 qconsoled_addr.sin_family=AF_INET;
 qconsoled_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 qconsoled_addr.sin_port=htons(conf->qc_port);
 if(bind(enchufe,(struct  sockaddr  *)&qconsoled_addr,sizeof(qconsoled_addr)) <0)
 {
  perror("Bind al socket");
  close(enchufe);
  exit(NETINIT_ERROR);
 }
 listen(enchufe,0);
 return(enchufe);                               
}
