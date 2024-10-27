# five-word-alphabet
C Program to find all combinations of five 5-letter words with no repeating letters given a dictionary file, inspired by Wordle.  

(note: currently it requires files formatted with windows line endings \r\n to work)  

example usage:  
gcc -o out five-word.c -pthread  
wget https://raw.githubusercontent.com/tabatkins/wordle-list/refs/heads/main/words  
sed 's/$/\r/' words > words.txt  
./out 6 words.txt
