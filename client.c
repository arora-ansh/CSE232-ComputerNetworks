#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080
   
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char hello[50] = "Hello from client ";
    char client_no_str[10];
    scanf("%s",client_no_str);
    strcat(hello,client_no_str);
    printf("%s",hello);
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    int flag = 0;
    int valread2;
    while(!flag){
        valread2 = read( sock , buffer, 1024);
        if(valread2>0){
            flag = 1;
            printf("%s\n",buffer );
        }
    }
    return 0;
}