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

void writeKeywordsCounter(std::set<string> keywords, std::vector <int> wordCounter, int wkPercent, int numP){
   	std::set <string>::iterator itKeywrds;
   	ofstream MyFile("setup-results/" + 
                std::to_string(numP) + "processes/" +
                "problemSize" + std::to_string(wkPercent) + "/keywordsCounter.txt");
   	int index = 0;
	for (auto el : keywords) {
		MyFile << el << ": " << wordCounter.at(index) << endl;
		index++;
	}
   	MyFile.close();
}

void manualReduction(std::vector <int> &out, std::vector <int> &in){
	std::transform (out.begin(), out.end(), in.begin(), out.begin(),std::plus<int>());
}

int main(int argc, char** argv){
    setlocale(LC_ALL, "Portuguese");

    std::string keywrdsFileName(argv[1]); // get the name of the file containing the keywords (the file must be on the same directory as the code)
    std::set <string> keywords = readKeywords(keywrdsFileName);
	std::vector <int> finalWordCounter (keywords.size()); 
    int wkPercent = atoi(argv[2]);//, numThreads = atoi(argv[4]);
	int numP, myRank, rc, receivedMsgs;
	MPI_Status status;

	time_t start, end;

	rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		return -1;
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numP);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	if(myRank == 0){
		time(&start);

		receivedMsgs = numP - 1;
		while((receivedMsgs--) > 0){
			std::vector <int> tempWordCounter (keywords.size()); 
			// wait for requests, get the source
			MPI_Recv(&tempWordCounter[0], keywords.size(), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			manualReduction(finalWordCounter, tempWordCounter);
		}
		time(&end);
		double time_taken = double(end - start);

		ofstream TimeFile("timelog.txt", ios::app);
		TimeFile << "    Time: " << time_taken << setprecision(5) << endl;
		TimeFile.close();
		writeKeywordsCounter(keywords, finalWordCounter, wkPercent, numP);
	}else {
		std::vector <int> myWordCounter (keywords.size()); 
		ifstream MyReadFile("setup-results/" + 
                std::to_string(numP) + "processes/" +
                "problemSize" + std::to_string(wkPercent) + 
				"/fileNames4Rank" + std::to_string(myRank) + ".txt");
		string fileName;
		while (getline (MyReadFile, fileName)) {
			handleTweet(fileName, keywords, myWordCounter); 
		}
		MPI_Send(&myWordCounter[0], keywords.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
    MPI_Finalize();
   return(0);
}

