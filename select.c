#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc,char**argv)
{
	char *token,*uri;
	fd_set readfds;
	int listenfd,connfd,maxfd,fp;
	struct sockaddr_in servaddr,clientaddr;
	socklen_t len=sizeof(clientaddr);
	char buff[2048],file[100];
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=INADDR_ANY;
	servaddr.sin_port=htons(80);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,10);
	maxfd=listenfd;
	for(;;)
	{
		FD_ZERO(&readfds);
		FD_SET(listenfd,&readfds);
		int ret=select(maxfd+1,&readfds,NULL,NULL,NULL);
		if(ret==-1)
		{
			printf("select error\n");
			continue;
		}
		if(FD_ISSET(listenfd,&readfds))
		{
			connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&len);
			if(connfd==-1)
			{
				printf("socket accept error\n");
				continue;
			}
		}
		FD_SET(connfd,&readfds);
		if(FD_ISSET(connfd,&readfds))
		{
			memset(buff,0,2048);
			read(connfd,buff,2047);
			printf("%s\n",buff);
			
			token=strtok(buff," ");
			uri=strtok(NULL," ");
			sprintf(file,".%s",uri);
			fp=open(file,O_RDONLY);
			if(fp==-1)
			{
				strcpy(buff,"HTTP/1.0 404 NOT FOUND\r\n\r\n");
				write(connfd,buff,strlen(buff));
			}
			else
			{
				memset(buff,0,2048);
				sprintf(buff,"HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n");
				write(connfd,buff,strlen(buff));
				while((ret=read(fp,buff,2048))>0)
					write(connfd,buff,ret);
			}
		}
		close(connfd);
	}	
	return 0;
}
		
