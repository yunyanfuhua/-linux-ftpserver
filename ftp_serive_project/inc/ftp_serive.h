#ifndef __FTP_SERIVE__
#define __FTP_SERIVE__

#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include <sys/stat.h>
#include<fcntl.h>



#define FTP_FILE "/home/yun/tftp"
#define MAX_LISTEN 10
#define MAX_SIZE 524
enum CMD_NO
{
	CMD_LS  = 1024,
	CMD_PUT,
	CMD_GET,
	CMD_BYE
};

//协议
//头两个字节 0x1d,0x2d 其他则为乱码
//CMD_int 命令号四个字节
//SIZE 四个字节本次传输最大字节数512,前三个有效，后一个填充
//buf[512]
//末尾结束0x2d,0x1d

int  make_tcp_seriver(const char *ip , unsigned int );

int rec_ls(int sock);

int rec_get(int sock,char *filename);

int rec_bye(int sock);

int rec_put(int sock,char *buf);

#endif
