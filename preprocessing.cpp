#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 

using namespace std;

int countWorloadSize(string dirPath, char *dirname,  string dirPathClusterUnity){
	DIR *dr;
    struct dirent *en;
    int wlSize = 0, filesCounter = 0;

    dr = opendir(dirname); //open directory
   	ofstream MyFile("fileNames.txt");

	en = readdir(dr);
	while(en != NULL){
		if((dirPath + '/' + en->d_name != dirPath + '/' + ".") && (dirPath + '/' + en->d_name != dirPath + '/' + "..")){
			ifstream in_file(dirPath + '/' + en->d_name, ios::binary);
			in_file.seekg(0, ios::end);
			wlSize += in_file.tellg();
			MyFile << dirPathClusterUnity + '/' + en->d_name << endl;
			filesCounter++;
		}
		en = readdir(dr);
	} 
    MyFile.close();
	closedir(dr); 
	
	return wlSize;
}

void splitWorkload(string dirPath, char *dirname,  string dirPathClusterUnity, int treshold, int numP, int wkPercent){
	int wlSizeCounter, numPCounter = 1;

	DIR *dr;
    struct dirent *en;

    dr = opendir(dirname); //open directory
	en = readdir(dr);
	while (numPCounter <= numP) {
		ofstream MyFile("setup-results/" + 
                std::to_string(numP) + "processes/" +
                "problemSize" + std::to_string(wkPercent) + 
                "/fileNames4Rank" + std::to_string(numPCounter) + ".txt");
	
		wlSizeCounter = 0;
		while(en != NULL && wlSizeCounter <= treshold){
			if((dirPath + '/' + en->d_name != dirPath + '/' + ".") && (dirPath + '/' + en->d_name != dirPath + '/' + "..")){
				ifstream in_file(dirPath + '/' + en->d_name, ios::binary);
				in_file.seekg(0, ios::end);
				wlSizeCounter += in_file.tellg();
				MyFile << dirPathClusterUnity + '/' + en->d_name << endl;
			}
			en = readdir(dr);
		} 
		numPCounter++;
    	MyFile.close();
	}
	closedir(dr); 
}

int main(int argc, char** argv){
    std::string dirPath(argv[1]); // get the name of the folder (at the front-end machine) containing the tweets (the folder must be on the same directory as the code)
    std::string dirPathClusterUnity(argv[2]); // get the path of the folder for the cluster unities 
	int totalWlSize = atoi(argv[3]); // the total amount of tweet files 
    int wkPercent = atoi(argv[4]); // the percentual of the workload to be used
	int numP = atoi(argv[5]); // number of processes that will be used to process the load

    splitWorkload(dirPath, argv[1], dirPathClusterUnity, (totalWlSize * wkPercent / 100) / numP, numP, wkPercent);

   return(0);
}