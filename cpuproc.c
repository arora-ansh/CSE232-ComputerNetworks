#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>  
#include <pthread.h>

// ps -eo pcpu,pid,comm | sort -k 1 -r | head -n 6

int main(){
    int n=3; // n denotes the number of CPU processes to be returned
    char* data[n][4];
    int pid_list[3] = {1,2,3}; //Write function to extract these
    for(int i=0;i<n;i++){
        int fd, sz;
        char *c = (char *) calloc(2000, sizeof(char));
        char file_name[100] = "/proc/";
        char pid_string[10];
        sprintf(pid_string,"%d",pid_list[i]);
        strcat(file_name,pid_string);
        strcat(file_name,"/stat");
        fd = open(file_name, O_RDONLY);
        if (fd < 0) { perror("r1"); exit(1); }
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
        data[i][0] = broken_c[0];
        data[i][1] = broken_c[1];
        data[i][2] = broken_c[13];
        data[i][3] = broken_c[14];
    }
    for(int i=0;i<n;i++){
        printf("%s %s %s %s\n",data[i][0],data[i][1],data[i][2],data[i][3]);
    }
    return 0;
}