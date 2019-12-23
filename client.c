#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define KILOBYTE 1000

void errorOut(char *msg);
int getPortNumber(char *argv[]);
void validateInput(int, char **);
void createServerAddress(struct sockaddr_in *, struct hostent *, int);
void sendConstantMessages(int sockfd, int seconds);

int main(int argc, char *argv[])
{
    int sockfd;
    int numSecondsOfMessages;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    validateInput(argc, argv);
    
    int DefaultProtocol = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, DefaultProtocol);
    if (sockfd < 0) 
        errorOut("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    createServerAddress(&serverAddress, server, htons(getPortNumber(argv)));
    if (connect(sockfd,(struct sockaddr *)&serverAddress,
			    sizeof(serverAddress)) < 0) 
        errorOut("ERROR connecting");

    numSecondsOfMessages = atoi(argv[3]);
     sendConstantMessages(sockfd, numSecondsOfMessages); 
     return 0;

}

void errorOut(char *msg)
{
    perror(msg);
    exit(0);
}

int getPortNumber(char *argv[]) {
	return atoi(argv[2]);
}

void validateInput(int argc, char *argv[]) {
	const int RequiredNumberOfArguments = 4;
	if (argc != RequiredNumberOfArguments) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
}


void createServerAddress(struct sockaddr_in *serverAddress,
			struct hostent *server,
			int serverPort) {
	bzero((char *) serverAddress, sizeof(&serverAddress));
	bcopy((char *)server->h_addr, (char *)&serverAddress->sin_addr.s_addr,
         server->h_length);
        serverAddress->sin_port = serverPort;
	serverAddress->sin_family = AF_INET;
}

double calculateTimeElapsed(clock_t start, clock_t end) {
	return (end - start) /(double) CLOCKS_PER_SEC;
}

void sendConstantMessages(int sockfd, int seconds) { 
	clock_t start, end;
	char buffer[KILOBYTE];
	char *finMessage = "FIN";
	bzero(buffer, KILOBYTE);
	start = clock();
	do {
		if (write(sockfd,buffer, KILOBYTE)< 0) 
			errorOut("ERROR writing to socket");
	} while (calculateTimeElapsed(start, clock()) < (double)seconds);
	write(sockfd, finMessage, strlen(finMessage));
	close(sockfd);
}
