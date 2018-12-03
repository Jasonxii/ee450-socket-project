#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<iostream>


#include "unioninfo.h"

using namespace std;
char buffer[MAXSIZE];
char message_send[MAXSIZE];
char id[SINGLESIZE];
char message_received[10];

// check if the ids are the same
bool compare_id(const char a[], const char b[]){

	for(int i = 0; i < SINGLESIZE; i ++){
		if(b[i] == '\0'){
			if(a[i] != '\0') return false;
			break;
		}

		if(a[i] != b[i]) return false;

	}

return true;

}

void read_file(){

	//test
	FILE *fp;
	char id[255];
	fp = fopen("database_a.csv","r");
	message_send[0] = '\0';
	
	while(fgets(buffer, 255, (FILE*) fp)){
		
		int counter = 0;
		int length = strlen(buffer);
		buffer[length-1] =' ';
		buffer[length] ='\0';
		while(counter < 255){
			if(buffer[counter] == ','){
				break;
			}
			id[counter] = buffer[counter];
			counter++;
		}
		id[counter] = '\0';
		if(compare_id(id,message_received) == true){
			
			strncpy(message_send, buffer, 255);
		
			printf("The Server A has found <1> matches\n");
			break;
		}

	}

	if(message_send[0] == '\0'){
		message_send[0] = ' ';
		message_send[1] = '\0';
		printf("The Server A has found <0> matches\n");
	}

	fclose(fp);
}




int main(void){
	int UDP_socket;
	struct sockaddr_in server_addr, client_addr;

// create the socket
if((UDP_socket = socket(AF_INET, SOCK_DGRAM, 0) )<0){
	perror("socket creation failed");
	exit(EXIT_FAILURE);
}

printf("The Server A is up and running using UDP on PORT<%i>\n", SERVERA_PORT);


memset(&server_addr, 0, sizeof(server_addr));
memset(&client_addr, 0, sizeof(client_addr));

// input the address info
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
server_addr.sin_port = htons(SERVERA_PORT);


// bind the UDP connection
if( bind(UDP_socket, (const struct sockaddr *)& server_addr, sizeof(server_addr)) < 0){
	perror("bind failed");
	exit(EXIT_FAILURE);
}


int n;
socklen_t len;
while(true){
// receive the id from the aws
n = recvfrom(UDP_socket, (char *)message_received, MAXSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
message_received[n] = '\0';
///


printf("The Server A received input<%s>\n", message_received);

read_file();


int length_counter = 0;
while(length_counter < MAXSIZE){
	if(message_send[length_counter] == '\0'){
		break;
	}
	length_counter++;
}

// send the char[] link info to the aws
sendto(UDP_socket, message_send, MAXSIZE, MSG_CONFIRM, (struct sockaddr *) &client_addr, len);

printf("The Server A finished sending the output to AWS\n");
}

close(UDP_socket);	
//return 0;
}







