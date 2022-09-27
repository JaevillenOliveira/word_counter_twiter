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

void writeKeywordsCounter(std::set<string> keywords, std::vector <int> wordCounter, int numFiles, int numThreads){
   std::set <string>::iterator itKeywrds;

   	ofstream MyFile("results/"+std::to_string(numThreads)+"threads/"+std::to_string(numFiles)+"files/keywordsCounter.txt");
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

int main(int argc, char** argv){
    setlocale(LC_ALL, "Portuguese");

	const int maxArraySize = 250000;
    std::string dirPath(argv[1]); // get the name of the folder containing the tweets (the folder must be on the same directory as the code)
    std::string keywrdsFileName(argv[2]); // get the name of the file containing the keywords (the file must be on the same directory as the code)
    std::set <string> keywords = readKeywords(keywrdsFileName);
	std::vector <int> finalWordCounter (keywords.size()); 
    int numFiles = atoi(argv[3]);
	int numThreads = atoi(argv[4]);

    DIR *dr;
    struct dirent *en;
    dr = opendir(argv[1]); //open directory

	#pragma omp declare reduction (sumvector : std::vector<int> : manualMerge(omp_out, omp_in)) initializer(omp_priv(omp_orig))
    
	if (dr) {
    	int fileCounter = 0, chunkSize;
    	while (fileCounter < numFiles) {
    		if((numFiles - fileCounter) > maxArraySize)
    			chunkSize = maxArraySize;
			else
				chunkSize = (numFiles - fileCounter);
				
			string filePaths [chunkSize];
			
			int count = 0;
			while(count < chunkSize){
				en = readdir(dr);
				if (en == NULL){
					chunkSize = count;
					numFiles = fileCounter + chunkSize;
					break;
				}
				filePaths[count] = dirPath + '/' + en->d_name;
				count++;
			}
			fileCounter += chunkSize;

			std::vector <int> tempWordCounter (keywords.size()); 
			#pragma omp parallel for reduction(sumvector:tempWordCounter)
			for (count = 0; count < chunkSize; count++) {
				cout << numThreads << " Threads - File " << count << endl;
				handleTweet(filePaths[count], keywords, tempWordCounter); 
			} 
			manualMerge(finalWordCounter, tempWordCounter);
    	}
    	
        closedir(dr); 
        writeKeywordsCounter(keywords, finalWordCounter, numFiles, numThreads);
    }
   return(0);
}

