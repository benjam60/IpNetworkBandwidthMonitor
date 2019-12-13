#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define KILOBYTE 1000

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
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) error("ERROR on accept");
     bzero(buffer, KILOBYTE);
     int bytesRead = 0;
     clock_t startTime = clock();
     do {
	bytesReceived = read(newsockfd, buffer, KILOBYTE);
	if (bytesReceived < 0) { error("ERROR reading from socket"); }
	else { bytesRead += bytesReceived; }
     } while (bytesReceived > 4); // (0 == hasFinMessage(buffer, n));
     double elapsedTimeInSeconds = (clock() - startTime)/(double)CLOCKS_PER_SEC;
     double kilobytesReceived = bytesRead / (double)1000;
     double Mbps = bytesRead / (double)125000;
     printf("Received=%f KB, Rate=%f Mbps", kilobytesReceived , Mbps);
     return 0; 
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int hasFinMessage(char * msg, int length) {
	int index = 0;
	while (index < length) {
		if (*msg != '0') { return 1;}
		++msg;
	}
	return 0;
}
