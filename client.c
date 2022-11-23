#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>  // socket(), bind()
#include <netinet/in.h>  // struct sockaddr , struct sockaddr_in
//#define DEST_IP “10.12.110.57”
#define BUFSIZE 100
#define M_USER 100

typedef struct _List{
    int join;
    char nickname[30];
}List;

// typedef struct _List{
//     int exist;
//     char nickname[BUFSIZE];
// }List;

// typedef struct _User{
//     int num;
//     List list[M_USER];
// }User;


int main(int argc, char **argv){
    fd_set reads, temps;
    int result;
    int sockfd;
    char message[500];
    int str_len;
    int total_user, i;
    struct timeval timeout;
    struct sockaddr_in dest_addr;
    char * nickname;
    List list[100];

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

    // int len = read(sockfd, list, sizeof(List)*200);
    // printf("len : %d\n",len);
    // printf("--------joined nickname list----------\n");
    // for(i=0; i<100; i++){
    //     if(list[i].join == 1){
    //         printf("%s\n",list[i].nickname);
    //     }
    // }
    // printf("\n");

    FD_ZERO(&reads);  //set fd_set all 0
    FD_SET(0, &reads); // standard input  //set fd0 as 1
    FD_SET(sockfd, &reads); // connection with client
    
    printf("========= chating room =========\n");
    while(1){
        printf("zz\n");
        temps = reads;
        // timeout.tv_sec = 1000;
        // timeout.tv_usec = 0;
        result = select(sockfd + 1, &temps, 0, 0, NULL); //1 : highest numbered f.d

        if (result == -1) { //errors in select 
            puts("select error!");
            exit(1);
        }
        else { // read occur
            if(FD_ISSET(0, &temps)) { //return true if 0 is in the temps
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