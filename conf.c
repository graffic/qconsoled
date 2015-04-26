/* QconsoleD file */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "defines.h"
#include "conf.h"

int qcd_initconf(qcd_config *conf)
{
 int x=CONF_F_ERROR,offset;
 FILE *fl_conf=NULL;
 char puf[1024],data[2048],*temp1,*temp2;

 init_conf(conf);

 /* Tiene preferencia el fichero de configuracion del directorio del 
    qconsoled */
 if(access("/etc/qconsoled.conf",R_OK) == 0) x=1;
 if(access("qconsoled.conf",R_OK) == 0) x=2;
 if(x==CONF_F_ERROR)
 {
  perror("qcd_initconf: No configuration file found!");
  exit(1);
 }

 /* Abrimos directamente ya q las comprobaciones estan hechas :) */
 switch(x)
 {
  case 1:
   fl_conf=fopen("/etc/qconsoled.conf","r");
   break;
  case 2:
   fl_conf=fopen("qconsoled.conf","r");
   break;
 }

       
 /* Una vez abierto el fichero hay una serie de secciones obligatorias :)
    que si no aparecen el fichero de configuracion se considera invalido
    Tambien destacar que se comprueba si existen los directorios y tal,
    es una inicializacion pesada pero evita problemas.
 */
 memset(data,'\0',2048);
 offset=0;
 while(!feof(fl_conf))
 {
  fgets(puf,1024,fl_conf);
  if(offset != 0)
  {
   strcpy(data+offset,puf);
   offset=0;
  }
  else strcpy(data,puf);
  temp1=data;
  while((temp2=index(temp1,10)) != NULL)
  {
   temp2[0]='\0';
   linea(temp1,conf);
   temp1=temp2+1;
  } 
  if(strlen(temp1) != 0)
  {
   strcpy(data,temp1);
   offset=strlen(temp1);
   memset(data+offset,'\0',2048-offset);
  }
  else memset(data,'\0',2048);
 } 
 if ((conf_check(conf))==ERROR)
 {
  fprintf(stderr,"Archivo de configuracion con errores :!\n");
  exit(1);
 }
 fclose(fl_conf);
#ifdef DEBUG__
 conf_print(conf);
#endif
 return(OK);
}

int linea(char *linea,qcd_config *conf)
{
 if (!strncmp(linea,"ISSUE=",6))
 {
  linea=index(linea,'"')+1;
  linea[strlen(linea)-1]='\0';
  conf->qc_issue=(char *)malloc(strlen(linea)+1);
  strncpy(conf->qc_issue,linea,strlen(linea));
 }
 else if (!strncmp(linea,"PROGRAM=",8))
 {
  linea=index(linea,'"')+1;
  linea[strlen(linea)-1]='\0';
  conf->program=(char *)malloc(strlen(linea)+1);
  strncpy(conf->program,linea,strlen(linea));
 }
 else if (!strncmp(linea,"PASSWORD=",9))
 {
  linea=index(linea,'"')+1;
  linea[strlen(linea)-1]='\0';
  conf->qc_password=(char *)malloc(strlen(linea)+1);
  strncpy(conf->qc_password,linea,strlen(linea));
 }
 else if (!strncmp(linea,"CONSOLES=",9))
 {
  linea=index(linea,'"')+1;
  linea[strlen(linea)-1]='\0';
  if(atoi(linea)<501) conf->qc_consoles=atoi(linea);
  else conf->qc_consoles=500;
 }
 else if (!strncmp(linea,"PORT=",5))
 {
  linea=index(linea,'"')+1;
  linea[strlen(linea)-1]='\0';
  conf->qc_port=atoi(linea);
 } 
 return(OK);
}

int conf_print(qcd_config *conf)
{
 fprintf(stderr,"%s-Program: %s\n",REF_QCON,conf->program);
 fprintf(stderr,"%s-Using password: %s ",REF_QCON,conf->qc_password);
 fprintf(stderr,"- Max consoles: %d ",conf->qc_consoles);
 fprintf(stderr,"- Port: %d -\n",conf->qc_port);
 fprintf(stderr,"%s-[ISSUE] %s\n",REF_QCON,conf->qc_issue);
 return(OK);
}

int conf_check(qcd_config *conf)
{
 if (conf->program==NULL) return(ERROR);
 if (conf->qc_password==NULL) return(ERROR);
 if (conf->qc_consoles==0) return(ERROR);
 if (conf->qc_port==0) return(ERROR);
 if (conf->qc_issue==NULL) return(ERROR);
 return(OK);
}

int init_conf(qcd_config *conf)
{
 conf->program=NULL;
 conf->qc_password=NULL;
 conf->qc_consoles=0;
 conf->qc_port=0;
 conf->qc_issue=NULL;
 return(OK);
}

int free_conf(qcd_config *conf)
{
 if (conf->program != NULL) free(conf->program);
 if (conf->qc_password != NULL) free(conf->qc_password);
 if (conf->qc_issue != NULL) free(conf->qc_issue);
 return(OK);
}
