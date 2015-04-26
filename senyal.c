#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/ipc.h> /* shmget() shmat() shmctl() */
#include <sys/shm.h> /* shmget() shmat() shmctl() */


#include "defines.h"              
#include "conf.h"

extern qcd_config *tmp_conf;
extern qcd_hijo *tmp_hijos;
extern int qsv_pid;
char *strsignal(int sig);

void muere_hijo(int sig)
{
 int wpid;
 wpid=wait(NULL);
 if(wpid==qsv_pid) {
  printf("%s%s-Servidor Terminando\n",REF_QCON,REF_QSV);
  qsv_pid=0;
 }
 else printf("%s-%s Recibida [PID:%d]\n",REF_QCON,strsignal(sig),wpid);
}
                                               
void sig_manejador(int sig)
{
 signal(sig,SIG_IGN);
 fprintf(stderr,"%s-%s recibida. Limpiando memoria (%s)\n",REF_QCON,strsignal(sig),(tmp_conf->id_mem)?"Servidor":"Hijo");
 shmdt(tmp_hijos);
 if (tmp_conf->id_mem>1) shmctl(tmp_conf->id_mem,IPC_RMID,0);  
 free_conf(tmp_conf);
 exit(0);
}

int qcd_initsignal(void)
{
 struct sigaction opciones;
 opciones.sa_handler=sig_manejador;
 opciones.sa_flags=SA_RESTART|SA_NOMASK;
 if (sigaction(SIGTERM,&opciones,NULL)<0) {perror("Desviando SIGTERM");exit(ERROR);}
 if (sigaction(SIGINT,&opciones,NULL)<0) {perror("Desviando SIGINT");exit(ERROR);}
 opciones.sa_handler=muere_hijo;
 if (sigaction(SIGCHLD,&opciones,NULL)<0) {perror("Desviando SIGINT");exit(ERROR);}

 if(signal(SIGPIPE,SIG_IGN)==SIG_ERR)
 {
  perror("Ignorando SIGPIPE");
  exit(ERROR);
 }
 return(0);                           
}                                                              