/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
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

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    int count = 0;
    int min = 200;
    int max = 0;
    int x[10];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
    (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    // need a 1 second time delay to wait for response
    struct timeval time_sent;
    struct timeval time_received;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));

    for(int i = 0; i < 10; i++){
      char msg[BUFSIZE];
      bzero(buf, BUFSIZE);
      strcpy(msg, "PING");

      gettimeofday(&time_sent, NULL);

      /* send the message to the server */
      serverlen = sizeof(serveraddr);
      n = sendto(sockfd, msg, strlen(msg), 0, &serveraddr, serverlen);
      if (n < 0){
        strcpy(msg, "REQUEST TIMEDOUT FROM SEND");
        printf ("%s\n", msg);
        sleep(1);
        continue;
      }

      /* print the server's reply */
      n = recvfrom(sockfd, msg, strlen(msg), 0, NULL, 0);
      if (n < 0){
        strcpy(msg, "REQUEST TIMEDOUT FROM RECEIVE");
        printf ("%s\n", msg);
        sleep(1);
        continue;
      }

      gettimeofday(&time_received, NULL);
      time_diff(time_sent, time_received);
      printf("PING received from %s: seq#=%d time=%.0lf ms\n" ,argv[1], i, time_diff(time_sent, time_received) );
      count++;
      sleep(1);
      if (time_diff(time_sent, time_received) < min){
       min = time_diff(time_sent, time_received);
      }
      if (time_diff(time_sent, time_received) > max){
       max = time_diff(time_sent, time_received);
      }
      x[i] += time_diff(time_sent, time_received);
    }

    int sum = 0;
    int avg = 0;
    for (int j = 0; j < 10; j++ ) {
      sum += x[j];
    }
    avg = sum/10;

    printf("--- ping statistics --- %d packets transmitted, %d received, %d%% packet loss rtt min/avg/max = %d %d %d ms\n", 10, count, (10-count)*10, min, avg, max);

    close(sockfd);
    return 0;
}
