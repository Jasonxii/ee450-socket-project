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
std::string Tt_str, Tp_str, delay_str;
char message_received[MAXSIZE];


void split_delay_info(){
  int counter = 0;
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


int main(int argc, char * argv[]) {
  int link_id, size, power, n;
  char link_id_str[SINGLESIZE], size_str[SINGLESIZE], power_str[SINGLESIZE];
  int client_socket;
  struct sockaddr_in aws_addr;
  char message[MAXSIZE];
  std::string message_copy;
  char *server_reply;

// read the inputs
  if(argc == 4){
      link_id = atoi(argv[1]);
      size = atoi(argv[2]);
      power = atoi(argv[3]);
  }else{
      printf("Your command is not as required, Please re-command.(eg. './client 3 10000 -30')\n");
      return 1;
  } 
 
//Create socket
client_socket = socket(AF_INET , SOCK_STREAM , 0);  
if (client_socket == -1)
{
    printf("Could not create socket\n");
}
printf("The client is up and running\n");
printf("The client sent link ID=<%i>, size=<%i>,and power=<%i> to AWS\n", link_id, size, power);

//Prepare the sockaddr_in structure 
aws_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
aws_addr.sin_family = AF_INET;
aws_addr.sin_port = htons( AWS_CLIENT_PORT );

//Connect to remote server
if (connect(client_socket, (struct sockaddr *)&aws_addr , sizeof(aws_addr)) < 0)
{
  perror("connect failed. Error\n");
  return 1;
}
 
  //keep communicating with server
  sprintf(link_id_str, "%d", link_id);
  sprintf(size_str, "%d", size);
  sprintf(power_str, "%d", power);
  message_copy = std::string(link_id_str)+" "+std::string(size_str)+" "+std::string(power_str)+" ";
  for(int i = 0; i < message_copy.length(); i++){
    message[i] = message_copy[i];
  }
  

  //Send some data
  if( send(client_socket , message , message_copy.length() , 0) < 0){
    puts("Send failed\n");
    return 1;
  }




  // receive the delay info from the aws
  n = recv(client_socket , message_received , MAXSIZE , 0);
  message_received[n] = '\0';
  if(message_received[0] == ' '){
  cout << "Found no matches for link<"<<link_id_str<< ">"<< endl;
  }else{
    split_delay_info();
  
  cout << "The delay for link <" << link_id_str << "> is <"<< delay_str <<">ms"<< endl;
  }

  
  close(client_socket);
  return 0;
}
