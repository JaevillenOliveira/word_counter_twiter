#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <cstring>
#include <sys/stat.h>

using namespace std;

int main(int argc, char** argv){
    //std::string dirPath(argv[1]); // get the name of the folder (at the front-end machine) containing the tweets (the folder must be on the same directory as the code)
    DIR *dr;
    struct dirent *en;
    long wlSize = 0;
    struct stat st;

    dr = opendir(argv[1]); //open directory
	en = readdir(dr);
	while(en != NULL){
        char path[strlen(argv[1]) + strlen(en->d_name) + 1];
        strcpy( path, argv[1] );
        strcat( path, "/");
        strcat( path, en->d_name );

		if(en->d_name[0] != '.'){
            
            stat(path, &st);
			// ifstream in_file(path, ios::binary);
			// in_file.seekg(0, ios::end);
            cout << wlSize << " + " << st.st_size << " = ";
			wlSize += st.st_size;
            cout << wlSize << endl;
		}
		en = readdir(dr);
	} 
    ofstream MyFile("workloadSize.txt");
    MyFile << wlSize << endl;
    MyFile.close();
	closedir(dr); 

   return(0);
}