#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>  
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
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
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    int con = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (con < 0){
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
    
    // char* top_data[x][4];//Top data will store the received the top n processes data

    int flag = 0;
    char* msg_received = "received";
    char max_pid[10] = {0};
    char max_proc_name[100] = {0};
    int max_cpu_usage = -1;

    FILE *filePointer;
    char filename[20] = "client_";
    strcat(filename,client_no_str);
    filePointer = fopen(filename, "w") ;
    
    while(flag<x){
        char single_data_row[100] = {0}; //Will hold the data that needs to be processed back to the client
        int valread2 = read(sock, single_data_row, 100);
        if(valread2>0){
            // printf("%s\n",single_data_row);
            fprintf(filePointer,"%s\n",single_data_row);
            //process the received row and store it in top_data
            char * token = strtok(single_data_row," ");
            // printf("%s \n",token);
            int j = 0;
            int cur_proc_sum = 0;
            char cur_proc_name[100] = {0};
            char cur_proc_pid[10] = {0};
            while(token != NULL && j<4){
                if(j==0){
                    strcpy(cur_proc_pid,token);
                }
                else if(j==1){
                    strcpy(cur_proc_name,token);
                }
                else if(j==2 || j==3){
                    cur_proc_sum += atoi(token);
                }
                token = strtok(NULL," ");
                j+=1;
            }
            //If cur_proc_sum greater than max_cpu_usage, reallocate them these new values
            if(cur_proc_sum>max_cpu_usage){
                max_cpu_usage = cur_proc_sum;
                strcpy(max_proc_name,cur_proc_name);
                strcpy(max_pid,cur_proc_pid);
            }
            flag += 1;
            send(sock,msg_received,strlen(msg_received),0);
        }
    }
    fclose(filePointer);

    printf("Max CPU Usage Process found- %s %s %d\n",max_pid,max_proc_name,max_cpu_usage);
    char final_client_result[200];
    sprintf(final_client_result, "Max CPU Usage Process found- %s %s %d\n",max_pid,max_proc_name,max_cpu_usage);
    send(sock,final_client_result,strlen(final_client_result),0);
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