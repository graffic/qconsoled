#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
              
#include "defines.h"
#include "fd_opc.h"
#include "help_ing.h"

void muere_cliente_txt(qcd_hijo *hijos)
{
 int pid;
 pid=hijos->pid;
 fprintf(stderr,"%s%s-Matando Hijo con PiD %d\n",REF_QCON,REF_QCL,pid);
 hijos->pid=0;
 shutdown(hijos->socket,2);
 hijos->socket=-1;
 /* Cuando esto peta se hace un detach de la zona de memoria compartida a la
 que nos attacheamos en el fork */
 kill(pid,9);
}

char *leer_in(char *buffer,int fd,qcd_hijo *hijo,int posact)
{
 int tam;
 char input_buff[512];
 if ( (tam=read(fd,input_buff,512) ) < 0) muere_cliente_txt(hijo); 
 if ( input_buff[0]==8 && ((posact-1) > 0) )
 { 
  buffer--;
  buffer[0]='\0';
  return(buffer); 
 } 
 if ((posact+tam)<510) memcpy(buffer,input_buff,tam);
 else return(NULL);
 /* Ahora comprobamos ke nos manda :) */ 
 if ( strchr(buffer,'\n') == NULL)
 {
  buffer+=tam;
  return(buffer);
 }
 else return(NULL);                       
}

void cliente_txt(qcd_config *conf,qcd_hijo *hijo,int num)
{
 char buff[4096],tmp2[4096],*tmp=NULL,*posin=NULL;
 char key_buff[512];
 char *lact,*lprox;
 fd_set leer;
 int tam,codigo,x=0;
 int estado=0; 
 
 dup2(hijo->socket,0);
 dup2(hijo->socket,1);
 setvbuf(stdout,NULL,_IONBF,0);
 setvbuf(stdin,NULL,_IOLBF,0);
 memset(key_buff,'\0',512);
 /* Proceso de autentificacion cutre */
 printf("%s\r\n%s\r\n\nPassword: ",QC_WELCOME,conf->qc_issue); 
 
 posin=key_buff;
 while(1)
 {
  FD_ZERO(&leer);FD_SET(0,&leer);
  select(1,&leer,NULL,NULL,NULL);
  /* Leer de lo ke nos manda el pibe conectado */ 
  if (FD_ISSET(0,&leer))
  {
   if( (posin=leer_in(posin,0,hijo,posin-key_buff))==NULL)
   {               
    posin=key_buff;
    if(mycmp(key_buff,conf->qc_password)) x++;
    else break;
    if(x==3)
    {
     printf("%s> Access Denied!\r\n",REF_QCL);
     muere_cliente_txt(hijo);
    }
    else
    {
     printf("Password: ");
     memset(buff,'\0',4096);
    }
   }
  }
 }
 printf("-----------Access Granted!-----------\r\n");
 memset(buff,'\0',4096);
 memcpy(tmp2,buff,4096);
 memset(key_buff,'\0',512);
 while(1)
 {
  FD_ZERO(&leer);
  FD_SET(hijo->llegan[0],&leer);
  FD_SET(0,&leer);
  select(hijo->llegan[0]+1,&leer,NULL,NULL,NULL);
  
  /* Una vez dentro cuando el pibe escribe */
  if (FD_ISSET(0,&leer))
  {
   if( (posin=leer_in(posin,0,hijo,posin-key_buff))==NULL)
   {
    posin=key_buff;
    tam=strlen(key_buff);
    if(!estado)
    {
     if(!mycmp(key_buff,"MSG")) 
     {
      estado=1;
      printf("%s <> Command mode ON\r\n",REF_QCL);
     }
     else if(!mycmp(key_buff,"SERVER_SWITCH_ON"))
     {
      tam=sprintf(tmp2,"%d %d :ON",MSG_SRV,num);
      write(hijo->salen,tmp2,tam);
     }
     else if(!mycmp(key_buff,"SERVER_SWITCH_OFF"))
     {
      tam=sprintf(tmp2,"%d %d :OFF",MSG_SRV,num);
      write(hijo->salen,tmp2,tam);
     }
     else if(!mycmp(key_buff,"HELP")) printf("%s\r\n",HELP_ING);
     else if(!mycmp(key_buff,"INFO"))
     {
      tam=sprintf(tmp2,"%d %d :INFO",MSG_INF,num);
      write(hijo->salen,tmp2,tam);   
     }
     else if(!mycmp(key_buff,"QUIT"))
     {
      printf("%s> Good Bye ;)\r\n",REF_QCL);
      muere_cliente_txt(hijo);
     }
    }
    else
    {
     if(!mycmp(key_buff,"MSG") && (tam<6))
     {
      estado=0;
      printf("%s <> Command mode OFF\n",REF_QCL);
     }
     else
     {
      tam=sprintf(tmp2,"%d %d :%s",MSG_DAT,num,key_buff);
      write(hijo->salen,tmp2,tam);
     }
    }
    memset(buff,'\0',4096);
    memcpy(buff,tmp2,4096);
    memset(key_buff,'\0',512);
   }
  }
  /* Nos llegan datos de qconsoled... que tendremos ke analizar */
  if (FD_ISSET(hijo->llegan[0],&leer))
  {
   tam=read(hijo->llegan[0],buff,4096);
   lact=buff;
   while( (lprox=strchr(lact,'\n')) != NULL )
   {
    lprox[0]='\0';
    codigo=atoi(lact);
    tmp=lact+5;
    switch(((int)codigo/(int)100)*100)
    {
     case MSG_SRV:
      if(codigo!=MSG_SRV)
      {
       if(!strncasecmp(tmp,"OFF",3)) tam=sprintf(tmp2,"%s> Server Already OFF !\r\n",REF_QCON);
       if(!strncasecmp(tmp,"ON",2)) tam=sprintf(tmp2,"%s> Server Already ON!\r\n",REF_QCON);      
      }
      else
      {
       if(!strncasecmp(tmp,"OFF",3)) tam=sprintf(tmp2,"%s> Server Switched OFF!\r\n",REF_QCON);
       if(!strncasecmp(tmp,"ON",2)) tam=sprintf(tmp2,"%s> Server Switched ON!\r\n",REF_QCON);
      }
      send(hijo->socket,tmp2,tam,0);
      break;
     case MSG_DAT:
      tam=sprintf(tmp2,"%s> %s\r\n",REF_QSV,tmp);
      send(hijo->socket,tmp2,tam,0);
      break;
     case MSG_INF:
      tam=sprintf(tmp2,"%s> %s\r\n",REF_QCON,tmp);
      send(hijo->socket,tmp2,tam,0);                  
      break;
     default:
      tam=sprintf(tmp2,"QconsoleD envia datos erroneos\r\n");
      send(hijo->socket,tmp2,tam,0);
      break;
    }
    lact=lprox+1;
   }  
   memset(buff,'\0',4096);
   memcpy(tmp2,buff,4096); 
  }
 }
 exit(1);
}
