# word_counter_twiter

## Input and Output
All input data must be inside the folder containing the executable file. All the outuput data will be placed in files inside the same folder.

### The Input 
- Folder containing tweet files. Each file (.txt) contains one tweet; 
- A .txt file containing words to be ignored (here is the nonwords.txt); 
- A .txt file containing the keywords to be shown at the end (here is the keywords.txt).

### The output
- allWordsCounter.txt, containing all the words and its count numbers;
- keywordsCounter.txt, containig the words matched with the keywords.txt file and its count numbers;
- timelog.txt store the log of execution time. At the end of each execution the time will be appended to this file.

    
## To build the serial code
    make serial-word-counter

## To execute the serial code
The code is executed using the command 'time' preappended. This is a Linux command to log wall, user and sys time.

## To build the parallel code
	g++ -fopenmp parallel-word-counter.cpp -o parallel-word-counter.out
	
## To execute the parallel code
	(time ./parallel-word-counter.out tweets/tweets nonwords.txt keywords.txt 500000) > output.txt 2>> timelog.txt

After the time command is called the executable.

After that, separated by whitespaces, are the parameters: the name of the folder containing the tweet files, the name of the nonwords file, the name of keywords file and the number of tweets to be processed (2092273 is the total number of files).

    (time ./serial-word-counter {name-of-tweets-folder} {name-of-nonwords-file} {name-of-keywords-file} {num-of-files-to-handle}) > output.txt 2>> timelog.txt
    (time ./serial-word-counter tweets nonwords.txt keywords.txt 2092273) > output.txt 2>> timelog.txt
    
## The logic explained

The main data structures used are:
- a set <string> to store all the nonwords
- a set <string> to store all the keywords
- a map <string, int> to store all the words found on the tweets and its counts

The main logic starts in a for loop that goes through the tweet files until the informed limit is reached or until it reaches the end of the folder.

In each iteration is called the 'handleTweet' function that receives the path of a tweet file, a pointer to the map structure containing the words and its counts, and the set structure containing the nonwords.

The 'handleTweet' function will open the file and read char by char, that will be put together on a string, until it finds a whitespace, a tab, a breakline, a non ascii or the end of the file. When one of those symbols are found the string built from all the previous chars will be checked to see if is a valid word.

First the function 'isValidWord' will be called to verify if the string's size > 2, if it starts with '#' or '@' (in the first case the word is a hashtag, so it's not valid; in the second case is a mention, also not valid). It is alse checked if the string starts with 'http', in which case the word is a link, and not valid.

After that check the word is searched in the nonword structure. If it is not found there it will be searched on the structure containing all words. if it is foud there the count is incremented by 1, if not, the word is inserted on the structure with the count equals 1.

At the end of the main for loop all the words are written on the allWordsConter.txt file with its count numbers and the the function 'writeKeywordsCounter' is called.

The writeKeywordsCounter function search for words on the allwords structure that match with the keywords from the keywords.txt file. The words that match are witten on the keywordsCounter.txt file with their count numbers.

 




