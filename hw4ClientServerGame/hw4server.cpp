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
#include <cmath>
using namespace std;

int closeness(int guess, int random);
int thousandth(int guess, int random);
int hundredth(int guess, int random);
int tens(int guess, int random);
int ones(int guess, int random);


struct player
{
  string playerName;
  int guessCount = 1;
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
  struct sockaddr_in serv_addr, cli_addr;

  int n;
  char buffer[256];

  srand(time(NULL));
  // const void* revealRand;
  // string secretRand;
  int guessThisRandom;
  int closenessValue;

  // string t = "Turn: 2 \nEnter a guess:    ";
  // const void* turnMsg = convertStringtoCstrVoidPtr(t);
  // //send game turn message to client
  // n = write(newsockfd,turnMsg,strlen(t.c_str())); //message to client
  // if (n < 0)
  // {
  //   error("ERROR writing to socket");
  // }

  //vars for send(),recv()
  int bytesLeft, bytesRecvd;

  //vars to receive user name from client
  char rcvdName[50];
  char* rcvdNamePtr;

  //vars to receive user guess from client
  long rcvdGuess;
  char *rcvdGuessPtr;
  long playerGuess;

  player* p = new player();

  //check if user entered port number
  if (argc < 2)
  {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  //create a new socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //check that protocol was assigned by OS
  if (sockfd < 0)
  {
    error("ERROR opening socket");
  }

  //all vals in buffer set to 0
  //param1: pointer to buffer,
  //param2: size of buffer
  bzero((char *) &serv_addr, sizeof(serv_addr));

  //port number on which the server will listen for connection
  portno = atoi(argv[1]); //convert string of digits to int

  //set fields of serv_addr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY; //IP addy of host
  serv_addr.sin_port = htons(portno); //convert 16bit int to network order

  //bind socket to an address
  if (bind(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
  {
    error("ERROR on binding");
  }

  //allow process to listen on socket for connections
	//param sockfd: socket file descriptor
	//param 5: size of backlog queue for num of connections waiting
	//while process is handling particular connection (5 is standard max)
  listen(sockfd,5);

  clilen = sizeof(cli_addr);

  //causes process to block until a client connects to server
	//returns new file descriptor, all communication on this connections should
	//be done using the new file descriptor
  newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
  if (newsockfd < 0)
  {
    error("ERROR on accept");
  }

  /*****************************************************************/
  /*FOLLOWING PROGRAM WILL RUN ONCE ABOVE SERVER CONNECTION SUCCESS*/
  /*****************************************************************/

  //receive name from client
  bytesLeft = 50;
  rcvdNamePtr = rcvdName;
  while(bytesLeft)
  {
    bytesRecvd = recv(newsockfd, rcvdNamePtr, bytesLeft, 0);
    if(bytesRecvd <= 0)
    {
      exit(-1);
    }
    bytesLeft -= bytesRecvd;
    rcvdNamePtr = rcvdNamePtr + bytesRecvd;
  }
  printf("DEBUG client name: %s\n", rcvdName);
  //set struct player playerName & timeStamp
  string namePlayer(rcvdName);
  p->playerName = namePlayer;
  p->timeStamp = time(0);
  printf("DEBUG the name is set, so dance: %s\n",p->playerName.c_str());

  guessThisRandom = generateRandomNum();
  // revealRand = convertRandom(guessThisRandom);
  // secretRand = to_string(guessThisRandom);
  printf("The random secret number = %d\n",guessThisRandom);

  //receive guessNum from client
  bytesLeft = sizeof(long);
  rcvdGuessPtr = (char *) &rcvdGuess;
  while (bytesLeft)
  {
    bytesRecvd = recv(newsockfd,rcvdGuessPtr,bytesLeft,0);
    if (bytesRecvd <= 0)
    {
      exit(-1);
    }
    bytesLeft -= bytesRecvd;
    rcvdGuessPtr = rcvdGuessPtr + bytesRecvd;
  }
  playerGuess = ntohl(rcvdGuess);
  printf("DEBUG %s guessed %d\n",p->playerName.c_str(), (int)playerGuess);
  cout << playerGuess << endl;

  if((int)playerGuess != guessThisRandom)
  {
    closenessValue = closeness((int)playerGuess, guessThisRandom);
    printf("DEBUG %s closeness = %d\n",p->playerName.c_str(), closenessValue);
  }

  /*EXPERIMENTAL CODE BELOW, PLUS END OF PROGRAM*/
  bzero(buffer,256);
  n = read(newsockfd,buffer,255);
  if (n < 0)
  {
    error("ERROR reading from socket");
  }

  //read mesage from client
  printf("Here is the message: %s\n",buffer); //message from client

  // printf("this is n: %d", n);
  // printf("client name: %s\n", buffer);
  // string foo(buffer);
  // p->playerName = foo;
  //
  // printf("print foo: %s\n",foo.c_str());
  // printf("the name is set, so dance: %s\n",p->playerName.c_str());

  //write message to client
  //param 2: message to client
  //param 3: size of msg
  // n = write(newsockfd,revealRand,sizeof(revealRand)); //message to client
  // if (n < 0)
  // {
  //   error("ERROR writing to socket");
  // }

  close(newsockfd);
  close(sockfd);

  return 0;
}

int closeness(int guess, int random)
{
  int th = thousandth(guess, random);
  int h = hundredth(guess, random);
  int t = tens(guess, random);
  int o = ones(guess, random);

  return th + h + t + o;
}

int thousandth(int guess, int random)
{
  int guessThou = (guess % 10000) / 1000;
  int randomThou = (random % 10000) / 1000;

  return abs(guessThou-randomThou);
}
int hundredth(int guess, int random)
{
  int guessHund = (guess % 1000) / 100;
  int randomHund = (random % 1000) / 100;

  return abs(guessHund-randomHund);
}
int tens(int guess, int random)
{
  int guessTens = (guess % 100) / 10;
  int randomTens = (random % 100) / 10;

  return abs(guessTens-randomTens);
}
int ones(int guess, int random)
{
  int guessOnes = (guess % 10);
  int randomOnes = (random % 10);

  return abs(guessOnes-randomOnes);
}
