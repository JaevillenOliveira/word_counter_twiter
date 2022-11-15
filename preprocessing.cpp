#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <cstring>
#include <sys/stat.h>
#include <stdio.h>


using namespace std;

void splitWorkload(char *dirname, char *dirPathClusterUnity, long treshold, int numP, int wkPercent){
	DIR *dr;
	FILE *fptr;
    struct dirent *en;
	struct stat st;
	char np[5], wlP[5], rk[5];
	long wlSizeCounter = 0;
	int numPCounter = 1;

	sprintf (np, "%d", numP);
	sprintf (wlP, "%d", wkPercent);

    dr = opendir(dirname); //open directory
	en = readdir(dr);
	cout << "Treshold: " << treshold << endl;
	while (numPCounter <= numP) {
		cout << "Process: " << numPCounter << endl;
		sprintf (rk, "%d", numPCounter);
		
		char filename[200];
		strcpy(filename,"setup-results/");
		strcat(filename, np);
		strcat(filename, "processes/");
		strcat(filename, "problemSize");
		strcat(filename, wlP);
		strcat(filename, "/fileNames4Rank");
		strcat(filename, rk);
		strcat(filename, ".txt");

		cout << filename << endl;

		fptr = fopen(filename,"w");
		fptr = freopen(filename,"a", fptr);
	
		wlSizeCounter = 0;
		while(en != NULL && wlSizeCounter < treshold){
			if(en->d_name[0] != '.'){
				char path[strlen(dirname) + strlen(en->d_name) + 1];
				strcpy(path,dirname);
				strcat(path, en->d_name);

				char pathClusterU[strlen(dirPathClusterUnity) + strlen(en->d_name) + 1];
				strcpy(pathClusterU,dirPathClusterUnity);
				strcat(pathClusterU, en->d_name);
				strcat(pathClusterU, "\n");

				stat(path, &st);
				wlSizeCounter += st.st_size;

				fputs(pathClusterU, fptr);
			}
			en = readdir(dr);
		} 
		cout << "wlSizeCounter: " << wlSizeCounter << endl;
		numPCounter++;
    	fclose(fptr);
	}
	closedir(dr); 
}

int main(int argc, char** argv){
	long totalWlSize = atoi(argv[3]); // the total size in bytes
    int wkPercent = atoi(argv[4]); // the percentual of the workload to be used
	int numP = atoi(argv[5]); // number of processes that will be used to process the load
	cout << "splitting" << endl;
    splitWorkload(argv[1], argv[2], (totalWlSize * wkPercent / 100) / numP, numP, wkPercent);
	cout << "split done!" << endl;
   return(0);
}