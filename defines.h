/* Qconsoled file         */
#ifndef _DEFINES_H
#define _DEFINES_H

#define		QSV_DIR		"/fat32/temp/"
#define		QSV_RUTA	"/fat32/temp/qwsv"
#define		QSV_PARAM	"qwsv"
#define		QC_PORT		10100
#define		QC_MAXCONNECT   5
#define 	QC_WELCOME	"                        Welcome/Bienvenido a\n__________________________________________________________________\n      __                                                    _____\n    /    )                                      /           /    )\n---/----/-----__-----__-----__----__-----__----/-----__----/----/-\n  /  \\ /    /   '  /   )  /   )  (_   /   )  /    /___)  /    /\n_(____X____(___ __(___/__/___/__(__)__(___/__/____(___ __/____/___\n       \\            Terminal Ready!\n"
                          
#define 	OK 		0
#define		NONBLOCK_FAIL	-1
#define		CLOSE_ERROR	-2
#define		ACCEPT_ERROR	-3
#define		NETINIT_ERROR 	-4
typedef struct{
	int pid;
	int pipe[2];
	int socket;
}qcd_hijo;

#endif
