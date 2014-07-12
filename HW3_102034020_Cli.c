#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
/*
static void* run(void * arg){
	int connfd = *((int *)arg);
	free(arg);
	char buf[2048];
	int n;
	while(n = read(connfd, buf, sizeof(buf)){
		strtok(buf, " ");
		if(!strcmp(buf, "NA")){
			char name[1024];
			str
			FILE* fp = 
		}
	} 
}
static void* run1(void* arg){
	
}*/
int port, tcpfd;
struct memb{
	char name[1024];
	int sockfd;
	int port;
	//struct sockaddr_in memSock;
	int used;
};

struct Room{
	char roomName[1024];
	struct memb member[1024];
	int used;
}room[100];

struct memb self;

static void* tcpListener(void* arg){
	
	int connfd = *((int*)arg);
	free(arg);
	int n,i,j;
	char buf[1024];
	pthread_detach(pthread_self()) ;
	printf("thread start\n");
	while((n = read(connfd, buf, sizeof(buf)))>0){
		if(buf[0] == 'K' && buf[1] == 'K'){
			char tmp[1024];
			strcpy(tmp , buf);
			char* s= strtok(tmp, " ");
			char ori[1024];
			s = strtok(NULL, " ");
			strcpy(ori, s);
			char roomName[1024];
			s= strtok(NULL, " ");
			strcpy(roomName, s);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(!strcmp(room[i].member[j].name, ori)){
							room[i].member[j].used = 0;
							goto S5;
						}
					}
				}
			}
			S5:;
			printf("%s leaves %s\n", ori, roomName);
		}
		else if(buf[0] == 'G' && buf[1] == 'G'){
			char tmp[1024];
			strcpy(tmp , buf);
			char* s= strtok(tmp, " ");
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			char ori[1024];
			s = strtok(NULL, " ");
			strcpy(ori, s);
			char file[1024];
			s = strtok(NULL, " ");
			strcpy(file, s);
			char sendMsg[1024000];
			FILE* fp = fopen(file, "r");
			
			char fileContent[2000000];
			fread(fileContent, 1, sizeof(fileContent), fp);
			fclose(fp);
			sprintf(sendMsg, "FF#$#$#$#$#%s#$#$#$#$#%s", file, fileContent);
			puts(sendMsg);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(!strcmp(room[i].member[j].name, ori)){
							write(room[i].member[j].sockfd, sendMsg, strlen(sendMsg));
							goto S8;
						}
					}
				}
			}
			S8:;
			
			
		}
		else if(buf[0] == 'F' && buf[1] == 'F'){
			char tmp[1024000];
			strcpy(tmp, buf);
			char* s= strtok(tmp, "#$#$#$#$#");
			s = strtok(NULL,"#$#$#$#$#");
			char fileName[1024];
			strcpy(fileName, s);
			FILE* fp = fopen(fileName, "w");
			char fileContent[1024000];
			s  = strtok(NULL, "#$#$#$#$#");
			strcpy(fileContent, s);
			fwrite(fileContent, 1, strlen(fileContent), fp);
			fclose(fp);
		}
		else {
			puts(buf);
			
		}
		memset(buf, 0, sizeof(buf));
	}
	return (NULL);
}


