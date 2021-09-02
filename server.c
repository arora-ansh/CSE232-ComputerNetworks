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
    printf("Client Message: %s\n",((struct args*)input)->client_msg);
    char* server_msg = "Server is done with you";
    send(((struct args*)input)->socket_id , server_msg , strlen(server_msg) , 0 );
    return NULL;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Connected to server";
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
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
        if (listen(server_fd, 3) < 0){
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        send(new_socket , hello , strlen(hello) , 0 );
        valread = read( new_socket , buffer, 1024);
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