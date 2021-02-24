#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>


#define BUFFER_SIZE 1401
#define TIMEOUT 5000
#define MINS 30

long millis() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec * 0.001;
}

int main(int argc, char *argv[]) { 
    int sockfd; 
    char buffer [ BUFFER_SIZE ]; 
    struct sockaddr_in servaddr;
    int port = atoi(argv[1]);
    const char *payload = "\xff" "\xff" "\xff" "\xff" "TSource Engine Query" "\x00";
    int counter = 0;

    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port);
    
    if (argc > 2)
        servaddr.sin_addr.s_addr = htonl(strtol(argv[3], NULL, 16));
    else
        serveraddr.sin_addr.s_addr = htonl(0x7f000001);
      
    int n, len;
    long t;
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 100000000;

    while (counter < MINS) {
        // Creating socket file descriptor 
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 

        sendto(sockfd, payload, strlen(payload) + 1,
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));

        t = millis();
        while (millis() - t < TIMEOUT) {
            n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE,  
                    MSG_DONTWAIT, (struct sockaddr *) &servaddr, 
                    &len);

            if (n < 0) {
                nanosleep(&req, NULL);
                continue;
            }

            buffer[n] = '\0';
            printf("Response : %s\n", buffer);
        }

        close(sockfd);

        if (counter >= MINS)
            break;

        sleep(1);
    }
  
    return 0; 
}