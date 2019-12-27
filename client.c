#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	while(1){

		char data[512];
		int read = recv(clientSocket,data,512,0);
		if(read>0){
		//	data[read] = '\0';
			printf("%s\n",data);
		}

	}

}

int main(){

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(2010);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("|-----------Connection Successfully Established ----------|\n");

	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	while(1){

		char input[512];
		scanf("%s",input);

		if(strcmp(input,"SHOW") == 0){

			send(clientSocket,input,512,0);

		}
		else if(strcmp(input,"SEND") == 0){

			send(clientSocket,input,512,0);
			
			scanf("%s",input);
			char i[512];
			strcpy(i,input);
			send(clientSocket,input,512,0);
			
			scanf("%[^\n]s",input);
			printf("Sending message to client : %s\n",i);
			printf("Message: %s\n", input);
			send(clientSocket,input,512,0);

		}
		else if(strcmp(input,"EXIT") == 0){
			printf("Client exited\n");
			send(clientSocket,input,512,0);
			close(clientSocket);
			break;
		}
		else{
			scanf("%[^\n]s",input);
			printf("Invalid Input, try again\n");

		}

	}


}