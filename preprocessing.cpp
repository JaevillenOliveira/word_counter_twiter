#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 

using namespace std;

int countWorloadSize(string dirPath, char *dirname, int numFiles){
	DIR *dr;
    struct dirent *en;
    int wlSize = 0, filesCounter = 0;

    dr = opendir(dirname); //open directory
   	ofstream MyFile("fileNames.txt");

	en = readdir(dr);
	while(en != NULL && filesCounter < numFiles){
		if((dirPath + '/' + en->d_name != dirPath + '/' + ".") && (dirPath + '/' + en->d_name != dirPath + '/' + "..")){
			ifstream in_file(dirPath + '/' + en->d_name, ios::binary);
			in_file.seekg(0, ios::end);
			wlSize += in_file.tellg();
			MyFile << dirPath + '/' + en->d_name << endl;
			filesCounter++;
		}
		en = readdir(dr);
	} 
    MyFile.close();
	closedir(dr); 
	
	return wlSize;
}

void splitWorkload(string dirPath, char *dirname, int treshold, int numP, int wkPercent){
	int wlSizeCounter, filesCounter = 0, numPCounter = 1;

	ifstream MyReadFile("fileNames.txt");
	string fileName;
	while (numPCounter <= numP) {
		ofstream MyFile("setup-results/" + 
                std::to_string(numP) + "processes/" +
                "problemSize" + std::to_string(wkPercent) + 
                "/fileNames4Rank" + std::to_string(numPCounter++) + ".txt");
		wlSizeCounter = 0;
		while(wlSizeCounter <= treshold && getline (MyReadFile, fileName)){
			ifstream in_file(fileName, ios::binary);
			in_file.seekg(0, ios::end);
			wlSizeCounter += in_file.tellg();
			MyFile << fileName << endl;
		}
	}
}

int main(int argc, char** argv){
    std::string dirPath(argv[1]); // get the name of the folder containing the tweets (the folder must be on the same directory as the code)
    int numFiles = atoi(argv[2]);
    int wkPercent = atoi(argv[3]);
	int numP = atoi(argv[4]);

    splitWorkload(dirPath, argv[1], countWorloadSize(dirPath, argv[1], numFiles) / numP, numP, wkPercent);

   return(0);
}