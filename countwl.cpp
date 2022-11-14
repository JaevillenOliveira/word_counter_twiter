#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <cstring>

using namespace std;

int main(int argc, char** argv){
    //std::string dirPath(argv[1]); // get the name of the folder (at the front-end machine) containing the tweets (the folder must be on the same directory as the code)
    DIR *dr;
    struct dirent *en;
    int wlSize = 0;
    ofstream fnames("fnames.txt");
    dr = opendir(argv[1]); //open directory
	en = readdir(dr);
	while(en != NULL){
        char path[strlen(argv[1]) + strlen(en->d_name) + 1];
        strcpy( path, argv[1] );
        strcat( path, "/");
        strcat( path, en->d_name );

		if(en->d_name[0] != '.'){
			ifstream in_file(path, ios::binary);
			in_file.seekg(0, ios::end);
			wlSize += in_file.tellg();
            fnames << path << endl;
		}
		en = readdir(dr);
	} 
    fnames.close();
    ofstream MyFile("workloadSize.txt");
    MyFile << wlSize << endl;
    MyFile.close();
	closedir(dr); 

   return(0);
}