#ifndef __FTP_CLIENT__
#define __FTP_CLIEND__

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>

int sen_get(int ,char*);
int sen_put(int ,char*);
int sen_bye(int);
enum CMD_NO
{
	CMD_LS  = 1024,
	CMD_PUT,
	CMD_GET,
	CMD_BYE
};

#endif
