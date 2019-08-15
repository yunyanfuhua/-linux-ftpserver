#include"ftp_serive.h"
char buf[524];
int main(int argc,char *argv[])
{
	int sockfd = make_tcp_seriver(argv[1],atoi(argv[2]));
	int  make_tcp_seriver(const char *ip , unsigned int );

		

	int max_fd;//保存要监听的文件描述符的最大值
	fd_set set;//监听文件描述符可读的集合
	
	max_fd = sockfd;//暂时还没有其他的客户端，最大值就是服务器的fd

		FD_ZERO(&set);
		FD_SET(sockfd,&set);
		int ret = select(max_fd+1,&set, NULL,NULL,NULL);
		while(FD_ISSET(sockfd,&set))//服务器就绪
		{
			struct sockaddr_in cAddr; //表示保存客户端的地址结构体
			socklen_t addrlen = sizeof(cAddr);
			int new_fd = accept(sockfd, (struct sockaddr *)&cAddr, &addrlen);

			printf("start\n",buf);
			while(1)
			{

				memset(buf,0,sizeof(buf));
				int n = read(new_fd,buf,524);
				int i=0;
				while(i < 524)
				{
					printf("%x\t",buf[i++]);
				}

				if(buf[0] == 0x1d || buf[1] == 0x2d)
				{
					int cmd = (buf[2]-'0')*1000 + (buf[3]-'0')*100 + (buf[4]-'0')*10 + (buf[5]-'0');
					printf("cmd=%d\n",cmd);
					if(CMD_LS == cmd)
					{
						rec_ls(new_fd);
						printf("ls\n");
					}
					else if(CMD_GET == cmd)
					{
						rec_get(new_fd,buf);
						printf("get\n");
					}
					else if(CMD_PUT == cmd)
					{
						rec_put(new_fd,buf);
						printf("put\n");
					}
					else if(1027 == cmd)
					{
						printf("bye\n");
						break ;
					}
				}
				else
				{
				}
			}	


		}
}
