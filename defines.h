/* Qconsoled file         */
#ifndef _QC__DEFINES_H
#define _QC__DEFINES_H

#define 	QC_WELCOME	"\r                        Welcome/Bienvenido a\r\n__________________________________________________________________\r\n      __                                                    _____\r\n    /    )                                      /           /    )\r\n---/----/-----__-----__-----__----__-----__----/-----__----/----/-\r\n  /  \\ /    /   '  /   )  /   )  (_   /   )  /    /___)  /    /\r\n_(____X____(___ __(___/__/___/__(__)__(___/__/____(___ __/____/___\r\n       \\            Terminal Ready!\r\n"
                          
#define 	OK 		0
#define		ERROR		1
#define		NONBLOCK_FAIL	2
#define		CLOSE_ERROR	3
#define		ACCEPT_ERROR	4
#define		NETINIT_ERROR 	5
#define		CONF_F_ERROR	6

#define		REF_QCON	"[\x1B[36mQconsoled\x1B[0m]"
#define		REF_QSV		"[\x1B[35mQServer\x1B[0m]"
#define		REF_QCL		"[\x1B[34mQClient\x1B[0m]"

#define		MSG_DAT		100 /* Data message */
#define		MSG_INF		200 /* Information message */
#define		MSG_SRV		300 /* Server status message */
#define		MSG_SRV_ALREADY	301 /* Forced server status message */

#define		DEBUG__

typedef struct{
	int pid;
	int llegan[2];
	int salen;
	int socket;
}qcd_hijo;

typedef struct{
	/* Conf file */
	char *program;
	char *qc_password;
	int qc_consoles;
	int qc_port;
	char *qc_issue;
	int id_mem;
}qcd_config;
#endif