static void* udpListener(void* arg){
	pthread_detach(pthread_self()) ;
	struct sockaddr_in servaddr;
	int sockfd, i, j;
	pthread_t tid, tid2;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	int n;
	socklen_t udpCliLen = sizeof(servaddr);
	char buff[1024];
	while(1){
		memset(buff, 0, sizeof(buff));
		n = recvfrom(sockfd, buff, 1024, 0, (struct sockaddr*)&servaddr, &udpCliLen);
		buff[n-1] = 0;
		buff[n] = 0;
		if(buff[0] == 'Y' && buff[1] == 'Y'){
			printf("buff = %s\n", buff);
			char tmp[1024];
			strcpy(tmp, buff);
			char* s = strtok(tmp, " ");
			s = strtok(NULL, " ");
			char roomName[1024];
			strcpy(roomName, s);
			s = strtok(NULL, " ");
			int tcpPort = strtol(s, NULL, 10);
			char sendMsg[2048];
			memset(sendMsg, 0, sizeof(sendMsg));
			sendMsg[0] = 'Z';
			sendMsg[1] = 'Z';
			sendMsg[2] = ' ';
			strcat(sendMsg, roomName);
			puts(roomName);
			strcat(sendMsg, " "); 
			printf("1sendMsg (YY): %s\n", sendMsg);
			for(i=0;i<100;i++){
				//printf("%s %s\n", roomName, room[i].roomName);
				if(!strcmp(room[i].roomName, roomName) && room[i].used){
					printf("hello\n");
					for(j=0;j<1024;j++){
						if(room[i].member[j].used){
							strcat(sendMsg, room[i].member[j].name);
							strcat(sendMsg, " ");
							char tt[100];
							sprintf(tt, "%d", room[i].member[j].port);
							strcat(sendMsg, tt);
							strcat(sendMsg, " ");
						}
					}
				}
			}
			printf("sendMsg (YY): %s\n", sendMsg);
			int tmpfd = socket(AF_INET, SOCK_DGRAM, 0);
			struct sockaddr_in dest;
			bzero(&dest, sizeof(dest));
			dest.sin_port = htons(tcpPort);
			dest.sin_family = AF_INET;
			dest.sin_addr.s_addr = inet_addr("127.0.0.1");
			socklen_t sockLen = sizeof(dest);
			sendto(sockfd, sendMsg, strlen(sendMsg), 0, (struct sockaddr*)&dest, sockLen);
			
		}
		else if(buff[0] == 'Z' && buff[1] == 'Z'){
			printf("buff = %s\n", buff);
			char list[100][2][100];
			int ptr=0;
			char tmp[1024];
			strcpy(tmp, buff);
			char *s =strtok(tmp, " ");
			s = strtok(NULL," ");
			char roomName[1024];
			strcpy(roomName, s);
			puts(roomName);
			s = strtok(NULL, " ");
			while(s!=NULL){
				strcpy(list[ptr][0],s);
				s = strtok(NULL, " ");
				strcpy(list[ptr++][1], s);
				s = strtok(NULL, " ");
			}
			
			int i, j, roomNum;
			for(j=0;j<100;j++){
				if(!room[j].used){
					strcpy(room[j].roomName, roomName);
					strcpy(room[j].member[0].name,  self.name);
					room[j].member[0].sockfd = 0;
					room[j].member[0].port = port;
					room[j].member[0].used = 1;
					room[j].used = 1;
					roomNum = j;
					break;
				}
			}
			
			
			
			int ptr1=1;
			struct sockaddr_in cliaddr;
			int tcpClilen = sizeof(cliaddr);
			char sendMsg[1024];
			for(i=0;i<ptr;i++){
				int clifd, connfd;
				struct sockaddr_in cliendaddr;
				clifd = socket(AF_INET, SOCK_DGRAM, 0);
				bzero(&cliendaddr, sizeof(cliendaddr));
				cliendaddr.sin_port = htons(strtol(list[i][1],NULL, 10));
				cliendaddr.sin_family = AF_INET;
				cliendaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
				sprintf(sendMsg, "XX %s %d %s\n", self.name, port, roomName);
				
				sendto(clifd, sendMsg, strlen(sendMsg), 0, (struct sockaddr*)&cliendaddr, sizeof(cliendaddr));
				connfd = accept(tcpfd, (struct sockaddr*)&cliaddr, &tcpClilen);
				
				int acceptfd;
				struct sockaddr_in dest;
				acceptfd = socket(AF_INET, SOCK_STREAM, 0);
				memset(&dest, 0, sizeof(dest));
				dest.sin_port = htons(strtol(list[i][1], NULL, 10));
				dest.sin_family = AF_INET;
				dest.sin_addr.s_addr = inet_addr("127.0.0.1");
				sleep(5);
				
				connect(acceptfd, (struct sockaddr*)&dest, sizeof(dest));
				int * tt1;
				tt1 = malloc(sizeof(int));
				*tt1 = acceptfd;
				pthread_create(&tid2, NULL, &tcpListener, tt1);

				
				printf("to here\n");
				strcpy(room[roomNum].member[ptr1].name, list[i][0]);
				room[roomNum].member[ptr1].port = strtol(list[i][1], NULL, 10);
				room[roomNum].member[ptr1].sockfd = connfd;
				room[roomNum].member[ptr1++].used = 1;
			}
		}
		else if(buff[0] == 'X' && buff[1] == 'X'){
			//sleep(1);
			char tmp[1024];
			strcpy(tmp, buff);
			char* s  =strtok(tmp, " ");
			s = strtok(NULL, " ");
			char origin[1024];
			strcpy(origin, s);
			s = strtok(NULL, " ");
			int originport = strtol(s, NULL, 10);
			s = strtok(NULL, " ");
			char roomName[1024];
			strcpy(roomName, s);
			printf("%s\n", roomName);
			int *acceptfd;
			acceptfd = malloc(sizeof(int));
			struct sockaddr_in dest;
			*acceptfd = socket(AF_INET, SOCK_STREAM, 0);
			memset(&dest, 0, sizeof(dest));
			dest.sin_port = htons(originport);
			dest.sin_family = AF_INET;
			dest.sin_addr.s_addr = inet_addr("127.0.0.1");
			connect(*acceptfd, (struct sockaddr*)&dest, sizeof(dest));
			printf("to here\n");
			
			pthread_create(&tid, NULL, &tcpListener, acceptfd);
			// pthread
			printf("to there\n");
			struct sockaddr_in cliaddr;
			int tcpClilen = sizeof(cliaddr);
			int connfd = accept(tcpfd, (struct sockaddr*)&cliaddr, &tcpClilen);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(!room[i].member[j].used){
							strcpy(room[i].member[j].name, origin);
							room[i].member[j].port = originport;
							room[i].member[j].sockfd = connfd;
							room[i].member[j].used = 1;
							room[i].used = 1;
							printf("Add %s\n", origin);
							goto S1;
						}
					}
				}
			}
			S1:;
		}
		else{
			puts(buff);
		}
	}
	return (NULL);
}




