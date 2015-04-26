#ifndef _CONF_H_
#define _CONF_H_

#include "defines.h"

int qcd_initconf(qcd_config *conf);
int linea(char *linea,qcd_config *conf);
int conf_check(qcd_config *conf);
int init_conf(qcd_config *conf);
int free_conf(qcd_config *conf);
int conf_print(qcd_config *conf);

#endif
