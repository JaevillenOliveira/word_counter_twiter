#include<bits/stdc++.h>
#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <stdio.h>
#include <ctype.h>
#include <cstdlib>
#include <cctype>
#include <set>
#include <vector>
#include <locale.h>
#include <mpi.h>
#include <cstring>
#include <string>
#include <sstream> 

using namespace std;

std::set <string> readKeywords(char *fileName){
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
	for (std::set<string>::iterator it=keywords.begin(); it!=keywords.end(); ++it){
		if (*it == wordToFind)
			return index;
		index++;
	}
	return -1;
}

int handleTweet(FILE *tweetFile, std::set<string> keywords, std::vector <int> &wordCounter){
	char ch;
	int wordAt;
	string str;

	str.clear();
	ch = (char) fgetc(tweetFile);
	while(ch != EOF){
		if(!isascii(ch)){
			ch = removeAccentuation(ch, (char) fgetc(tweetFile));
		}
		if(ch != ' ' and ch != '\t' and ch != '\n' and isascii(ch) and ch != EOF){ 
			str = str + ch;
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
		ch = (char) fgetc(tweetFile);	
	}
	return 0;
}

void writeKeywordsCounter(std::set<string> keywords, std::vector <int> wordCounter, int wkPercent, int numP){
   	std::set <string>::iterator itKeywrds;
	char np[5], wlP[5], nOcur[10];
	char filename[200], lineToWrite[50];

	sprintf (np, "%d", numP);
	sprintf (wlP, "%d", wkPercent);

	strcpy(filename,"setup-results/");
	strcat(filename, np);
	strcat(filename, "processes/");
	strcat(filename, "problemSize");
	strcat(filename, wlP);
	strcat(filename, "/keywordsCounter.txt");

	FILE *fptr;
	fptr = fopen(filename,"w");
	fptr = freopen(filename,"a", fptr);

   	int index = 0;
	for (std::set<string>::iterator it = keywords.begin(); it != keywords.end(); ++it){
		sprintf (nOcur, "%d", wordCounter.at(index)); // cast the integer (Num of ocurrences of a word) to char *[]
		
		// mounts the phrase: <word>: <word count>
		strcpy(lineToWrite, (*it).c_str());
		strcat(lineToWrite, ": ");
		strcat(lineToWrite, nOcur);
		strcat(lineToWrite, "\n");
		
		//write the line to the file
		fputs(lineToWrite, fptr);
		fputs(lineToWrite, fptr);
		index++;
	}
   	fclose(fptr);
}

void manualReduction(std::vector <int> &out, std::vector <int> &in){
	std::transform (out.begin(), out.end(), in.begin(), out.begin(),std::plus<int>());
}

int main(int argc, char** argv){
    setlocale(LC_ALL, "Portuguese");

    std::set <string> keywords = readKeywords(argv[1]);
	std::vector <int> finalWordCounter (keywords.size()); 
    int wkPercent = atoi(argv[2]);//, numThreads = atoi(argv[4]);
	int numP, myRank, rc, receivedMsgs;
	MPI_Status status;
	// const string curDirectory = "/home/jaevillen/Ecomp/word_counter_twiter/tweets/";
	const string curDirectory = "/home/jaevillen/word-counter/tweets/";
	
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

		FILE *timeFile;
		timeFile = fopen("timelog.txt","a");

		char lineToWrite[50], timetkstr[20];
		sprintf (timetkstr, "%f", time_taken);

		strcpy(lineToWrite, "    Time: ");
		strcat(lineToWrite, timetkstr);
		
		//write the line to the file
		fputs(lineToWrite, timeFile);
		fclose(timeFile);
		writeKeywordsCounter(keywords, finalWordCounter, wkPercent, numP);
	}else {
		FILE *tweetFile, *tweetsNamesFile;
		std::vector <int> myWordCounter (keywords.size()); 

		int i;
		string temp = "";
		char tweetAlternatePath[200];
		char np[5], wlP[5], rk[5];
		sprintf (np, "%d", numP);
		sprintf (wlP, "%d", wkPercent);
		sprintf (rk, "%d", myRank);

		char filename[200];
		strcpy(filename,"setup-results/");
		strcat(filename, np);
		strcat(filename, "processes/");
		strcat(filename, "problemSize");
		strcat(filename, wlP);
		strcat(filename, "/fileNames4Rank");
		strcat(filename, rk);
		strcat(filename, ".txt");

		tweetsNamesFile = fopen(filename, "r"); 
		cout << "Running    " << filename << endl;

		char line[200];
		while (fgets(line, sizeof(line), tweetsNamesFile)) { 
			// remove the new line character
			for(i = 0; i < 200; i++){
				if(line[i] == '\n'){
					line[i] = '\0';
					i = 200;
				}
			}
			tweetFile = fopen(line, "r");
			if(!tweetFile){
				for(i = 0; i < sizeof(line); i++){
					if(line[i] == '/'){
						temp.clear();
					}else{
						if(line[i] == '\n'){
							break;
						}
						temp = temp + line[i];
					}
				}
				
				strcpy(tweetAlternatePath,curDirectory.c_str());
				strcat(tweetAlternatePath, temp.c_str());
				tweetFile = fopen(tweetAlternatePath, "r");
			};
			handleTweet(tweetFile, keywords, myWordCounter); 
			fclose(tweetFile);
		}
		fclose(tweetsNamesFile);
		MPI_Send(&myWordCounter[0], keywords.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
    MPI_Finalize();
   return(0);
}

