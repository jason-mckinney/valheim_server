#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <netdb.h>
#include <signal.h>


#define BUFFER_SIZE 1401
#define PACKET_TIMEOUT 5000
#define MAXTRIES 5


long millis() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec * 0.001;
}


int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    
    int sockfd, n, len;
    int timeout = 30;
    int counter = 0;
    int timeout_counter = 0;
    int port = atoi(argv[1]);

    long t;
    
    char buffer [ BUFFER_SIZE ];
    char *ptr;
    const char *payload = "\xff" "\xff" "\xff" "\xff" "TSource Engine Query" "\x00";
    
    if (argc > 2) {
        timeout = atoi(argv[2]);
    }

    printf("playercount_poll: INFO: Starting player count automatic server shutdown checker, timeout is %d minutes...\n\n", timeout);
    printf("playercount_poll: INFO: Port to check is %d\n\n", port);

    memset(&servaddr, 0, sizeof(servaddr));
    
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (argc > 3)
        servaddr.sin_addr.s_addr = htonl(strtol(argv[2], NULL, 16));
    else
        servaddr.sin_addr.s_addr = htonl(0x7f000001);

    while (counter < timeout) {
        // Creating socket file descriptor
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            printf("\nplayercount_poll: ERROR: socket creation failed\n\n");
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        sendto(sockfd, payload, strlen(payload) + 1,
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));

        t = millis();
        while (millis() - t < PACKET_TIMEOUT) {
            n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE,  
                    MSG_DONTWAIT, (struct sockaddr *) &servaddr, 
                    &len);

            if (n >= 5) {
                timeout_counter = -1;

                if(buffer[4] == 'I') {
                    for(ptr = buffer+6; *ptr != '\0'; ptr++);  //Name
                    for(ptr = ptr + 1; *ptr != '\0'; ptr++);   //Map
                    for(ptr = ptr + 1; *ptr != '\0'; ptr++);   //Folder
                    for(ptr = ptr + 1; *ptr != '\0'; ptr++);   //Game
                    
                    if(*(ptr + 3) > 0) counter = -1;
                }

                break;
            }

            usleep(100000);
        }

        close(sockfd);

        if (++counter >= timeout || ++timeout_counter >= MAXTRIES) {
            FILE *fp;
            char pidbuf[5];
            pid_t pid;

            printf("playercount_poll: INFO: %d minutes of inactivity, shutting down...\n\n", timeout);

            /* Open the command for reading. */
            fp = popen("/usr/bin/pidof valheim_server.x86_64", "r");
            if (fp == NULL) {
                exit(1);
            }

            /* Read the output a line at a time - output it. */
            if (fgets(pidbuf, 5, fp) != NULL) {
                pid = atoi(pidbuf);
            }

            /* close */
            pclose(fp);
            return kill(pid, 2);
        }

        t = 60000000 - 1000 * (millis() - t);
        if (t > 0) usleep(t);
    }

    return 0;
}
