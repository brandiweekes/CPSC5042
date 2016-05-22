#include <stdio.h>
#include <sys/types.h> //defs of data types used in sys calls
#include <sys/socket.h> //defs of structs needed for sockets
#include <netinet/in.h> //consts and structs for internet domain addresses

//called when a system call fails
void error(char *msg)
{
	perror(msg); //display message about the error on stderr
	exit(1); //aborts the program
}

int main(int argc, char *argv[])
{
	int sockfd; //store vals returned by socket sys call
	int newsockfd; //store vals returned by accept sys call
	int portno; //store port num on which server accepts connections
	int clilen; //stores size of address of client, need for accept sys call
	int numCharReadWrite; //return val for read(), write()

	char buffer[256]; //server reads char from socket connection to this buffer
	struct sockaddr_in serv_addr; //contains addy of server
	struct sockaddr_in cli_addr; //contains addy of client connecting to server

	//struct sockaddr_in /*TODO?*/

	//check if user entered port number
	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided");
		exit(1);
	}

	//creates a new socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//check that the protocol was assigned by OS
	if (sockfd < 0)
	{
		error("ERROR opening socket");
	}

	//all vals in buffer set to 0
	//param: 1 is pointer to buffer, 2 is size of buffer
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//port number on which the server will listen for connections
	portno = atoi(argv[1]); //converts string of digits to int

	//set fields of serv_addr
	serv_addr.sin_family = AF_INET; //addy family
	serv.sin_port = htons(portno); //converts 16bit int to network order
	serv_addr.sin_addr.s_addr = INADDR_ANY; //IP addy of host
	
	//bind socket to an address
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR on binding");
	}

	//allow process to listen on socket for connections
	//param sockfd: socket file descriptor
	//param 5: size of backlog queue for num of connections waiting 
	//while process is handling particular connection (5 is standard max)
	listen(sockfd, 5);

	clilen = sizeof(cli_addr);

	//causes process to block unitl a client connects to server
	//returns new file descriptor, all communication on this connections should
	//by done using the new file descriptor
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if(newsockfd < 0)
	{
		error("ERROR on accept");
	}

	bzero(buffer, 256);
	numCharReadWrite = read(newsockfd, buffer, 255);
	if(numCharReadWrite < 0)
	{
		error("ERROR reading from socket");
	}
	else
	{
		//read message from client
		printf("Here is the message: %s", buffer);
	}

	//write message to client 
	//param 18: size of mesage
	numCharReadWrite = write(newsockfd, "I got your message", 18);
	if (numCharReadWrite < 0)
	{
		error("ERROR writing to socket");
	}

	return 0; //terminate main program

}