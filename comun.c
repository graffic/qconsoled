#include <ctype.h>
#include <stdio.h>
#include <string.h>

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
