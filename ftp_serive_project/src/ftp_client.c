/*************************************************************************
	> File Name: socket.c
	> Author: yun
	> Mail: 1825752528@qq.com 
	> Created Time: 2019年08月01日 星期四 10时13分11秒
 ************************************************************************/

#include"ftp_client.h"
#define DEBUG
char buf[524];
int main(int argc,char *argv[])
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in  sa_addr;
	memset(&sa_addr,0,sizeof(sa_addr));
	
	sa_addr.sin_family = AF_INET;
	sa_addr.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &(sa_addr.sin_addr));
	socklen_t len = sizeof( sa_addr);
	
	int connect_t = connect(sock,(struct sockaddr *)&sa_addr,len);

	while(0 == connect_t)
	{
		char buf_cmd[512],buf_name[512];
		memset(buf_cmd,0,sizeof(buf_cmd));
		memset(buf_name,0,sizeof(buf_name));
		printf("ftp>");
		scanf("%s",buf_cmd);
		if(strncmp("ls",buf_cmd,2) == 0)
		{
			sen_ls(sock);
			continue;
		}
		else if(strncmp("bye",buf_cmd,3) == 0)
		{
			sen_bye(sock);
			 return ;
		}
		scanf("%s",buf_name);
//		printf("%s %s\n",buf_cmd,buf_name);
		 if(strncmp("put",buf_cmd,3) == 0)
		{
			printf("put\n");
			sen_put(sock,buf_name);
			
		}
		else if(strncmp("get",buf_cmd,3) == 0)
		{
			printf("get\n");
			sen_get(sock,buf_name);
		}
		 else if(strncmp("exit",buf_cmd,3) == 0)
		 {
			printf("exit\n");
			 sen_bye(sock);
		 }
//		printf("%s\n",buf);
	}

}

int sen_bye(int sock)
{
	memset(buf,0,sizeof(buf));
	char *p=buf;
	int cmd_int= CMD_BYE;
	int i=0;
	p[i++]=0x1d;
	p[i++]=0x2d;
	sprintf(p+2,"%4d",cmd_int);
	p+=6;	
	p[0]=0x2d;
	p[1]=0x1d;
//	printf("ss=%s,\n",p);
	i=0;
	write(sock,buf,sizeof(buf));
	return ;
	char buf2[524];
	read(sock,buf2,sizeof(buf2));
#ifdef DEBUG
	while(i < 524)
	{
		printf("%x\t",buf2[i++]);
	}
	printf("%s\n",buf2 + 10);
#endif
}

int sen_ls(int sock)
{
	memset(buf,0,sizeof(buf));
	char *p=buf;
	int cmd_int= CMD_LS;
	int i=0;
	p[i++]=0x1d;
	p[i++]=0x2d;
	sprintf(p+2,"%4d",cmd_int);
	p+=6;	
	p[0]=0x2d;
	p[1]=0x1d;
//	printf("ss=%s,\n",p);
	i=0;
	write(sock,buf,sizeof(buf));
	char buf2[524];
	read(sock,buf2,sizeof(buf2));
#ifdef DEBUG
	while(i < 524)
	{
		printf("%x\t",buf2[i++]);
	}
#endif
	printf("%s\n",buf2 + 10);
}
int sen_put(int sock, char *filename)
{
	memset(buf,0,sizeof(buf));
	char *p=buf;
	int cmd_get= CMD_PUT;
	int i=0;
	p[i++]=0x1d;
	p[i++]=0x2d;
	sprintf(p+2,"%4d",cmd_get);
	p+=6;	
	char *p_back=p;
	p+=4;
	int x = sprintf(p,"%s",filename);
	sprintf(p_back,"%03d",x);
	p+=x;
	p[0]=0x2d;
	p[1]=0x1d;
//	printf("ss=%s,x=%d\n",p,x);
	i=0;

	send(sock,buf,524,0);//发送先行段

	FILE *fp = fopen(filename,"a+");
	
	int num=1;
while(num!=0)
{	
	char buf[524];
	memset(buf,0,sizeof(buf));
	char *p=buf;
	int cmd_get= CMD_GET;
	 i=0;
	p[i++]=0x1d;
	p[i++]=0x2d;
	p+=2;
	sprintf(p,"%4d",cmd_get);
	p+=4;	
	char *p_back = p;
	p+=4;

//	printf("%s\n",filename);
	char buf2[512];
	num=fread(p,sizeof(char),512,fp);
	
	fflush(fp);
	//printf("ss=%s,i=%d\n",p,num);

	sprintf(p_back,"%03d",num);
	p+=num;
	p[0] = 0x2d;
	p[1] = 0x1d;
#ifdef DEBUG
	i=0;
	while(i<524)
	{
		printf("%x\t",buf[i]);
		i++;
	}
#endif
			write(sock,buf,524);
//	printf("**************%d*************\n",num);
	
}
	fclose(fp);


}

int  sen_get(int sock,char *filename)
{
	
	memset(buf,0,sizeof(buf));
	char *p=buf;
	int cmd_get= CMD_GET;
	int i=0;
	p[i++]=0x1d;
	p[i++]=0x2d;
	sprintf(p+2,"%4d",cmd_get);
	p+=6;	
	char *p_back=p;
	p+=4;
	int x = sprintf(p,"%s",filename);
	sprintf(p_back,"%03d",x);
	p+=x;
	p[0]=0x2d;
	p[1]=0x1d;
//	printf("ss=%s,x=%d\n",p,x);
	i=0;

	send(sock,buf,524,0);
	//write(sock,buf,524);
//	printf("%s\n",buf);

	char buf_rec[524]={0};
	memset(buf_rec,0,sizeof(buf_rec));
	fd_set rec_set;
	int max_fd = sock;
	FD_ZERO(&rec_set);
	FD_SET(sock,&rec_set);

	int rec_t = select(max_fd+1,&rec_set,&rec_set,NULL,NULL);
#ifdef DEBUG
	printf("%s\n",buf);
	printf("rec_t=%d\n",rec_t);
	printf("xx=%d\n",FD_ISSET(sock,&rec_set));
#endif
	int flag = 0;
	char c='A';
//	printf("%c\n",c);
	FILE *fp = fopen(filename,"ab+");
		char y;
	while(FD_ISSET(sock,&rec_set))
	{
		printf("xy\n");
		read(sock,&y,1);
		int size=1;
		while(-1 != y  )
		{
			while(0x1d != y)
			{
				read(sock,&y,1);
				//printf("%x\t",y);
			}
#ifdef DEBUG
#endif
			read(sock,&y,1);
			printf("y=%c\n",y);
			if(0x2d == y)
			{
				int i=4;
				while(i--)read(sock,&y,1);
				char buf[4];
				read(sock,&buf[0],1);
				read(sock,&buf[1],1);
				read(sock,&buf[2],1);
				read(sock,&buf[3],1);
				buf[3] = '\0';
				 size = atoi(buf);
				 if(0 == size)break;
				printf("sssszize=%d\n",size);
				i=0;
				char buf2[size];
#ifdef DEBUG
#endif
				while(i<size)
				{
					read(sock,&buf[i++],1);
				//	printf("read %x\t",buf[i-1]);
				}
				i=0;
				while(i<size)
				{
					int  res = fwrite(&buf[i],1,1,fp);
	//				 printf("res%d\n",res);
				printf("b%d=%x\t",i,buf[i++]);
				}
#ifdef DEBUG
				printf("size=%d\n",size);
#endif

			}else if(0x1d == y)
				{
					continue;

				}
		}
			printf("break\n");
			fclose(fp);
			break;
		}
}




