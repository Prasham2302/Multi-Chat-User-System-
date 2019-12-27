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

int clientCount = 0;
int start_counter = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;

};

struct Node{
	struct Node *prev;
	struct Node *next;
	struct client Client;
};

struct Node *head;
struct Node *tail;


struct Node *cl;
pthread_t thread[512];

void * Go_online(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	printf("Client Number: %d connected.\n",index + 1);

	while(1){

		char data[512];
		int read = recv(clientSocket,data,512,0);
		data[read] = '\0';

		char output[512];
		if(read>0){
			printf("%s\n", data);
		}
		if(strcmp(data,"SHOW") == 0){
			printf("in show\n");
			int l = 0;
			struct Node* curr = cl;
			int i = 0;
			while(i<clientCount){
				printf("%d\n",index);
				if( index != curr->Client.index){
					l += snprintf(output + l,512,"Client Number: %d is at socket %d.\n",curr->Client.index+1,curr->Client.sockID);
				}
				i++;
				curr = curr->next;
			}

			send(clientSocket,output,512,0);
			continue;

		}
		if(strcmp(data,"SEND") == 0){

			read = recv(clientSocket,data,512,0);
			data[read] = '\0';
			struct Node* curr = cl;
			if(strcmp(data,"ALL")==0){
				read = recv(clientSocket,data,512,0);
				printf("Sending message to ALL clients\n");
				data[read] = '\0';
				int i = 0;
				while(i<clientCount){
					if(curr->Client.index != index){
						char t[512];
						char n[512] = "Message recieved from :";
						sprintf(t,"%d", clientDetail->index);
						strcat(n,t);
						send(curr->Client.sockID,n,512,0);
						send(curr->Client.sockID,data,512,0);

					}
					curr = curr->next;
					i++;
				}

			}
			else{
				
				int id = atoi(data) - 1;
				int flag = 0 ;
				read = recv(clientSocket,data,512,0);
				data[read] = '\0';
				int i = 0;
				while(i<clientCount){
					if(curr->Client.index == id){
						flag = 1;
						char t[512];
						char n[512] = "Message recieved from :";
						sprintf(t,"%d", clientDetail->index);
						strcat(n,t);
						send(curr->Client.sockID,n,512,0);
						send(curr->Client.sockID,data,512,0);
					}
					curr = curr->next;
					i++;
				}
				if(flag == 0){
					printf("Invalid client number \n");
					char err[512] = "Invalid client number";
					send(clientDetail->sockID,err,512,0);
				}
				
					
			}		

		}
		if(strcmp(data,"EXIT") == 0){
			//printf("in exit\n");
			struct Node* one = cl;
			struct Node* two = cl;
			for(int i = 0;i<clientCount;i++){
				if(one->Client.index == index){
					//printf("%d\n", i);
					if(i == 0){
						//printf("going\n");
						cl = cl->next;
					}
					else{
						int j = 0;
						while(j<i){
							two = two->next;
							j++;
						}
						two->next = one->next;
					}
					break;
				}
				if(one->next != NULL){
					one=one->next;
				}

			}
			printf("Removed a client from the database\n");
			clientCount--;
			break;	
		}

	}

	return NULL;

}

int main(){

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(2010);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;

	if(listen(serverSocket,512) == -1) return 0;

	printf("Server started, Currently listenting on port 2010 -x-x-x-x-x-x-x\n");
	printf("Instructions: \n->Use 'SHOW' to see all Available clients\n");
	printf("->Use 'SEND' then client number to send message to the respective client\n");
	printf("->Use 'SEND ALL' then message to send message to all the available clients\n");
	printf("->Please close the client by typing 'EXIT' on the client's terminal\n");

	struct Node *temp = NULL;
	cl = (struct Node*)malloc(sizeof(struct Node));
	temp = cl;

	while(1){
		struct client temp_client;

		temp_client.sockID = accept(serverSocket, (struct sockaddr*) &temp_client.clientAddr, &temp_client.len);
		temp_client.index = start_counter;

		pthread_create(&thread[start_counter], NULL, Go_online, (void *) &temp_client);
		temp->Client = temp_client;
		temp->next = (struct Node*)malloc(sizeof(struct Node));

		temp = temp->next;
		clientCount ++;
		start_counter++;
 
	}

}