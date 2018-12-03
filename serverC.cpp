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
#include<sstream>
#include<cmath>
#include<math.h>
#include<stdlib.h>

#include "unioninfo.h"

using namespace std;
char message_received[MAXSIZE];
char message_send[MAXSIZE];
std::string bandwidth_str, length_str, light_speed_str, noise_power_str, link_id_str, size_str, power_str;
double bandwidth, length, light_speed, noise_power, link_id, size, power, noise_power_w, power_w, Tt, Tp, delay, capacity;
char Tt_char[SINGLESIZE], Tp_char[SINGLESIZE], delay_char[SINGLESIZE];

// transfer the char[] into the single string
void split_str_p(){
	int counter = 1;
	bandwidth_str = "";
	length_str = "";
	light_speed_str = "";
	noise_power_str = "";
	link_id_str = "";
	size_str = "";
	power_str = "";
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ','){
			break;
		}
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ','){
			break;
		}
		bandwidth_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ','){
			break;
		}
		length_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ','){
			break;
		}
		light_speed_str += message_received[i];
		counter++;
	}
	counter++;
	for (int i = counter; i < MAXSIZE; i++){
		if(message_received[i] == ' '){
			break;
		}
		noise_power_str += message_received[i];
		counter++;
	}
	counter++;
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
	// transfer the string to the doule type
	link_id = atof(const_cast<const char *>(link_id_str.c_str()));
	bandwidth = atof(const_cast<const char *>(bandwidth_str.c_str()));
	length = atof(const_cast<const char *>(length_str.c_str()));
	light_speed = atof(const_cast<const char *>(light_speed_str.c_str()));
	noise_power = atof(const_cast<const char *>(noise_power_str.c_str()));
	size = atof(const_cast<const char *>(size_str.c_str()));
	power = atof(const_cast<const char *>(power_str.c_str()));
	
	//cout << "shuchu"<< bandwidth << endl;
	//cout << "shuchu"<< link_id << bandwidth<<length<<light_speed<<noise_power<< size<< power << endl;



}

// transfer the char[] into the single string
void split_str(){
	int counter = 1;
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


// calculate process
void calculate(){

	power_w = pow(10.0 , ( power / 10.0)) / 1000.0;
	noise_power_w = pow(10.0 , ( noise_power / 10.0)) / 1000.0;
	//cout << power_w << " " << noise_power_w <<  endl;
	capacity = bandwidth * 1000000 * log( 1.0 + power_w / noise_power_w) / log(2.0);
	Tt = size / capacity * 1000.0;
	Tp = length / light_speed / 10.0;
	delay = Tt + Tp;
}

// tranfer all the single delay info into char[]
void transfer(){
	// transfer the double type into the string
	snprintf(Tt_char, SINGLESIZE,"%.2f",Tt);
	snprintf(Tp_char, SINGLESIZE,"%.2f",Tp);
	snprintf(delay_char, SINGLESIZE,"%.2f",delay);
	//cout << "Tt " << Tt_char << "Tp " << Tp_char << "delay " << delay_char << endl;
	strncpy(message_send, Tt_char, 255);
	int len = strlen(Tt_char);
	message_send[len] = ' ';
	for(int i = 0; i < strlen(Tp_char); i++){
		message_send[len+1+i] = Tp_char[i];
	}
	len = strlen(Tt_char)+strlen(Tp_char)+1;
	message_send[len] = ' ';
	for(int i = 0; i < strlen(delay_char); i++){
		message_send[len+1+i] = delay_char[i];
	}
	len = strlen(Tt_char)+strlen(Tp_char)+strlen(delay_char)+2;
	message_send[len] = ' ';
	message_send[len+1] = '\0';

	//printf("%s",message_send);
	//printf("end\n");


}





int main(void){
	int UDP_socket;
	struct sockaddr_in server_addr, client_addr;

// create the socket
if((UDP_socket = socket(AF_INET, SOCK_DGRAM, 0) )<0){
	perror("socket creation failed");
	exit(EXIT_FAILURE);
}

printf("The Server C is up and running using UDP on PORT<%i>\n", SERVERC_PORT);


memset(&server_addr, 0, sizeof(server_addr));
memset(&client_addr, 0, sizeof(client_addr));

// input the address info
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
server_addr.sin_port = htons(SERVERC_PORT);

// bind the socket info
if( bind(UDP_socket, (const struct sockaddr *)& server_addr, sizeof(server_addr)) < 0){
	perror("bind failed");
	exit(EXIT_FAILURE);
}

int n;
socklen_t len;
while(true){

n = recvfrom(UDP_socket, (char *)message_received, MAXSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
message_received[n] = '\0';

if(message_received[0] == ' '){
	message_send[0] = ' ';
	message_send[1] = '\0';
	split_str();
	cout << "The Server C received no match link information of link<"<< link_id_str<<">, file size <"<< size_str;
	cout <<">, and signal power <"<< power_str<<">" << endl;
	// sent back the delay info to the aws
	sendto(UDP_socket, message_send, MAXSIZE, MSG_CONFIRM, (struct sockaddr *) &client_addr, len);
}else{
	split_str_p();
	cout << "The Server C received link information of link<"<< link_id_str<<">, file size <"<< size_str;
	cout <<">, and signal power <"<< power_str<<">" << endl;
	calculate();
	cout << "The server C finished the calculation for link <" << link_id_str<<">"<<endl;
	transfer();
	int length_counter = 0;
	while(length_counter < MAXSIZE){
		if(message_send[length_counter] == '\0'){
			break;
		}
		length_counter++;
	}
	// sent back the delay info to the aws
	sendto(UDP_socket, message_send, MAXSIZE, MSG_CONFIRM, (struct sockaddr *) &client_addr, len);
}


printf("The Server C finished sending the output to AWS\n");	


}
close(UDP_socket);
//return 0;
}