/*
 * PingClient.c
 * UDP Pinger Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

void error(char *msg) {
    perror(msg);
    exit(0);
}

double time_diff(struct timeval x , struct timeval y) {
    double x_ms , y_ms , diff;

    x_ms = (double)(x.tv_sec) * 1000 + (double)(x.tv_usec) / 1000;
    y_ms = (double)(y.tv_sec) * 1000 + (double)(y.tv_usec) / 1000;

    diff = (double)y_ms - (double)x_ms;

    return diff;
}

double find_min(double a[], int size) {
    double min = 5000;

    for (int i = 0; i < size; i++) {
        if (a[i] <= min && a[i] > 0) {
            min = a[i];
        }
    }
    return min;
}

double find_avg(double a[], int size) {
    double sum = 0;

    for (int i = 0; i < size; i++) {
        sum += a[i];
    }
    return sum/size;
}

double find_max(double a[], int size) {
    double max = a[0];

    for (int i = 0; i < size; i++) {
        if (a[i] >= max) {
            max = a[i];
        }
    }
    return max;
}


int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    int count = 0;

    // sent time and received time
    struct timeval time_sent;
    struct timeval time_received;

    // timeout
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // check command line arguments
    if (argc != 3) {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    // socket: create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // gethostbyname: get the server's DNS entry
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    // build the server's Internet address
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    // timeout
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout));

    double rtt_array[10] = {0};

    for(int i = 0; i < 10; i++) {
        char msg[50];
        sprintf(msg, "PING %d\n", i); // puts string into buffer

        gettimeofday(&time_sent, NULL);

        // send the message to the server
        serverlen = sizeof(serveraddr);
        n = sendto(sockfd, msg, strlen(msg), 0, &serveraddr, serverlen);

        if (n < 0) {
            strcpy(msg, "REQUEST TIMEDOUT");
            printf("%s\n", msg);
            sleep(1);
            continue;
        }
        while (1) {
            // print the server's reply
            n = recvfrom(sockfd, msg, strlen(msg), 0, NULL, 0);
            if (n > 0) {
                int received_seq_num = atoi(&msg[5]);
                if(received_seq_num != i)
                    continue;
                gettimeofday(&time_received, NULL);
                double rtt = time_diff(time_sent, time_received);
                printf("PING received from %s: seq#=%d time=%.0lf ms\n", argv[1], received_seq_num, rtt);
                count++;
                rtt_array[i] = rtt;
                break;
            } else if (n < 0) {
                // packet dropped
                printf("REQUEST TIMEDOUT for seq#%d\n", i);
                sleep(1);
                break;
            }
        }
        sleep(1);
    }
    printf("--- ping statistics --- \n%d packets transmitted, %d received, %d%% packet loss rtt min/avg/max = %0.1f/%0.1f/%0.1f\n", 10, count, (10-count)*10, find_min(rtt_array, 10), find_avg(rtt_array, 10),find_max(rtt_array, 10));

    // close socket
    close(sockfd);
    return 0;
}
