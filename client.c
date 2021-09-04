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
    
    // Receiving top processes calculated by server

    printf("Top processes returned by server- \n");
    int flag = 0;
    char* msg_received = "received";

    FILE *filePointer;
    char filename[20] = "client_";
    strcat(filename,client_no_str);
    filePointer = fopen(filename, "w") ;
    
    while(flag<x){
        char single_data_row[100] = {0}; //Will hold the data that needs to be processed back to the client
        int valread2 = read(sock, single_data_row, 100);
        if(valread2>0){
            printf("%s\n",single_data_row);
            fprintf(filePointer,"%s\n",single_data_row);
            flag += 1;
            send(sock,msg_received,strlen(msg_received),0);
        }
    }
    fclose(filePointer);

    // Following code to find top most process, server side
    char max_pid[10] = {0};
    char max_proc_name[100] = {0};
    int max_cpu_usage = -1;
    // dirent.h functions to check what all folders belong to /proc/  
    struct dirent *de; // Pointer for directory entry
	DIR *dr = opendir("/proc");
	if (dr == NULL) {
		printf("Could not open current directory");
		return 0;
	}
    int pid_array[1000]; //Array to hold current open processes' PID
    int sz = 0;
	while ((de = readdir(dr)) != NULL){
		// printf("%s\n", de->d_name);
        if(de->d_name[0]!='\0' && de->d_name[0]>=49 && de->d_name[0]<=57){ //Thus only considering numerical values, hence getting the pid values
            pid_array[sz++] = atoi(de->d_name);
        }
    }
	closedir(dr);

    int n = sz; // n denotes the number of CPU processes to be returned
    for(int i=0;i<n;i++){
        int fd, sz;
        char *c = (char *) calloc(2000, sizeof(char));
        char file_name[100] = "/proc/";
        char pid_string[10];
        sprintf(pid_string,"%d",pid_array[i]);
        strcat(file_name,pid_string);
        strcat(file_name,"/stat");
        fd = open(file_name, O_RDONLY);
        if (fd < 0) { perror("r1"); continue; } //Tthis gets triggered, especially when processes get killed during the process, very rare though
        sz = read(fd, c, 2000);
        c[sz] = '\0';
        // c now holds proc/<pid>/stat file, from which we need to extract the data that we need 
        char * token = strtok(c," ");
        char * broken_c[52];
        int j = 0;
        while(token != NULL){
            broken_c[j++] = token;
            token = strtok(NULL," ");
        }
        char cur_pid[10] = {0}; strcpy(cur_pid,broken_c[0]);
        char cur_proc_name[100] = {0}; strcpy(cur_proc_name,broken_c[1]);
        char cur_cpu_user[10] = {0}; strcpy(cur_cpu_user,broken_c[13]);
        char cur_cpu_kernel[10] = {0}; strcpy(cur_cpu_kernel,broken_c[14]);
        int cur_cpu_user_val = atoi(cur_cpu_user);
        int cur_cpu_kernel_val = atoi(cur_cpu_kernel);
        if(cur_cpu_kernel_val+cur_cpu_user_val>max_cpu_usage){
            max_cpu_usage = cur_cpu_kernel_val+cur_cpu_user_val;
            strcpy(max_pid,cur_pid);
            strcpy(max_proc_name,cur_proc_name);
        }
    }

    //Sending topmost process found here to server
    printf("Max CPU Usage Process found client side- %s %s %d\n",max_pid,max_proc_name,max_cpu_usage);
    char final_client_result[200];
    sprintf(final_client_result, "Max CPU Usage Process found by client- %s %s %d\n",max_pid,max_proc_name,max_cpu_usage);
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