#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>  // socket(), bind()
#include <netinet/in.h>  // struct sockaddr , struct sockaddr_in
#define BUFSIZE 200
#define MAXUSER 200

typedef struct _List{
    int join;
    char nickname[30];
}List;


int main(int argc, char **argv){
    fd_set reads, temps;
    int result, sockfd, str_len, i;
    struct sockaddr_in dest_addr;
    char * nickname;
    char message[BUFSIZE];
    List list[MAXUSER];

    if(argc!=4){
        printf("Usage : %s <IP> <port> <nickname>\n", argv[0]);
        exit(1);
    }
    nickname = argv[3];
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {}
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(argv[2]));
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) !=0){
        close(sockfd);
        return -1;
    }

    write(sockfd, nickname, strlen(nickname)+1);

    int total_byte=0, read_byte, size;
    read(sockfd, &size, sizeof(size));
    //printf("%d",size);
    while(total_byte < size){
        if((read_byte = read(sockfd, &list[total_byte/sizeof(List)], sizeof(List)*50))>0){
            total_byte += read_byte;
            //printf("%d\n",total_byte);
        }
    }

    printf("--------joined nickname list----------\n");
    for(i=0; i<MAXUSER; i++){
        if(list[i].join == 1){
            printf("%s\n",list[i].nickname);
        }
    }

    FD_ZERO(&reads);  //set fd_set all 0
    FD_SET(0, &reads); // standard input  //set fd0 as 1
    FD_SET(sockfd, &reads); // connection with client
    
    printf("========= chating room =========\n");
    while(1){
        temps = reads;
        result = select(sockfd + 1, &temps, 0, 0, NULL); //1 : highest numbered f.d

        if (result == -1) { //errors in select 
            puts("select error!");
            exit(1);
        }
        else { // read occur
            if(FD_ISSET(0, &temps)) { //return true if 0 is in the temps
                printf("\n");
                str_len = read(0, message, 500);
                message[str_len] = '\0';
                write(sockfd, message, strlen(message)+1);
            }
            else if(FD_ISSET(sockfd, &temps)){// other write chatting
                read(sockfd, message, BUFSIZE);
                fputs(message, stdout);
            }
        }
    } // while(1)
}