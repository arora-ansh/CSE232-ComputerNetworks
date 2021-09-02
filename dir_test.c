#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>  
#include <pthread.h>

//Implement merge sort for top n processes
void merge(int arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;

	/* create temp arrays */
	int L[n1], R[n2];

	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
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

	/* Copy the remaining elements of L[], if there
	are any */
	while (i < n1) {
		arr[k] = L[i];
		i++;
		k++;
	}

	/* Copy the remaining elements of R[], if there
	are any */
	while (j < n2) {
		arr[k] = R[j];
		j++;
		k++;
	}
}

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
	if (l < r) {
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l + (r - l) / 2;

		// Sort first and second halves
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);

		merge(arr, l, m, r);
	}
}

int main(void){
	struct dirent *de; // Pointer for directory entry

	// opendir() returns a pointer of DIR type.
	DIR *dr = opendir("/proc");

	if (dr == NULL) // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory" );
		return 0;
	}
    int pid_array[1000];
    int sz = 0;
	while ((de = readdir(dr)) != NULL){
		// printf("%s\n", de->d_name);
        if(de->d_name[0]!='\0' && de->d_name[0]>=49 && de->d_name[0]<=57){
            pid_array[sz++] = atoi(de->d_name);
        }
    }
    printf("%d\n",sz);
    // for(int i=0;i<sz;i++) printf("%d\n",pid_array[i]);
	closedir(dr);	

    int n = sz; // n denotes the number of CPU processes to be returned
    char* data[n][4];
    // int pid_list[3] = {1,2,3}; //Write function to extract these
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
    for(int i=0;i<n;i++){
        printf("%s %s %s %s\n",data[i][0],data[i][1],data[i][2],data[i][3]);
    }

	return 0;
}
