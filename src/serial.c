/** 
 * @file    serial.c
 * @brief   Simule le comportement d'une communication série via socket
 */
// ==================== INCLUDES ====================
#include <stdio.h>
#include <stdbool.h>

#include "serial.h"

static int sockfd = -1;
static int client = -1;

// ==================== COM management ====================
int serial_init(){
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2),&wsa);
#endif
    // Socket serveur
    //int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) { perror("socket"); return 1; }

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(12345);
    serv.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd,(struct sockaddr*)&serv,sizeof(serv)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(sockfd,1) < 0) {
        perror("listen"); return 1;
    }

    //result = fcntl(sockfd, F_GETFL, NULL);
    //This sets the socket to nonblocking
    //result |= O_NONBLOCK
    //fcntl(client, F_SETFL, O_NONBLOCK);
    //ioctl(client, FIONBIO, &on);
    SetSocketBlockingEnabled(sockfd, false);

    printf("[SER] Waiting connection...\n");
return sockfd;
}

int serial_accept(){
    client = accept(sockfd,NULL,NULL);
    
    if (client < 0) { perror("accept"); return -1; }
    printf("[SER] Client connected.\n");

    return client;
}

//void serial_close(int sockfd, int client){
void serial_close(){
      #ifdef _WIN32
        closesocket(client); closesocket(sockfd); WSACleanup();
    #else
        close(client); close(sockfd);
    #endif
}

// ==================== Message Management ====================
void send_response(const char *msg) {
    send(client, msg, strlen(msg), 0);
}

void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\r' || s[n-1] == '\n')) { s[--n] = 0; }
}

void receive_request(char buf[256], int len) {
        //read from usb/observer
        //int n = recv(client, buf, sizeof(buf)-1, 0);
        //printf("[MCU] Waitning for data... \n");
        int n = recv(client, buf, len-1, 0);
        //if(n <= 0) break;
        if(n <= 0){
            buf[0] = 0;
            return;
        }
        buf[n] = 0;
        //if(strlen(buf) == 0) return;
        trim_newline(buf);
        //printf("[SER] Data received ! \n");
}

/** Returns true on success, or false if there was an error */
bool SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef _WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0);
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0);
#endif
}

