#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>  // socket(), bind()
#include <netinet/in.h>  // struct sockaddr , struct sockaddr_in
#include <string.h>
#define BUFSIZE 200
#define MAXUSER 200

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

typedef struct _List{
    int join;
    char nickname[30];
}List;


int main(int argc, char **argv){
    int serv_sock;
    struct sockaddr_in serv_addr;

    fd_set reads, temps;
    int i, fd_max, str_len;
    char message[BUFSIZE];
    char message2[BUFSIZE];
    char nicknames[BUFSIZE];
    List list[MAXUSER];
    //struct timeval timeout;

    for(i=0; i<MAXUSER; i++){
        list[i].join = 0;
    }

    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))){
        error_handling("bind() error");
    }
    if(listen(serv_sock,5) == -1){
        error_handling("listen() error");
    }

    FD_ZERO(&reads); // 0 0 0 0 ...
    FD_SET(serv_sock, &reads); // make bit 1 in fd(serv_sock discriptor)
    fd_max = serv_sock;

    while(1){
        int fd, str_len;
        int clnt_sock, clnt_len;
        struct sockaddr_in clnt_addr;

        temps = reads;
        if(select(fd_max+1, &temps, 0, 0, NULL) == -1){
            error_handling("select() error");
        }

        for(fd=0; fd < fd_max+1; fd++){
            if(FD_ISSET(fd, &temps)){
                if(fd == serv_sock){ // when client try to connect with server
                    clnt_len = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_len);
                    str_len = read(clnt_sock, nicknames, BUFSIZE);
                    
                    list[clnt_sock].join = 1;
                    strcpy(list[clnt_sock].nickname, nicknames);
                    sprintf(message, "client %d join, nickname : %s\n", clnt_sock, nicknames);
                    for(i=0; i<fd_max+1; i++){// let exist user that new client join
                        if(FD_ISSET(i, &reads) && (i != serv_sock)){
                            write(i, message, strlen(message)+1);               
                        }
                    }
                    // let new clinet know the list of nickname who is join in chatting
                    int size = sizeof(List)*MAXUSER;
                    write(clnt_sock, &size, sizeof(size));
                    size = write(clnt_sock, list, sizeof(List)*MAXUSER);

                    FD_SET(clnt_sock, &reads);
                    if(fd_max <clnt_sock){
                        fd_max = clnt_sock;
                    }
                    printf("connected client: %d\n",clnt_sock);
                }
                else{
                    str_len = read(fd, message, BUFSIZE);
                    if(str_len == 0){
                        sprintf(message, "client %d leave, nickname : %s\n", fd, list[fd].nickname);
                        for(i=0; i<fd_max+1; i++){
                            if(FD_ISSET(i, &reads) && (i != serv_sock)){
                                write(i, message, strlen(message));               
                            }
                        }
                        list[fd].join = 0;
                        FD_CLR(fd, &reads);
                        close(fd);
                        printf("closed client: %d\n", fd);
                    }
                    else{// if a clinet write chatting, send to every one
                        sprintf(message2, "%s : %s\n", list[fd].nickname, message);
                        for(i=0; i<fd_max+1; i++){
                            if(FD_ISSET(i, &reads) && (i != serv_sock) && (i != fd)){
                                write(i, message2, strlen(message2)+1);               
                            }
                        }
                    }
                }
            }
        }
    }
}