#include<stdio.h>

#include<stdlib.h>

#include<unistd.h>

#include<string.h>

#include<sys/types.h>

#include<sys/socket.h>

#include<arpa/inet.h>

#include<netinet/in.h>

#include<netdb.h>

#include<errno.h>

void error(char* msg)

{

perror(msg);

exit(0);

}

int main(int argc,char* argv[])

{

pid_t id;

struct sockaddr_in rdd_in,client,server;

struct hostent* hostname;

int sockd,newsockd;

if(argc<2)

error("./proxy <port_no>");

printf("\n* proxy server started *\n");

bzero((char*)&server,sizeof(server));

bzero((char*)&client, sizeof(client));

server.sin_family=AF_INET;

server.sin_port=htons(atoi(argv[1]));

server.sin_addr.s_addr=INADDR_ANY;

sockd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

if(sockd<0)

error("Problem in initializing socket");

if(bind(sockd,(struct sockaddr*)&server,sizeof(server))<0)

error("Error on binding");

listen(sockd,50);

int clientlen=sizeof(client);

accept:

newsockd=accept(sockd,(struct sockaddr*)&client,&clientlen);

if(newsockd<0)

error("Problem to accept connection");

id=fork();

if(id==0)

{

struct sockaddr_in hostaddr;

int flagvalue=0,newsockfd,m,portno=0,j,sockfd;

char buffervalue[610],p1[400],p2[400],p3[50];

char* temp=NULL;

bzero((char*)buffervalue,600);

recv(newsockd,buffervalue,600,0);

sscanf(buffervalue,"%s %s %s",p1,p2,p3);

if(((strncmp(p1,"GET",3)==0))&&((strncmp(p3,"HTTP/1.1",8)==0)||(strncmp(p3,"HTTP/1.0",8)==0))&&(strncmp(p2,"http://",7)==0))

{

strcpy(p1,p2);

flagvalue=0;

for(j=7;j<strlen(p2);j++)

{

if(p2[j]==':')

{

flagvalue=1;

break;

}

}

temp=strtok(p2,"//");

if(flagvalue==0)

{

portno=80;

temp=strtok(NULL,"/");

}

else

{

temp=strtok(NULL,":");

}

sprintf(p2,"%s",temp);

printf("hostname = %s",p2);

hostname=gethostbyname(p2);

if(flagvalue==1)

{

temp=strtok(NULL,"/");

portno=atoi(temp);

}

strcat(p1,"^]");

temp=strtok(p1,"//");

temp=strtok(NULL,"/");

if(temp!=NULL)

temp=strtok(NULL,"^]");

printf("\npath = %s\nPortno = %d\n",temp,portno);

bzero((char*)&hostaddr,sizeof(hostaddr));

hostaddr.sin_port=htons(portno);

hostaddr.sin_family=AF_INET;

bcopy((char*)hostname->h_addr,(char*)&hostaddr.sin_addr.s_addr,hostname->h_length);
sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
newsockfd=connect(sockfd,(struct sockaddr*)&hostaddr,sizeof(struct sockaddr));
sprintf(buffervalue,"\nConnected to %s  IP - %s\n",p2,inet_ntoa(hostaddr.sin_addr));
if(newsockfd<0)
error("Error in connecting to remote server");
printf("\n%s\n",buffervalue);
bzero((char*)buffervalue,sizeof(buffervalue));
if(temp!=NULL)
sprintf(buffervalue,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,p3,p2);
else
sprintf(buffervalue,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",p3,p2);
m=send(sockfd,buffervalue,strlen(buffervalue),0);
printf("\n%s\n",buffervalue);
if(m<0)
error("Error writing to socket");
else{
do
{
bzero((char*)buffervalue,600);
m=recv(sockfd,buffervalue,600,0);
if(!(m<=0))
send(newsockd,buffervalue,m,0);
}while(m>0);
}
}
else
{
send(newsockd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0);
}
close(sockfd);
close(newsockd);
close(sockd);

_exit(0);
}
else
{
close(newsockd);
goto accept;
}
return 0;
}
