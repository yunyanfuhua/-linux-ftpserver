#include"ftp_serive.h"


int make_tcp_seriver(const char *ip,unsigned int port)
{
	//创建一个套接字
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	{
		perror("socket error");
		return  -1;
	}
	//2.初始化一个服务器的IPV4结构体
	struct sockaddr_in sAddr;
	memset(&sAddr,0,sizeof(sAddr));//内存设置
	sAddr.sin_family = AF_INET; //指定协议族  AF_INET
	sAddr.sin_port = htons(port);   //端口号
	//sAddr.sin_addr; //ip地址
	//inet_aton(argv[2],&(sAddr.sin_addr));
	sAddr.sin_addr.s_addr = inet_addr(ip);
	
	//3.bind。把一个地址结构体绑定到套接字上面去
	int  res = bind(sock, (struct sockaddr *)&sAddr,sizeof(sAddr));
	if(res == -1)
	{
		perror("bind error");
		return -1;
	}
	
	//4.listen 开启对一个套接字的监听
	listen(sock,MAX_LISTEN);
	
	return sock;
}

int rec_ls(int sock)
{
	DIR *dir = opendir(FTP_FILE);
	if(dir == NULL)
	{
		perror("opendir error");
		return -1;
	}
	struct  dirent *dirp = NULL; 
	char filename_buf[512];
	memset(filename_buf,0,sizeof(filename_buf));
	int r=0;
	while(dirp = readdir(dir))
	{
		if(!strcmp(dirp->d_name,".") || !strcmp(dirp->d_name,".."))
		{
			continue;
		}
		if(!strncmp(dirp->d_name,".",1))
		{
			continue;
		}

	   sprintf(filename_buf+r,"%s ",dirp->d_name);
	   r += strlen(dirp->d_name)+1;
//	   printf("%d\n",r);
	}
//	printf("file=%s\n",filename_buf);


	char buf[524];
	memset(buf,0,sizeof(buf));

	char *p = buf;
	*p++  = 0x1d;
	*p++  = 0x2d;
	int cmd_int = CMD_LS;
	sprintf(p,"%4d",cmd_int);
	p+=4;
	int filename_size = strlen(filename_buf);
	sprintf(p,"%3d",filename_size);
	p+=4;
	int i=0;
	while(i < filename_size)
	{
		*p = filename_buf[i++];
		p++;
	}
	*p = 0x2d;
	*p = 0x1d;
	write(sock,buf,sizeof(buf));
	
}

int rec_put(int sock,char *buf)
{
#ifdef DEBUG
	int xxx=0;
	while(xxx < 20)
	{
		printf("%x\n",buf[xxx++]);
	}
#endif

	char *p = buf;
	p+=6;
	char buf_size[4];
	buf_size[0] = *p++;
	buf_size[1] = *p++;
	buf_size[2] = *p++;
	buf_size[3] = *p++;


	int size = atoi(buf_size);
//	printf("size=%d\n",size);
	
#ifdef DEBUG
	printf("%d\n",size);
#endif
	char buf2[size+1];
	//read(sock,buf2,size);
	int i_buf_size = 0;
	while(i_buf_size < size)
	{
//		printf("p=%x\t",p[i_buf_size]);
		buf2[i_buf_size] = p[i_buf_size];
		i_buf_size++;
//		printf("b=%x\t",buf2[i_buf_size-1]);

	}
	//buf2[size] = '\0';
	i_buf_size = 0;
#ifdef DEBUG
	while(i_buf_size < size)
	{
		printf("b=%x\t",buf2[i_buf_size++]);
	}
#endif
	//printf("buf2=%s\n",buf2);

	char buf3[20] = {"2.txt"};
	char filename_buf[513];
	memset(filename_buf,0,sizeof(filename_buf));
	sprintf(filename_buf,"%s/%s",FTP_FILE,buf2);
//	printf("%s\n",filename_buf);
	FILE *fp = fopen(filename_buf,"a+");

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
	char y;
#ifdef DEBUG
			printf("size=\n");
			int i=0;
			while(1)
			{
				char y;
				read(sock,&y,1);
				printf("%x\t",y);
			}
#endif

		while(FD_ISSET(sock,&rec_set))
	{
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
			printf("size=\n");
#endif
		read(sock,&y,1);
//		printf("y=%c\n",y);
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
	//		printf("sssszize=%d\n",size);
			i=0;
			char buf2[size];
#ifdef DEBUG
			printf("size=\n");
#endif
			while(i<size)
			{
				read(sock,&buf[i++],1);
			}
			i=0;
			while(i<size)
			{
				int  res = fwrite(&buf[i],1,1,fp);
#ifdef DEBUF
				 printf("res%d\t",res);
				printf("b%d=%x\t",i,buf[i++]);
#endif
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

int rec_get(int sock,char *sen_buf)
{
	char *p_s = sen_buf;
	p_s+=6;
	char size_buf[4];
	size_buf[0] = *p_s;
	size_buf[1] = p_s[1];
	size_buf[2] = p_s[2];
	size_buf[3] = '\0';
	int size = atoi(size_buf);
	//printf("size=%d\n",size);
	int i=0;
#ifdef DEBUG

	while(i<20)
	{
		printf("%x\t",sen_buf[i++]);
	}
#endif
	p_s+=4;

	int j=0;
	char filename[512];
	i=size;
	while(i>0)
	{
		filename[j++] =	*p_s;
		p_s++;
		i--;
	}
	filename[j] = '\0';

	char filename_buf[513];
	memset(filename_buf,0,sizeof(filename_buf));
	sprintf(filename_buf,"%s/%s",FTP_FILE,filename);
	FILE *fp = fopen(filename_buf,"a+");
	
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

	printf("%s\n",filename_buf);
	char buf2[512];
	num=fread(p,sizeof(char),512,fp);
	
	fflush(fp);
	//printf("ss=%s,i=%d\n",p,num);

	sprintf(p_back,"%03d",num);
	p+=num;
	p[0] = 0x2d;
	p[1] = 0x1d;
	i=0;
	/*
	while(i<524)
	{
		printf("%x\t",buf[i]);
		i++;
	}*/
			write(sock,buf,524);
			/*
	printf("**************%d*************\n",num);
	*/
	
}
	fclose(fp);



}
