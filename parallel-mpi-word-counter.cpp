#include<bits/stdc++.h>
#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <stdio.h>
#include <ctype.h>
#include <cstdlib>
#include <cctype>
#include <omp.h>
#include <map>
#include <set>
#include <vector>
#include <locale.h>
#include <mpi.h>

using namespace std;

std::set <string> readKeywords(string fileName){
   std::set <string> keywords;
   string keyword;
   ifstream MyReadFile(fileName);

   while (getline (MyReadFile, keyword)) {
      keywords.insert(keyword);
   }
   MyReadFile.close();
   return keywords;
}

bool isNotAlpha(char c) {
   return !isalpha(c);
}

bool isValidWord(string word){
   if(word.size() <= 2)
      return false;
   if(word[0] == '#' or word[0] == '@')
      return false;
   if(word.find("http") != std::string::npos)
      return false;
   return true;
}

void clearWord(string &word){
   transform(word.begin(), word.end(), word.begin(), ::tolower);
//    word.erase(remove_if(word.begin(), word.end(), isNotAlpha), word.end());
}

char removeAccentuation(char c, char h){
	string comAcentos = "ÄÅÁÂÀÃäáâàãÉÊËÈéêëèÍÎÏÌíîïìÖÓÔÒÕöóôòõÜÚÛüúûùÇç";
	string semAcentos = "AAAAAAaaaaaEEEEeeeeIIIIiiiiOOOOOoooooUUUuuuuCc";

	for (int i = 0; i < comAcentos.size(); i += 2){
		if(c == comAcentos[i] and h == comAcentos[i+1]){
			return semAcentos[i/2];
		}
	}
	return c;
}

int getWordIndex(std::set<string> keywords, string wordToFind){
	int index = 0;
	for (auto el : keywords) {
		if (el == wordToFind)
			return index;
		index++;
	}
	return -1;
}

void handleTweet(string fileName, std::set<string> keywords, std::vector <int> &wordCounter){
	//Creating an input stream to read a file
	ifstream ifstream_ob;
	ifstream_ob.open(fileName, ios::in);

	char ch;
	int wordAt;
	std::string str;

	str.clear();
	while(ifstream_ob){
		ch = ifstream_ob.get(); 
		if(!isascii(ch)){
			ch = removeAccentuation(ch, ifstream_ob.get());
		}
		if(ch != ' ' and ch != '\t' and ch != '\n' and isascii(ch) and ch != EOF){ 
			str += ch;
		}else{
			if(isValidWord(str)){
				clearWord(str);
				wordAt = getWordIndex(keywords, str);
				if(wordAt != -1){
					wordCounter.at(wordAt)++;

				}
			}
			str.clear();
		}
	}
	ifstream_ob.close();
}

void writeKeywordsCounter(std::set<string> keywords, std::vector <int> wordCounter, int numFiles, int numP){
   std::set <string>::iterator itKeywrds;

   	ofstream MyFile("results/"+std::to_string(numP)+"threads/"+std::to_string(numFiles)+"files/keywordsCounter.txt");
   	int index = 0;
	for (auto el : keywords) {
		MyFile << el << ": " << wordCounter.at(index) << endl;
		index++;
	}
   MyFile.close();
}

void manualMerge(std::vector <int> &out, std::vector <int> &in){
	std::transform (out.begin(), out.end(), in.begin(), out.begin(),std::plus<int>());
}

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

void splitWorkload(string dirPath, char *dirname, int treshold, int numP){
	int wlSizeCounter, filesCounter = 0, numPCounter = 1;

	ifstream MyReadFile("fileNames.txt");
	string fileName;
	while (numPCounter <= numP) {
		ofstream MyFile("fileNames4Rank" + std::to_string(numPCounter++) + ".txt");
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
    setlocale(LC_ALL, "Portuguese");

    std::string dirPath(argv[1]); // get the name of the folder containing the tweets (the folder must be on the same directory as the code)
    std::string keywrdsFileName(argv[2]); // get the name of the file containing the keywords (the file must be on the same directory as the code)
    std::set <string> keywords = readKeywords(keywrdsFileName);
	std::vector <int> finalWordCounter (keywords.size()); 
    int numFiles = atoi(argv[3]);//, numThreads = atoi(argv[4]);
	int numP, myRank, rc, receivedMsgs;
	MPI_Status status;


	rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		return -1;
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numP);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	if(myRank == 0){
		splitWorkload(dirPath, argv[1], countWorloadSize(dirPath, argv[1], numFiles) / numP, numP);
		receivedMsgs = numP - 1;
		while((receivedMsgs--) > 0){
			std::vector <int> tempWordCounter (keywords.size()); 
			// wait for requests, get the source
			MPI_Recv(&tempWordCounter[0], keywords.size(), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			manualMerge(finalWordCounter, tempWordCounter);
		}
		writeKeywordsCounter(keywords, finalWordCounter, numFiles, numP);
	}else {
		std::vector <int> myWordCounter (keywords.size()); 
		ifstream MyReadFile("fileNames4Rank" + std::to_string(myRank) + ".txt");
		string fileName;
		while (getline (MyReadFile, fileName)) {
			handleTweet(fileName, keywords, myWordCounter); 
		}
		MPI_Send(&myWordCounter[0], keywords.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
    MPI_Finalize();
   return(0);
}

