/* A simple client in the internet domain using TCP
The server name & port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string>
using namespace std;

//called when a system call fails
void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];

  //vars for send(),recv()
  int bytesLeft, bytesRecvd, bytesSent;

  int turnNum = 0;
  long guessNum;
  long clientGuess;

  //vars to receive closeness from server
  long rcvdCloseness;
  char*  rcvdClosenessPtr;
  long closenessValue = -1;

  //vars to receive user name from client
  char rcvdVictory[56];
  char* rcvdVictoryPtr;

  string enterName;
  char sendName[50];

  //check if user entered port number
  if (argc < 3)
  {
     fprintf(stderr,"usage %s hostname port\n", argv[0]);
     exit(0);
  }

  //port number on which the server will listen for connections
  portno = atoi(argv[2]);

  //creates new socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //check that the protocol was assigned by OS
  if (sockfd < 0)
  {
    error("ERROR opening socket");
  }

  //contains name of host on internet
  server = gethostbyname(argv[1]);
  if (server == NULL)
  {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }

  //all vals in buffer set to 0
  //param 1: pointer to buffer
  //param 2: size of buffer
  bzero((char *) &serv_addr, sizeof(serv_addr));

  //set fields of serv_addr
  serv_addr.sin_family = AF_INET;
  //copy length bytes from s1 to s2
  bcopy((char *)server->h_addr,
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
  {
    error("ERROR connecting");
  }

  /*FOLLOWING PROGRAM WILL RUN ONCE ABOVE SERVER CONNECTION SUCCESS*/

  printf("Welcome to Number Guessing Game!\n");
  printf("Enter your name: ");
  cin >> enterName;
  if (enterName.length() >= 50)
  {
    exit(-1); // too long
  }
  strcpy(sendName, enterName.c_str());
  bytesSent = send(sockfd, (void *) sendName, 50, 0);
  if (bytesSent != 50)
  {
    exit(-1);
  }

  while(closenessValue != 0)
  {
    turnNum++;
    //user enters a guess between 0 & 9999, inclusive
    printf("Turn: %d\n", turnNum);
    printf("Enter a guess: ");
    cin >> guessNum;
    while(guessNum < 0 || guessNum > 9999)
    {
      printf("Guess must be between 0 and 9999, inclusive\n");
      printf("Enter a guess: ");
      cin >> guessNum;
    }
    //send number to server
    clientGuess = htonl(guessNum);
    bytesSent = send(sockfd, (void *) &clientGuess, sizeof(long), 0);
    if (bytesSent != sizeof(long))
    {
     exit(-1);
    }
    printf("Result of guess: ");

    //receive closenessValue from server
    bytesLeft = sizeof(long);
    rcvdClosenessPtr = (char *) &rcvdCloseness;
    while (bytesLeft)
    {
      bytesRecvd = recv(sockfd,rcvdClosenessPtr,bytesLeft,0);
      if (bytesRecvd <= 0)
      {
        printf("bytesRecvd <= 0\n");
        exit(-1);
      }
      bytesLeft -= bytesRecvd;
      rcvdClosenessPtr = rcvdClosenessPtr + bytesRecvd;
    }
    closenessValue = ntohl(rcvdCloseness);
    printf("%d\n",(int)closenessValue);
    //cout << closenessValue << endl;
    printf("\n");

  }
  //cout << "exited game loop" << endl;

  //receive victory message from server

  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0)
       error("ERROR reading from socket");
  printf("buffer %s\n",buffer);

  bzero(rcvdVictory,55);
  bytesLeft = 55;
  rcvdVictoryPtr = rcvdVictory;
  while(bytesLeft)
  {
    bytesRecvd = recv(sockfd, rcvdVictoryPtr, bytesLeft, 0);
    if(bytesRecvd <= 0)
    {
      error("ERROR receiving victory message from socket");
    }
    bytesLeft -= bytesRecvd;
    rcvdVictoryPtr = rcvdVictoryPtr + bytesRecvd;
  }
  printf("recv %s\n", rcvdVictory);

  //read from socket
  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0)
       error("ERROR reading from socket");
  //read message from socket
  printf("last %s\n",buffer);

  close(sockfd);
  return 0;
}
