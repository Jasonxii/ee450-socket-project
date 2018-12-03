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
#include <cassert>
#include<iostream>
#include<string>
#include <cstdlib>
#include "unioninfo.h"
using namespace std;


std::string link_id_str, size_str, power_str;
char client_message[MAXSIZE];
char link_info[MAXSIZE];
char serverC_send_info[MAXSIZE];
char serverC_received_info[MAXSIZE];
char delay_info[SINGLESIZE];



void search(char ch, char send_message[], int message_len){
	int UDP_socket;
	char buffer[MAXSIZE];
	int backend_server_port;
	struct sockaddr_in server_addr;


if(ch == 'A'){
	backend_server_port = SERVERA_PORT;
}else{
	backend_server_port = SERVERB_PORT;
}

if((UDP_socket = socket(AF_INET, SOCK_DGRAM, 0) )<0){
	perror("socket creation failed");
	exit(EXIT_FAILURE);
}

memset(&server_addr, 0, sizeof(server_addr));

//Prepare the sockaddr_in structure
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
server_addr.sin_port = htons(backend_server_port);


int n;
socklen_t len;
// send the id info to the severA or serverB
sendto(UDP_socket, send_message, message_len, MSG_CONFIRM, (struct sockaddr *) &server_addr, sizeof(server_addr));


cout <<"The AWS sent link ID=<" << send_message<<"> to Backend-Server "<< ch<< " using UDP over port<" << AWS_UDP_PORT<<">"<<endl;


// receive link info from the serverA or serverB
n = recvfrom(UDP_socket, (char *)buffer, MAXSIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
if(buffer[0] == ' '){
	cout <<"The AWS received <0> matches from Backend-Server <" << ch <<"> using UDP over port<"<< AWS_UDP_PORT<<">"<< endl;
}else{
	strncpy(link_info, buffer, MAXSIZE);
	cout <<"The AWS received <1> matches from Backend-Server <" << ch <<"> using UDP over port<"<< AWS_UDP_PORT<<">" << endl;

}

//close(UDP_socket);

}

// split the message from the client into single string
void split_str(){
	link_id_str ="";
	size_str = "";
	power_str = "";
	int counter = 0;
	for (int i = counter; i < MAXSIZE; i++){
		if(client_message[i] == ' '){
			break;
		}
		link_id_str += client_message[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(client_message[i] == ' '){
			break;
		}
		size_str += client_message[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(client_message[i] == ' '){
			break;
		}
		power_str += client_message[i];
		counter++;
	}
}


void serverC_UDP_connection(){
	int UDP_socket;
	char buffer[MAXSIZE];
	struct sockaddr_in server_addr;

	//create the socket
	if((UDP_socket = socket(AF_INET, SOCK_DGRAM, 0) )<0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	//Prepare the sockaddr_in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(SERVERC_PORT);


	int n;
	socklen_t len;

	// send the id info to the serverA or serverB
	sendto(UDP_socket, serverC_send_info, strlen(serverC_send_info), MSG_CONFIRM, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if(serverC_send_info[0] ==' '){
		printf("The AWS sent No match to the monitor and the client using TCP over ports <%i> and <%i>, respectively\n", 
			AWS_CLIENT_PORT, AWS_MONITOR_PORT);
	}else{
		cout << "The AWS sent link ID=<" << link_id_str<<">, size=<"<< size_str<< ">, power=<"<<power_str;
		cout << ">, and link information to Backend-Server C using UDP over port<" << AWS_UDP_PORT<<">"<<endl;
	}

	// receive the link info from the serverA  or serverB
	n = recvfrom(UDP_socket, (char *)buffer, MAXSIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
	if(serverC_send_info[0] !=' '){
		printf("The AWS received outputs from Backend-Server C using UDP over port <%i>\n",AWS_UDP_PORT);
	}

	strncpy(serverC_received_info, buffer, 255);// copy the buffer info

	//close(UDP_socket);

}


// convert the single string into the char[] to be transferred to the serverC
void process_serverC_info(){
	
	strncpy(serverC_send_info, link_info, MAXSIZE);
	int loc = strlen(serverC_send_info);
	for(int i = loc; i < MAXSIZE; i++){
		if(client_message[i-loc] == '\0'){
			serverC_send_info[i] = '\0';
			break;
		}
		serverC_send_info[i] = client_message[i-loc];
	}
	
}

// split the delay info with char[] into single
void split_delay(){
	int counter = 0;
	while(counter < MAXSIZE){
		if(serverC_received_info[counter] == ' '){
			break;
		}
		counter++;
	}
	counter++;
	while(counter < MAXSIZE){
		if(serverC_received_info[counter] == ' '){
			break;
		}
		counter++;
	}
	counter++;
	for(int i = 0; i < MAXSIZE; i++){
		if(serverC_received_info[counter] == ' '){
			delay_info[i] = '\0';
			break;
		}
		delay_info[i] = serverC_received_info[counter];
		counter++;  
	}


}


int main(){

	int TCP_socket , client_socket , base_sockaddr_size ,n;
	struct sockaddr_in server_addr , client_addr;

	int TCP_monitor_socket, client_monitor_socket, n_monitor;
	struct sockaddr_in server_monitor_addr, client_monitor_addr;

	//Create socket 
	TCP_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (TCP_socket == -1)
	{
		printf("Could not create socket");
	}

	//create another socket with the monitor client
	TCP_monitor_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (TCP_monitor_socket == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons( AWS_CLIENT_PORT );

	//Prepare the sockaddr_in structure
	server_monitor_addr.sin_family = AF_INET;
	server_monitor_addr.sin_addr.s_addr = INADDR_ANY;
	server_monitor_addr.sin_port = htons( AWS_MONITOR_PORT );
	
	//Bind with the client
	if( bind(TCP_socket,(struct sockaddr *)&server_addr , sizeof(server_addr)) < 0)
	{
		//print the error message
		perror("bind client failed. Error");
		exit(EXIT_FAILURE);
	}

	// bind with the monitor client 
	if( bind(TCP_monitor_socket,(struct sockaddr *)&server_monitor_addr , sizeof(server_monitor_addr)) < 0)
	{
		//print the error message
		perror("bind monitor failed. Error");
		exit(EXIT_FAILURE);
	}
	
	
	//Listen
	listen(TCP_socket , 3);
	listen(TCP_monitor_socket , 3);
	
	//Accept and incoming connection
	puts("The AWS is up and running");
	base_sockaddr_size = sizeof(struct sockaddr_in);
	


	while(true){

	//accept connection from an incoming client
	client_socket = accept(TCP_socket, (struct sockaddr *)&client_addr, (socklen_t*)& base_sockaddr_size);
	if (client_socket < 0)
	{
		perror("accept failed");
		exit(EXIT_FAILURE);
	}



	client_monitor_socket = accept(TCP_monitor_socket, (struct sockaddr *)&client_monitor_addr, (socklen_t*)& base_sockaddr_size);
	if (client_monitor_socket < 0)
	{
		perror("accept failed");
		exit(EXIT_FAILURE);
	}
	


	//Receive a message from client
	n = recv(client_socket , client_message , MAXSIZE , 0);
	client_message[n] = '\0';

	split_str();


	cout << "The AWS received link ID=<"<<link_id_str <<">, size=<" <<size_str<<">,and power=<"<<power_str<<
	"> from the client using TCP over port<"<< AWS_CLIENT_PORT <<">"<< endl; 

	//send link_info into monitor
	send(client_monitor_socket , client_message , strlen(client_message), 0);
	
	cout << "The AWS sent link ID=<"<<link_id_str <<">, size=<" <<size_str<<">,and power=<"<<power_str<<
	"> to the monitor using TCP over port<"<< AWS_MONITOR_PORT <<">"<< endl; 

	char id[link_id_str.length()];
	
	for( int i = 0; i < link_id_str.length(); i++){
     	id[i] = link_id_str[i];
	}
	id[link_id_str.length()] = '\0';


	search('A', id, link_id_str.length());// convertig info with serverA
	
	search('B', id, link_id_str.length());// convertig info with serverB

	if( link_info[0] == '\0'){
		
		link_info[0] = ' ';
		link_info[1] = '\0';
		
	}

	process_serverC_info();// prepare the info with char[] for serverC

	serverC_UDP_connection(); // convertig info with serverC

	split_delay();// split the delay info with char[] to single string

	// send the Delay info to the monitor
	send(client_monitor_socket , serverC_received_info , strlen(serverC_received_info), 0);

	// send the Delay info to the client
	send(client_socket , serverC_received_info , strlen(serverC_received_info), 0);

	if(serverC_received_info[0] != ' '){
	cout << "The AWS sent delay=<" << delay_info << ">ms to the client using TCP over port<"<< AWS_CLIENT_PORT<<">"<< endl;
	cout << "The AWS sent detailed results to the monitor using TCP over port<"<< AWS_MONITOR_PORT<<">"<< endl;
	}


}

	close(TCP_socket);
	close(client_socket);
	close(TCP_monitor_socket);
	close(client_monitor_socket);

	
}