int main(int argc, char* argv[]){
	int sockfd;
	int n, i, j;
	pthread_t tudp, ttcp;
	struct sockaddr_in servaddr, tcpServer;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_port = htons(strtol(argv[2], NULL, 10));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	char msg[1024];
	char tmp[1024];

	while(1){
		gets(msg);
		strcpy(tmp, msg);
		char* s = strtok(tmp, " ");
		if(!strcmp(s, "NA")){
			puts(msg);
			write(sockfd, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			n = read(sockfd, msg, sizeof(msg));
			puts(msg);
		}
		else if(!strcmp(s, "DA")){
			write(sockfd, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			n = read(sockfd, msg, sizeof(msg));
			puts(msg);
		}
		else if(!strcmp(s, "I")){
			s = strtok(NULL, " ");
			strcpy(self.name, s); 
			puts(msg);
			write(sockfd, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			n = read(sockfd, msg, sizeof(msg));
			if(strtol(msg, NULL, 10) == 0){
				printf("Login failed\n");
				continue;
			}
			else{
				port = strtol(msg, NULL, 10);
				pthread_create(&tudp, NULL, &udpListener, NULL);
				tcpfd = socket(AF_INET,SOCK_STREAM,0);
				bzero(&tcpServer,sizeof(tcpServer));
				tcpServer.sin_port = htons(port);
				tcpServer.sin_family = AF_INET;
				tcpServer.sin_addr.s_addr = htonl(INADDR_ANY);
				bind(tcpfd, (struct sockaddr*)&tcpServer, sizeof(tcpServer));
				listen(tcpfd,100);
			}
		}
		else if(!strcmp(s, "O")){
			puts(msg);
			write(sockfd, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			n = read(sockfd, msg, sizeof(msg));
			puts(msg);
		}
		else if(!strcmp(s, "OL")){
			write(sockfd, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			n = read(sockfd, msg, sizeof(msg));
			puts(msg);
		}
		
		else if(!strcmp(s, "NR")){
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			for(i = 0;i<100;i++){
				if(!room[i].used){
					strcpy(room[i].roomName, roomName);
					strcpy(room[i].member[0].name, self.name);
					room[i].member[0].sockfd = 0;
					room[i].member[0].port =port;
					room[i].member[0].used =1 ;
					room[i].used = 1;
					
					printf("Create room successfully\n");
					break;
				}
			}
		}
		else if(!strcmp(s, "IP")){
			printf("IP mode\n");
			s = strtok(NULL, " ");
			char roomName[1024];
			strcpy(roomName, s);
			s = strtok(NULL, " ");
			char destUser[1024];
			strcpy(destUser, s);
			int destPort;
			s = strtok(NULL, " ");
			destPort = strtol(s, NULL, 10);
			struct sockaddr_in dest;
			int socketfd;
			char query[1024];
			socketfd = socket(AF_INET, SOCK_DGRAM, 0);
			bzero(&dest, sizeof(dest));
			dest.sin_port = htons(destPort);
			dest.sin_family = AF_INET;
			dest.sin_addr.s_addr = inet_addr("127.0.0.1");
			sprintf(query, "%s invite you to %s (%d)\n", self.name, roomName, port);
			printf("%s", query);
			sendto(socketfd, query, strlen(query), 0, (struct sockaddr*)&dest, sizeof(dest));
		}
		else if(!strcmp(s, "A")){
			s = strtok(NULL, " ");
			char destUser[1024];
			strcpy(destUser, s);
			s = strtok(NULL, " ");
			char roomName[1024];
			strcpy(roomName, s);
			s = strtok(NULL, " ");
			int destPort = strtol(s, NULL, 10);
			s = strtok(NULL, " ");
			char Ans = s[0];
			
			struct sockaddr_in dest;
			int socketfd;
			char query[1024];
			socketfd = socket(AF_INET, SOCK_DGRAM, 0);
			bzero(&dest, sizeof(dest));
			dest.sin_port = htons(destPort);
			dest.sin_family = AF_INET;
			dest.sin_addr.s_addr = inet_addr("127.0.0.1");
			if(Ans == 'Y'){
				sprintf(query, "%c%c %s %d\n", 'Y','Y', roomName, port);
				printf("%s", query);
				sendto(socketfd, query, strlen(query), 0, (struct sockaddr*)&dest, sizeof(dest));
			}
		}
		else if(!strcmp(s, "SR")){
			for(i=0;i<100;i++){
				if(room[i].used){
					printf("%s\n", room[i].roomName);
					for(j=0;j<1024;j++){
						if(room[i].member[j].used){
							printf("%s %d %d\n", room[i].member[j].name, room[i].member[j].port, room[i].member[j].sockfd);
						}
					}
					printf("-------------------------------\n");
				}
			}
		}
		//[S] [Room ID] [Message]
		else if(!strcmp(s, "S")){
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			char msg[1024];
			s = strtok(NULL, " ");
			strcpy(msg, s);
			char sendMsg[1024];
			sprintf(sendMsg, "%s said [%s] in %s", self.name, msg, roomName);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(room[i].member[j].used && strcmp(room[i].member[j].name, self.name)){
							printf("%s\n", sendMsg);
							write(room[i].member[j].sockfd, sendMsg, strlen(sendMsg));
						}
					}
				}
			}
		}
		else if(!strcmp(s, "P")){
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			char msg[1024];
			s = strtok(NULL, " ");
			strcpy(msg, s);
			char sendMsg[1024];
			sprintf(sendMsg, "%s post [%s] in %s", self.name, msg, roomName);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(room[i].member[j].used && strcmp(room[i].member[j].name, self.name)){
							printf("%s\n",sendMsg);
							write(room[i].member[j].sockfd, sendMsg, strlen(sendMsg));
						}
					}
				}
			}
		}
		else if(!strcmp(s, "G")){ // G roomName User file Y
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			s = strtok(NULL, " ");
			char destUser[1024];
			strcpy(destUser,s);
			char msg[1024];
			s = strtok(NULL, " ");
			strcpy(msg, s);
			s = strtok(NULL, " ");
			char Ans = s[0];
			if(Ans == 'Y'){
				for(i=0;i<100;i++){
					if(!strcmp(room[i].roomName, roomName) && room[i].used){
						for(j=0;j<1024;j++){
							if(!strcmp(destUser, room[i].member[j].name)){
								char query[1024];
								char getMsg[1024];
								sprintf(query, "GG %s %s %s", roomName, self.name, msg);
								printf("%s\n", query);
								write(room[i].member[j].sockfd, query, strlen(query));
							}
						}
					}
				}
			}
		}
		else if(!strcmp(s, "L")){
			char roomName[1024];
			s = strtok(NULL, " ");
			strcpy(roomName, s);
			char sendMsg[1024];
			sprintf(sendMsg, "KK %s %s", self.name, roomName);
			for(i=0;i<100;i++){
				if(!strcmp(room[i].roomName, roomName)){
					for(j=0;j<1024;j++){
						if(room[i].member[j].used && room[i].member[j].sockfd){
							write(room[i].member[j].sockfd, sendMsg, strlen(sendMsg));
						}
						else if(!strcmp(room[i].member[j].name, self.name)){
							room[i].member[j].used = 0;
						}
					}
					room[i].used = 0;
				}
			}
		}                            
	}
	
}


