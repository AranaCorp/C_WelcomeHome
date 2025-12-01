// commande.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

int recv_line(int sock, char *buf, int maxlen) {
    int i=0; char c;
    while(i<maxlen-1) {
        int n=recv(sock,&c,1,0);
        if(n<=0) return n;
        buf[i++]=c;
        if(c=='\n') break;
    }
    buf[i]=0; return i;
}

int main() {
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2),&wsa);
#endif
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(12345);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd,(struct sockaddr*)&serv,sizeof(serv))<0) {
        perror("Connexion echouee"); return 1;
    }
    printf("[CMD] Connecte au device.\n");

    char cmd[128], buf[256];
    while(1) {
        printf("Commande (btn in/out, set_delay x, set_analog x, cmd on/off, status, date, sync, clear, quit : \n");
        if(!fgets(cmd,sizeof(cmd),stdin)) break;
        if(strncmp(cmd,"quit",4)==0) {
            send(sockfd,cmd,strlen(cmd),0);
            recv_line(sockfd,buf,sizeof(buf));
            printf("[CMD] Recv: %s",buf);
            break;
        }
        if(strlen(cmd) > 1 && cmd[strlen(cmd)-1]=='\n'){
            printf("[CMD] Send: %s",cmd);
            send(sockfd,cmd,strlen(cmd),0);
            int n=recv_line(sockfd,buf,sizeof(buf));
            if(n>0) printf("[CMD] Recv: %s\n",buf);
        }
    }

#ifdef _WIN32
    closesocket(sockfd); WSACleanup();
#else
    close(sockfd);
#endif
    return 0;
}