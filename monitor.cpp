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
#include<string>
#include <cassert>
#include<iostream>
#include "unioninfo.h"

#include <cstdlib>
using namespace std;

std::string link_id_str, size_str, power_str;
std::string Tt_str, Tp_str, delay_str;
char message_received[MAXSIZE];

// transfer char[] link info into single info
void split_str(){
	int counter = 0;
	link_id_str = "";
	size_str = "";
	power_str = "";
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		link_id_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		size_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		power_str += message_received[i];
		counter++;
	}
}

// transfer char[] delay info into single info
void split_delay_info(){

	int counter = 0;
	Tt_str = "";
	Tp_str = "";
	delay_str = "";
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		Tt_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		Tp_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		delay_str += message_received[i];
		counter++;
	}
}

int main(void) {
printf("The monitor is up and running\n");

while(true){

  int client_socket, n;
  struct sockaddr_in aws_addr;
 
 
//Create socket
client_socket = socket(AF_INET , SOCK_STREAM , 0);  
if (client_socket == -1)
{
    printf("Could not create socket\n");
}


aws_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
aws_addr.sin_family = AF_INET;
aws_addr.sin_port = htons( AWS_MONITOR_PORT );

//Connect to remote server
if (connect(client_socket, (struct sockaddr *)&aws_addr , sizeof(aws_addr)) < 0)
{
  perror("connect failed. Error\n");
  return 1;
}



// receive link message from the aws
n = recv(client_socket , message_received , MAXSIZE , 0);
message_received[n] = '\0';
split_str();// 
cout << "The monitor received link ID=<"<<link_id_str <<">, size=<" <<size_str<<">,and power=<"<<power_str<<
	"> from the AWS"<< endl; 
 
// receive calculate from the aws
n = recv(client_socket , message_received , MAXSIZE , 0);
message_received[n] = '\0';
if(message_received[0] == ' '){
	cout << "Found no matches for link<"<<link_id_str<< ">"<< endl;
}else{
	split_delay_info();
	cout << "The result for link<" << link_id_str<< ">"<< endl;
	cout << "Tt = <" << Tt_str <<">ms"<< endl;
	cout << "Tp = <" << Tp_str <<">ms" << endl;
	cout << "delay = <" << delay_str <<">ms"<< endl;
}

}
//close(client_socket);
//return 0;
}
