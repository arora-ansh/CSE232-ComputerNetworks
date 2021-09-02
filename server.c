#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>  
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>


#define PORT 8080

void merge(int arr[], int low, int mid, int high){
	int i, j, k;
	int n1 = mid - low + 1;
	int n2 = high - mid;

	int L[n1], R[n2];

	for (i = 0; i < n1; i++)
		L[i] = arr[low + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[mid + 1 + j];

	i = 0; 
	j = 0; 
	k = low; 

	while (i < n1 && j < n2) {
		if (L[i] <= R[j]) {
			arr[k] = L[i];
			i++;
		}
		else {
			arr[k] = R[j];
			j++;
		}
		k++;
	}
	while (i < n1) {
		arr[k] = L[i];
		i++;
		k++;
	}
	while (j < n2) {
		arr[k] = R[j];
		j++;
		k++;
	}
}

void sort(int arr[], int low, int high) {
	if (low < high) {
		int mid = low + (high - low) / 2;
		sort(arr, low, mid);
		sort(arr, mid + 1, high);
		merge(arr, low, mid, high);
	}
}

struct args {
    char client_msg[100];
    int socket_id;
};

void *threadFunc(void *input){
    // printf("Going to put this thread in sleep for 20 secs\n");
    // sleep(20);
    char client_pt_req[10];
    int flag = 0;
    while(!flag){
        int rt = read(((struct args*)input)->socket_id, client_pt_req, 1024);
        if(rt>0){
            flag = 1;
            printf("Client requests top %d processes\n",atoi(client_pt_req));
        }
    }
    int top_x = atoi(client_pt_req);

    struct dirent *de; // Pointer for directory entry

	// dirent.h functions to check what all folders belong to /proc/  
	DIR *dr = opendir("/proc");
	if (dr == NULL) {
		printf("Could not open current directory" );
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
    // printf("%d\n",sz);
    // for(int i=0;i<sz;i++) printf("%d\n",pid_array[i]);
	closedir(dr);

    int n = sz; // n denotes the number of CPU processes to be returned
    char* data[n][4];
    for(int i=0;i<n;i++){
        int fd, sz;
        char *c = (char *) calloc(2000, sizeof(char));
        char file_name[100] = "/proc/";
        char pid_string[10];
        sprintf(pid_string,"%d",pid_array[i]);
        strcat(file_name,pid_string);
        strcat(file_name,"/stat");
        fd = open(file_name, O_RDONLY);
        if (fd < 0) { perror("r1"); continue; }
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
    // int top_x = 10; //Number of top cpu processes needed to be found
    int cpuusage_arr[n]; //Holds the cpuusage for all processes
    for(int i=0;i<n;i++){
        // printf("%s %s %s %s\n",data[i][0],data[i][1],data[i][2],data[i][3]);
        int total_cpuval = atoi(data[i][2])+atoi(data[i][3]);
        cpuusage_arr[i] = total_cpuval;
    }
    sort(cpuusage_arr,0,n-1); //Sorted, to find top (top_x)# of processes
    // for(int i=0;i<n;i++) printf("%d ",cpuusage_arr[i]);
    // printf("\n");
    int cutoff_usage = cpuusage_arr[n-top_x]; //nth largest value stored here
    char* top_data[top_x][4]; //Will hold the data that needs to be processed back to the client
    int j = 0;
    for(int i=0;i<n;i++){
        if(j<top_x && atoi(data[i][2])+atoi(data[i][3])>=cutoff_usage){
            top_data[j][0] = data[i][0];
            top_data[j][1] = data[i][1];
            top_data[j][2] = data[i][2];
            top_data[j][3] = data[i][3];
            j+=1;
        }
    }
    for(int i=0;i<top_x;i++){
        printf("%s %s %s %s\n",top_data[i][0],top_data[i][1],top_data[i][2],top_data[i][3]);
    }
    sleep(20);
    printf("Client Message: %s\n",((struct args*)input)->client_msg);
    char* server_msg = "Server is done with you";
    send(((struct args*)input)->socket_id , server_msg , strlen(server_msg) , 0 );
    return NULL;
}

int main(int argc, char const *argv[])
{
    
    char buffer[1024] = {0};
    char *hello = "Connected to server";
       
    // Creating socket file descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    int thread_count = 0;
    pthread_t thread_id[20];
    while(1){
        
        //Listening to socket for potential connection
        if (listen(server_fd, 3) < 0){
            perror("listen");
            exit(EXIT_FAILURE);
        }
        int new_socket  = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket<0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        send(new_socket , hello , strlen(hello) , 0 );
        int valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer);
        struct args *thread_data = (struct args *)malloc(sizeof(struct args));
        strcpy(thread_data->client_msg, buffer);
        thread_data->socket_id = new_socket;
        pthread_create(&thread_id[thread_count++], NULL, threadFunc, (void *)thread_data);
        // printf("Hello message sent\n");
    }
    for(int i=0;i<thread_count;i++){
        pthread_join(thread_id[i], NULL);
    }
    return 0;
}