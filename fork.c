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
	int fp,i;
	char*token,*uri;
	fd_set readfds;
	int listenfd,connfd;
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
	for(;;)
	{
		connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&len);
		if(connfd==-1)
		{
			printf(" connect failed\n");
			continue;
		}
		if(fork()==0)
		{
			close(listenfd);
			memset(buff,0,2048);
			int ret=read(connfd,buff,2047);
			printf("%s\n",buff);		
			token=strtok(buff," ");
			uri=strtok(NULL," ");
			sprintf(file,".%s",uri);
			fp=open(file,O_RDONLY);
			if(fp==-1)
			{
				memset(buff,0,2048);
				strcpy(buff,"HTTP/1.0 404 NOTFOUND\r\n\r\n");
				write(connfd,buff,strlen(buff));
			}
			else
			{
				sprintf(buff,"HTTP/1.0 200 OK \r\nContent-Type:text/html\r\n\r\n");
				write(connfd,buff,strlen(buff));
				while((ret=read(fp,buff,2048))>0)
					write(connfd,buff,ret);
			}
			close(connfd);
			exit(0);
		}
		wait();
		close(connfd);
	}	
	return 0;
}
		
