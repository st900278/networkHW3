#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
struct onlineMember{
	int connfd;
	struct sockaddr_in socketaddr;
	char name[1024];
	int port;
	int exist;
}member[1024];

int memberNum = 1024;
struct clientsock{
	int clifd;
	struct sockaddr_in cliaddr;
};

static void* run(void * arg){
	int connfd = *((int *)arg);
	struct clientsock  clisock = *((struct clientsock*)arg);
	free(arg);
	char buf[2048];
	char msg[2048];
	printf("%s\n", inet_ntoa(clisock.cliaddr.sin_addr));
	int n;
	int i,j;
	while(1){
		memset(buf, 0, sizeof(buf));
		memset(msg, 0, sizeof(msg));
		n = read(clisock.clifd, buf, 2048);
		puts(buf);
		char* tmp = strtok(buf, " ");
		if(!strcmp(tmp, "NA")){
			char name[1024];
			char password[1024];
			char fileName[1024];
			tmp = strtok(NULL, " ");
			strcpy(name, tmp);
			puts(name);
			strcpy(fileName, name);
			strcat(fileName, ".txt");
			puts(fileName);
			tmp = strtok(NULL, " ");
			strcpy(password, tmp);
			puts(password);
			FILE* fp = fopen(fileName, "w");
			if(fp == NULL){
				printf("Create file failed\n");
				sprintf(msg, "Create file failed\n");
				write(clisock.clifd, msg, strlen(msg));
				continue;
			}
			fputs(name, fp);
			fputs("\n", fp);
			fputs(password, fp);
			sprintf(msg, "Create Account successfully\n");
			write(clisock.clifd, msg, strlen(msg));
			fclose(fp);
		}
		else if(!strcmp(tmp, "DA")){
			char name[1024];
			char password[1024];
			char fileName[1024];
			tmp = strtok(NULL, " ");
			strcpy(name, tmp);
			tmp = strtok(NULL, " ");
			strcpy(password, tmp);
			strcpy(fileName, name);
			strcat(fileName, ".txt");
			FILE* fp = fopen(fileName, "r");
			if(fp == NULL){
				printf("Can't find the file\n");
				sprintf(msg, "Can't find the filen");
				write(clisock.clifd, msg, strlen(msg));
				continue;
			}
			fgets(msg, 2048, fp);
			fgets(msg, 2048, fp);
			if(!strcmp(msg, password)){
				fclose(fp);
				remove(fileName);
				sprintf(msg, "Delete Account successfully\n");
				write(clisock.clifd, msg, strlen(msg));
			}
			else{
				fclose(fp);
				sprintf(msg, "Delete Account failed\n");
				write(clisock.clifd, msg, strlen(msg));
			}
		}
		else if(!strcmp(tmp, "I")){
			char name[1024];
			char password[1024];
			char fileName[1024];
			char tmp1[1024];
			int port;
			tmp = strtok(NULL, " ");
			strcpy(name, tmp);
			tmp = strtok(NULL, " ");
			strcpy(password, tmp);
			tmp = strtok(NULL, " ");
			port = strtol(tmp, NULL, 10);
			strcpy(fileName, name);
			strcat(fileName, ".txt");
			FILE* fp = fopen(fileName, "r");
			if(fp == NULL){
				printf("Can't find the file\n");
				sprintf(msg, "Can't find the file\n");
				write(clisock.clifd, msg, strlen(msg));
				continue;
			}
			fgets(msg, 2048, fp);
			fgets(msg, 2048, fp);
			if(!strcmp(msg, password)){
				fclose(fp);
				sprintf(msg, "%d\n", port);
				write(clisock.clifd, msg, strlen(msg));
				for(i=0;i<memberNum;i++){
					if(!member[i].exist){
						strcpy(member[i].name, name);
						member[i].connfd = clisock.clifd;
						member[i].socketaddr = clisock.cliaddr;
						member[i].port = port;
						member[i].exist = 1;
						break;
					}
				}
				
			}
			else{
				fclose(fp);
				sprintf(msg, "0\n");
				write(clisock.clifd, msg, strlen(msg));
			}
			
		}
		else if(!strcmp(tmp, "O")){
			char name[1024];
			char password[1024];
			char fileName[1024];
			tmp = strtok(NULL, " ");
			strcpy(name, tmp);
			tmp = strtok(NULL, " ");
			strcpy(password, tmp);
			strcpy(fileName, name);
			strcat(fileName, ".txt");
			FILE* fp = fopen(fileName, "r");
			if(fp == NULL){
				printf("Can't find the file\n");
				sprintf(msg, "Can't find the file\n");
				write(clisock.clifd, msg, strlen(msg));
				continue;
			}
			fgets(msg, 2048, fp);
			fgets(msg, 2048, fp);
			if(!strcmp(msg, password)){
				fclose(fp);
				
				for(i=0;i<memberNum;i++){
					if(!strcmp(member[i].name, name)){
						member[i].exist =0;
						sprintf(msg, "Logout\n");
						write(clisock.clifd, msg, strlen(msg));
						break;
					}
				}
			}
			else{
				fclose(fp);
				sprintf(msg, "No permission\n");
				write(clisock.clifd, msg, strlen(msg));
			}
		}
		else if(!strcmp(tmp, "OL")){
			for(i=0;i<memberNum;i++){
				if(member[i].exist){
					strcat(msg, member[i].name);
					strcat(msg," ");
					strcat(msg,  inet_ntoa(member[i].socketaddr.sin_addr));
					strcat(msg, " ");
					char t[100];
					sprintf(t, "%d", member[i].port);
					strcat(msg, t);
					strcat(msg, "\n");
				}
			}
			write(clisock.clifd, msg, strlen(msg));
		}
	} 
}

int main(int argc, char* argv[]){
	int sockfd;
	pthread_t tid;
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_port = htons(strtol(argv[1], NULL, 10));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(sockfd, 100);
	
	while(1){
		struct clientsock* clisock; 
		clisock = malloc(sizeof(struct clientsock));
		printf("test\n");
		int addrlen = sizeof(clisock->cliaddr);
		clisock->clifd = accept(sockfd, (struct sockaddr*)&(clisock->cliaddr), &addrlen);
		printf("%s\n", inet_ntoa(clisock->cliaddr.sin_addr));
		printf("test1\n");
		pthread_create(&tid, NULL, &run, (void *)clisock);
	}	
	
}


