# word_counter_twiter

## The problem

Acquire a dataset formed by tweets from brazilian users in the recent weeks, count the words in those tweets and get the frequency of ocurrence of some key words.

## Project code content
There are three code files. 

1 - The script-download-db.py contains the query to get the most recent tweets from Brazil.

2 - The serial-word-counter.cpp is a serial code to count all the words from the tweets, obeying the following rules:
- Uma palavra é uma sequência de duas ou mais letras contíguas;
- Letras isoladas devem ser desconsideradas;
- Conjunções, artigos e preposições devem ser desconsiderados;
- Letras acentuadas devem ser transformadas em suas equivalentes sem acento;
- Apóstrofos, hifens e outros caracteres especiais devem ser eliminados;
- Caracteres não ASCII devem ser desconsiderados;
- As letras maiúsculas são consideradas equivalentes às suas contrapartes minúsculas, sugere-se converte-las minúsculas;

3 - The parallel-word-counter is the parallel version of the same code

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

After the time command is called the executable.

After that, separated by whitespaces, are the parameters: the name of the folder containing the tweet files, the name of the nonwords file, the name of keywords file and the number of tweets to be processed (2092273 is the total number of files).

    (time ./serial-word-counter {name-of-tweets-folder} {name-of-keywords-file} {num-of-files-to-handle} {num-threads}) > output.txt 2>> timelog.txt
    (time ./serial-word-counter tweets nonwords.txt keywords.txt 2092273 4) > output.txt 2>> timelog.txt

## To build the parallel code
	g++ -fopenmp parallel-word-counter.cpp -o parallel-word-counter.out
	
## To execute the parallel code
	(time ./parallel-word-counter.out tweets nonwords.txt keywords.txt 500000) > output.txt 2>> timelog.txt

    
## The serial code logic

The main data structures used are:
- a std::set <string> to store all the nonwords
- a std::set <string> to store all the keywords
- a std::map <string, int> to store all the words found on the tweets and its counts

The main logic starts in a for loop that goes through the tweet files until the informed limit is reached or until it reaches the end of the folder.

In each iteration is called the 'handleTweet' function that receives the path of a tweet file, a pointer to the map structure containing the words and its counts, and the set structure containing the nonwords.

The 'handleTweet' function will open the file and read char by char, that will be put together on a string, until it finds a whitespace, a tab, a breakline, a non ascii or the end of the file. When one of those symbols are found the string built from all the previous chars will be checked to see if is a valid word.

First the function 'isValidWord' will be called to verify if the string's size > 2, if it starts with '#' or '@' (in the first case the word is a hashtag, so it's not valid; in the second case is a mention, also not valid). It is alse checked if the string starts with 'http', in which case the word is a link, and not valid.

After that check the word is searched in the nonword structure. If it is not found there it will be searched on the structure containing all words. if it is foud there the count is incremented by 1, if not, the word is inserted on the structure with the count equals 1. That is the end of the 'handleTweet' function.

At the end of the main for loop all the words are written on the allWordsConter.txt file with its count numbers and the the function 'writeKeywordsCounter' is called.

The writeKeywordsCounter function search for words on the allwords structure that match with the keywords from the keywords.txt file. The words that match are witten on the keywordsCounter.txt file with their count numbers.
	
## The parallel code logic

The code was parallelized by data. Which means that with multiple threads available, each thread will execute the 'handleTweet' function for diferent files. To achieve this the '#pragma omp parallel for' directive was used on the loop the goes through the files.
	
In this scenario it was made some adjustments to avoid data conflicts. In the serial version a pointer to the folder of tweets was used to read the files names one at a time, dinamically, so no information about it is stored on memory. But in the parallel case there's a conflict when the threads try to get the address of the next file to be accessed, which makes the solution not viable.
	
To solve this problem, on the parallel logic the names of the files are read beforehand, in a serial way, and stored in an array. This way, as the indexes of the for loop are uniquely assigned to each thread, it is assured the each thread will access diferent files and there is no conflict. 
	
But in this case there is a limitation about the size of the array storing the names of the files. Because there is a big amount of files (2092273 to be exact) it's not possible to store all the names in memory on execution time. To solve this it was assigned, in code, a chunk size (250000) for the amount of files handled at a time. So there is a loop in which are read the name of the next 250000 files which then are handled, in paralel. The loop goes on until the number of files informed by the user is reached or until the end of the folder.
	
Another conflict was detected on the reading and updating of the map structure that stores all the words and its count numbers. To solve that the '#pragma omp critical' directive was used on that part of the code.
	





