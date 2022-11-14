#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <cstring>

using namespace std;

void splitWorkload(char *dirname, char *dirPathClusterUnity, int treshold, int numP, int wkPercent){
	int wlSizeCounter, numPCounter = 1;
	DIR *dr;
    struct dirent *en;
	char np[5], wlP[5], rk[5];

	sprintf (np, "%d", numP);
	sprintf (wlP, "%d", wkPercent);

    dr = opendir(dirname); //open directory
	en = readdir(dr);
	while (numPCounter <= numP) {
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

		FILE *fptr;
		fptr = fopen(filename,"w");
		fptr = freopen(filename,"a", fptr);
	
		wlSizeCounter = 0;
		while(en != NULL && wlSizeCounter <= treshold){
			if(en->d_name[0] != '.'){
				char path[strlen(dirname) + strlen(en->d_name) + 1];
				strcpy(path,dirname);
				strcat(path, "/");
				strcat(path, en->d_name);

				char pathClusterU[strlen(dirPathClusterUnity) + strlen(en->d_name) + 1];
				strcpy(pathClusterU,dirPathClusterUnity);
				strcat(pathClusterU, "/");
				strcat(pathClusterU, en->d_name);
				strcat(pathClusterU, "\n");

				ifstream in_file(path, ios::binary);
				in_file.seekg(0, ios::end);
				wlSizeCounter += in_file.tellg();

				fputs(pathClusterU, fptr);
			}
			en = readdir(dr);
		} 
		numPCounter++;
    	fclose(fptr);
	}
	closedir(dr); 
}

int main(int argc, char** argv){
	int totalWlSize = atoi(argv[3]); // the total size in bytes
    int wkPercent = atoi(argv[4]); // the percentual of the workload to be used
	int numP = atoi(argv[5]); // number of processes that will be used to process the load
	cout << "splitting" << endl;
    splitWorkload(argv[1], argv[2], (totalWlSize * wkPercent / 100) / numP, numP, wkPercent);
	cout << "split done!" << endl;
   return(0);
}