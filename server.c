#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>
#include <string.h>

#define KILOBYTE 1000
#define FIRST_LETTER_OF_FIN_MESSAGE 'F' 

void error(char *msg);
int hasFinMessage(char * msg, int length);

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, bytesReceived;
     char buffer[KILOBYTE];
     struct sockaddr_in serv_addr, cli_addr;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     bzero(buffer, KILOBYTE);
     double bytesRead = 0;
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) error("ERROR on accept");
     clock_t startTime = clock();
     bytesReceived = read(newsockfd, buffer, KILOBYTE);
     while (buffer[0] != FIRST_LETTER_OF_FIN_MESSAGE) {
	if (bytesReceived < 0) { error("ERROR reading from socket"); }
	else { bytesRead += bytesReceived; }
	bytesReceived = read(newsockfd, buffer, KILOBYTE);
	//buffer[0] != FIRST_LETTER_OF_FIN_MESSAGE
     }
     
	char *finAckMessage = "FINACK";
        write(sockfd, finAckMessage, strlen(finAckMessage));
	close(sockfd);
	double elapsedTimeInSeconds = (clock() - startTime)/(double)CLOCKS_PER_SEC;
     double kilobytesReceived = bytesRead / (double)1000;
     double Mbps = bytesRead / (double)125000;

     FILE *f;
     f = fopen("LastPerformanceTest.txt", "w+");
     fprintf(f, "Received=%f KB, Rate=%f Mbps", kilobytesReceived , Mbps);
     fclose(f);
     return 0; 
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

