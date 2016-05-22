/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

struct player
{
  string name;
  int guessCount;
  time_t timeStamp;
};

int generateRandomNum()
{
  int secretNum = rand() % 9999 + 0000; //TODO: pad 4 digits
  return secretNum;
}

const void* convertRandom(int randomNum)
{
  string secretNumtoRevealNum = to_string(randomNum);
  const void* revealNum = secretNumtoRevealNum.c_str();
  return revealNum;
}

const void* convertStringtoCstrVoidPtr(string message)
{
  const void* passMsg = message.c_str();
  return passMsg;
}

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  srand(time(NULL));
  string w = "Welcome to Number Guessing Game!\nEnter your name: ";
  const void* welcome;
  const void* revealRand;
  int guessThisRandom;


  if (argc < 2)
  {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd,
      (struct sockaddr *) &serv_addr,
      sizeof(serv_addr)) < 0)
  {
    error("ERROR on binding");
  }

  guessThisRandom = generateRandomNum();
  revealRand = convertRandom(guessThisRandom);


  //printf("Waiting for player to guess: %s\n",to_string(guessThisRandom));
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd,
         (struct sockaddr *) &cli_addr,
         &clilen);
  if (newsockfd < 0)
  {
    error("ERROR on accept");
  }

  welcome = convertStringtoCstrVoidPtr(w);
  n = write(newsockfd,welcome,strlen(w.c_str())); //message to client
  if (n < 0)
  {
    error("ERROR writing to socket");
  }

  bzero(buffer,256);
  n = read(newsockfd,buffer,255);
  if (n < 0)
  {
    error("ERROR reading from socket");
  }

  printf("Here is the message: %s\n",buffer); //message from client

  n = write(newsockfd,revealRand,sizeof(revealRand)); //message to client
  if (n < 0)
  {
    error("ERROR writing to socket");
  }

  close(newsockfd);
  close(sockfd);

  return 0;
}
