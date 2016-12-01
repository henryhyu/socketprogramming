/*
* * The command line arguments should be:
* * ./EmailSender <recipient address>
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#define SMTP_PORT 25 // SMTP Port Number is 25
#define MAXLINE 1024 // NOTE: Can be modified to any size you want.

char *Server_IP = "128.111.53.4"; // NOTE: IP of the mail server after running "nslookup -type=MX cs.ucsb.edu". Hence you will need to change this value.

void main (int argc, char ** argv)
{
  char sendline[MAXLINE], recvline[MAXLINE];
  char recipient_address[MAXLINE]; // "RCPT TO" address.
  char sender_address[MAXLINE]; // "MAIL FROM" adress.
  char *temp;

  if (argc != 2)
  {
    printf("usage: ./EmailSender <recipient address,\"RCPTTO\">\n");
    exit(0);
  }
  strcpy(recipient_address, argv[1]);

  /* Establish a TCP connection with the main server */
  int sockfd;
  struct sockaddr_in serveraddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0); // Initialize socket

  serveraddr.sin_family = AF_INET; // Address family that is used to designate the type of addresses socket can communicate with
  serveraddr.sin_addr.s_addr = inet_addr(Server_IP); // connect: Initializes conversation by sending appropriate introduction packets
  serveraddr.sin_port = htons(SMTP_PORT);

  connect(sockfd, (struct sockaddr*)&serveraddr,sizeof(serveraddr));


  /* Read greeting from the server */
  read(sockfd, recvline, MAXLINE);
  printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "220")!=0)
  {
    printf("220 reply not received from server.\n");
    exit(0);
  }
  /* Send HELO command and get server response */
  strcpy(sendline, "HELO alice\r\n");
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "250")!=0)
  {
    printf("250 reply not received from server.\n");
    exit(0);
  }

  // Send MAIL FROM command.
  strcpy(sender_address, "MAIL FROM: <henryyu@umail.ucsb.edu>\r\n"); // MAIL FROM: <alice@crepes.fr>
  printf("%s", sender_address);
  write(sockfd, sender_address, strlen(sender_address));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "250")!=0)
  {
    printf("250 reply not received from server.\n"); // Error code
    exit(0);
  }

  // Send RCPT TO command.

  strcpy(sendline, "RCPT TO: <"); // RCPT TO: <bob@cs.ucsb.edu> format
  strcat(sendline, recipient_address);
  strcat(sendline, ">\r\n");
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "250")!=0)
  {
    printf("250 reply not received from server.\n"); // Error code
    exit(0);
  }

  // Send DATA command.
  strcpy(sendline, "DATA\r\n"); // DATA format
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "354")!=0)
  {
    printf("354 reply not received from server.\n"); // Error code
    exit(0);
  }

  // Send message data.
  strcpy(sendline, "SUBJECT: hello\nHi Bob, How's the weather?\nAlice.\n"); // Hard coded message
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  // End with a single period.
  strcpy(sendline, ".\r\n");
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");

  // Send QUIT command.
  strcpy(sendline, "QUIT\r\n");
  printf("%s", sendline);
  write(sockfd, sendline, strlen(sendline));

  read(sockfd, recvline, MAXLINE);
  //printf("%s\n", recvline);
  temp = strtok(recvline, " ");
  if (strcmp(temp, "221")!=0)
  {
    printf("221 reply not received from server.\n"); // Error code
    exit(0);
  }

  exit(0);
}
