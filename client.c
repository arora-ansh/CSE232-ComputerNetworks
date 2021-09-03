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
    
    printf("Enter number of top processes you want to see-\n");
    int x;
    scanf("%d",&x);
    char top_x[10];
    sprintf(top_x,"%d",x);
    send(sock , top_x , strlen(top_x) , 0 );
    
    int flag = 0;
    char* single_data_row; //Will hold the data that needs to be processed back to the client
    while(flag<x){
        int valread2 = read(sock, single_data_row, 100);
        if(valread2>0){
            flag += 1;
            printf("%s\n",single_data_row);
        }
    }
    flag = 0;
    int valread3;
    while(!flag){
        valread3 = read( sock , buffer, 1024);
        if(valread3>0){
            flag = 1;
            printf("%s\n",buffer );
        }
    }
    return 0;
}