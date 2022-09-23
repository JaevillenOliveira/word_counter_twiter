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

bool isValidWord(string word){
   if(word.size() <= 2)
      return false;
   if(word[0] == '#' or word[0] == '@')
      return false;
   if(word.find("http") != std::string::npos)
      return false;
   return true;
}

bool isNotAlpha(char c) {
   return !isalpha(c);
}

void clearWord(string &word){
   transform(word.begin(), word.end(), word.begin(), ::tolower);
   word.erase(remove_if(word.begin(), word.end(), isNotAlpha), word.end());
   
}

void handleTweet(string fileName, std::map<string,int> &words, std::set<string> nonWords){
	//Creating an input stream to read a file
	ifstream ifstream_ob;
	ifstream_ob.open(fileName, ios::in);

	char ch;
   std::string str;
   std::map <string,int>::iterator itMap;
   std::set <string>::iterator itSet;

   str.clear();
	while(ifstream_ob)
	{
		ch = ifstream_ob.get(); 
      if(ch != ' ' and ch != '\t' and ch != '\n' and isascii(ch) and ch != EOF){ 
         str += ch;
      }else{
         if(isValidWord(str)){
            clearWord(str);
            itSet = nonWords.find(str);
            if(itSet == nonWords.end()){ // if the string doesn't belongs to the non-words list
            	#pragma omp critical
            	{
				   itMap = words.find(str);
				   if (itMap != words.end()){
				      itMap->second += 1;
				   }else
				      words[str] = 1;
            	}
            }
         }
         str.clear();
      }
	}
	ifstream_ob.close();

}

void writeAllWordsCounter(std::map <string,int> words){
   ofstream MyFile("allWordsCounter.txt");
   for (std::map <string,int>::iterator it = words.begin(); it != words.end(); ++it){
      MyFile << it->first << ": " << it->second << endl;
   }
   MyFile.close();
}

void writeKeywordsCounter(std::map <string,int> words, string keywordsFilePath){
   std::set <string> keywords = readKeywords(keywordsFilePath);
   std::set <string>::iterator itKeywrds;

   ofstream MyFile("keywordsCounter.txt");
   for (std::map <string,int>::iterator it = words.begin(); it != words.end(); ++it){
      itKeywrds = keywords.find(it->first);
      if(itKeywrds != keywords.end())
         MyFile << it->first << ": " << it->second << endl;
   }
   MyFile.close();
}

int main(int argc, char** argv){
    // ########### sequential part start

	const int maxArraySize = 250000;
    std::string dirPath(argv[1]); // get the name of the folder containing the tweets (the folder must be on the same directory as the code)
    std::string nonwordsFileName(argv[2]); // get the name of the file containing the nonwords (the file must be on the same directory as the code)
    std::set <string> nonWords = readKeywords(nonwordsFileName); // read the nonwords file
    std::string keywrdsFileName(argv[3]); // get the name of the file containing the keywords (the file must be on the same directory as the code)
    int numFiles = atoi(argv[4]);
    std::map <string,int> words; // structure to store all the words counted
    DIR *dr;
    struct dirent *en;
    dr = opendir(argv[1]); //open directory
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
			#pragma omp parallel for
			for (count = 0; count < chunkSize; count++) {
				cout << filePaths[count] << endl;
				handleTweet(filePaths[count], words, nonWords); 
			} 
    	
    	}
    	
        cout << "nfksjdhfkwgbseakjbgWFVJHWEFuyw GFEWHFGYWUEfguw yv " << endl;
        closedir(dr); 
        writeAllWordsCounter(words);
        writeKeywordsCounter(words, keywrdsFileName);

        ofstream MyFile("timelog.txt", std::ios_base::app);
        MyFile << '\n' << "##############################" << '\n' << "Handling "+ to_string(numFiles) + " files" << endl;
        MyFile.close();

    }
   return(0);
}

