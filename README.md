# word_counter_twiter

## Input and Output
    All data must be inside the folder containing the executable file
    For input there is a folder containing tweets files, which are .txt files; a .txt file containing words to be ignored (here is the nonwords.txt); and a .txt file containing the keywords to be shown at the end (here is the keywords.txt).

    The output will be the files 
        - allWordsCounter.txt, containing all the words and its count numbers;
        - keywordsCounter.txt, containig the words matched with the keywords.txt file and its count numbers;
        - timelog.txt store the log of execution time for all executions

    
## To build the serial code
    make serial-word-counter

## To execute the serial code
    time ./serial-word-counter {name-of-tweets-folder} {name-of-nonwords-file} {name-of-keywords-file} {num-of-files-to-handle}
    time ./serial-word-counter tweets nonwords.txt keywords.txt 2092273
