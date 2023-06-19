//
//  wordle_solver.c
//  
//
//  Created by jen on 5/28/23.
//

#include "wordle_solver.h"
#include "dictionary.h"
#include <stdio.h>
#include <ctype.h>
#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(  __VA_ARGS__ )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif


#define ALPHABET 26
#define WORDLE_WORD_SIZE 5


enum LetterState {
    LetterState_Unknown = 0,
    LetterState_No,
    LetterState_Yes,
};

struct  HintState {
    enum LetterState letter;
    uint8_t yellow_position; // use the n bits to denote position. bit 0 is first postition.
    uint8_t green_position; // use the n bits to denote position. bit 0 is first postition.

};

int UserInputGuess( char* buffer, char* hints) ;
int UpdateLetterState(char* guess, char*  hint_string, struct HintState* letters_list);
void PrintLetterState(struct HintState* letters_list);
int SearchDictionary(char** directionary, bool* remaining_words, struct HintState* letters_list, int dictionary_size);
bool  RuleMatchWord(char* word, char letter, struct HintState* letter_status);
void PrintWord(char* word, struct HintState* letters_list);
void PrintRemainingWords(char** directionary, bool* remaining_words, struct HintState* letters_list, int dictionary_size) ;
void WordleInit(bool* remaining_words, struct HintState* letters_list, int dictionary_size) ;

int main(int argc, char *argv[]) {

    int dictionary_length = sizeof(word_dictionary)/sizeof(char *);
    bool remaining_words[DICTIONARY_LENGTH];
    printf("words in dictionary_length are %d\n", dictionary_length);

    char guess_string[20] = "\0";
    char hint_string[20] = "\0";
    struct HintState letter_array[ALPHABET];

    WordleInit(remaining_words, letter_array, dictionary_length) ;

    for (int turn = 1; turn < 6; turn++) {

        // Take in last guess.
        if (!UserInputGuess(guess_string , hint_string)) {
            printf("Oh you found your word!!! Yay\n");
            break;
        }
        guess_string[WORDLE_WORD_SIZE] = '\0';
        hint_string[WORDLE_WORD_SIZE] = '\0';

        // Print summary of reamining letter options
        UpdateLetterState(guess_string, hint_string, letter_array);
        PrintLetterState(letter_array);

        printf("Trimming word list\n");
        // Provide list of all remaining options after trimming.
        int count = SearchDictionary(word_dictionary, remaining_words, letter_array, dictionary_length);
        printf("REMAINING WORDS ARE %d\n\n", count);
        PrintRemainingWords(word_dictionary, remaining_words, letter_array, dictionary_length);
        if (count <= 1) {
            break;
        }
    }

    return 0;
}

void WordleInit(bool* remaining_words, struct HintState* letters_list, int dictionary_size) {

    for(int i = 0; i < dictionary_size; i++) {
        remaining_words[i] = true;
    }

    for(int i = 0; i < ALPHABET; i++) {
        letters_list[i].letter = LetterState_Unknown;
        letters_list[i].green_position = 0;
        letters_list[i].yellow_position = 0;
    }

}


int UserInputGuess( char* buffer, char* hints) {

    //empty out
    //scanf( "%s", buffer); // Bah bug
    printf("What is last guess (no more than 5 letters? ");
    scanf( "%5s", buffer);
    char * p = buffer;
    for ( ; *p; ++p) *p = (char)tolower(*p);
    printf("guess is: %s size: %lu\n\n", buffer, strlen(buffer));

    // now get yellows and green data.
    printf("What is were the color clues from last guess (*-none, (Y)ellow, (G)reen)?");
    scanf( "%5s", hints);
    p = hints;
    for ( ; *p; ++p) *p = (char)tolower(*p);
    printf("hints back are: %s size: %lu\n\n", hints, strlen(hints));
    if (strcmp(hints, "ggggg"))
        return 1;
    return 0;
}


int UpdateLetterState(char* guess, char*  hint_string, struct HintState* letters_list) {

    int index = 0;
    while(guess[index] != '\0') {

        char c = guess[index];
        int letter_index = (int) c - 'a';

        switch (hint_string[index]) {
        case '*':
            letters_list[letter_index].letter = LetterState_No;
            break;
        case 'y':
            letters_list[letter_index].letter = LetterState_Yes;
            letters_list[letter_index].yellow_position |=  (1 << index);
            break;
        case 'g':
            letters_list[letter_index].letter = LetterState_Yes;
            letters_list[letter_index].green_position |=  (1 << index);
            break;
        }
        DEBUG_PRINT("Updated letter list %c %c %x\n", c, letter_index + 'a', letters_list[letter_index].letter);
        index++;
    }
    return 0;
}


void PrintLetterState(struct HintState* letters_list) {
    for (int i = 0; i < ALPHABET; i++) {
        char letter = (char)( i) + 'a';

        switch (letters_list[i].letter) {
        case LetterState_Unknown:
            printf(ANSI_COLOR_BLUE     "%c\n"     ANSI_COLOR_RESET, letter);
            break;
        case LetterState_No:
            printf(ANSI_COLOR_RED     "%c\n"     ANSI_COLOR_RESET, letter);
            break;
        case LetterState_Yes:
            // Check on state
            if (letters_list[i].green_position) {
            printf(ANSI_COLOR_GREEN     "%c %x\n"     ANSI_COLOR_RESET, letter, letters_list[i].green_position);
            }
            else if (letters_list[i].yellow_position) {
            printf(ANSI_COLOR_YELLOW     "%c %x\n"     ANSI_COLOR_RESET, letter, letters_list[i].yellow_position);
            }
            break;
        }
    }
    printf( "\n" );
}

int SearchDictionary(char** directionary, bool* remaining_words, struct HintState* letters_list, int dictionary_size) {
    //ASSERT(directionary != 0);
    //ASSERT(remaining_words  != 0);
    int count = 0;
    for (int word = 0; word < dictionary_size; word++) {

        for (int letter_index = 0; letter_index < ALPHABET; letter_index++) {
            char letter = (char)( letter_index) + 'a';
            struct HintState* letter_status = &letters_list[letter_index]; 
            DEBUG_PRINT("Before word matching %s %c %x\n", directionary[word], letter, letter_status->letter);
            if (letter_status->letter == LetterState_Unknown) {
                continue;
            }
            // see if in the word

            // see if letter is in an ruled out  position.
            if (!RuleMatchWord(directionary[word], letter, letter_status)) {
                remaining_words[word] = false;
                
                count++;
                DEBUG_PRINT("Removed %s count %d\n", directionary[word], count);
                break;
            } else DEBUG_PRINT("Keeping %s %c\n", directionary[word], letter);

        }
    }
    return DICTIONARY_LENGTH - count -1;
}


void PrintRemainingWords(char** directionary, bool* remaining_words, struct HintState* letters_list, int dictionary_size) {
    //ASSERT(directionary != 0);
    //ASSERT(remaining_words  != 0);
    printf("Printing remaining word list %d\n", dictionary_size);
    int count = 0;
    for (int i = 0; i < dictionary_size; i++) {
        if (remaining_words[i]) {
            printf("[%d]", ++count);
            PrintWord(directionary[i], letters_list);
            printf("\n");
        }

    }
    printf("Total words remaining %d\n", count);
    if (count == 1) {
        printf("Found solution\n");
    }
}

void PrintWord(char* word, struct HintState* letters_list) {
   // ASSERT(word != 0);
    //ASSERT(letters_list  != 0);
    uint8_t letter_mask = 0;
    uint8_t score = 0;
    DEBUG_PRINT("PrintWord %s ", word);
    for (int i = 0; i < WORDLE_WORD_SIZE; i++) {
        char c = word[i];
        int letter_index = (int) c - 'a';
        DEBUG_PRINT("%s [%d] = %c\n", word, i, c);
        switch (letters_list[letter_index].letter) {
        case LetterState_Unknown:
            printf(ANSI_COLOR_BLUE     "%c"     ANSI_COLOR_RESET, c);
            break;
        case LetterState_No: // SHouldn't happen
            printf(ANSI_COLOR_RED     "%c"     ANSI_COLOR_RESET, c);
            break;
        case LetterState_Yes:
                // Check on state

            letter_mask = (1 << i);

            if (letters_list[letter_index].green_position & letter_mask) {
                printf(ANSI_COLOR_GREEN     "%c"     ANSI_COLOR_RESET, c);
                score += 2;
            }
            else if (letters_list[letter_index].yellow_position & letter_mask) { // Don't need it but want it.
                printf(ANSI_COLOR_YELLOW     "%c"     ANSI_COLOR_RESET, c);
                score += 1;
            } 
            else if (letters_list[letter_index].yellow_position ) {
                printf(ANSI_COLOR_YELLOW     "%c"     ANSI_COLOR_RESET, c);
                score += 1;
            }   
            else   if (!(letters_list[letter_index].green_position & letter_mask)) {
                printf(ANSI_COLOR_YELLOW     "%c"     ANSI_COLOR_RESET, c);
                score += 1;
            }
            else printf("NO COLOUR %c %x %x %x", c, letter_mask, letters_list[letter_index].green_position, letters_list[letter_index].yellow_position);
            break;
        }

    }
    printf(" %d", score);

}    



// True if keeping, false is removing.
bool  RuleMatchWord(char* word, char letter, struct HintState* letter_status) {

    char *ch_ptr = word;
    uint8_t mask = 0;
    DEBUG_PRINT("RuleMatchWord %s %c %d\n", word, letter, letter_status->letter);
    while(ch_ptr) {
        ch_ptr = strchr(ch_ptr, letter);
        //DEBUG_PRINT("RuleMatchWord strchr  %p\n", ch_ptr);
        if (!ch_ptr) {
            DEBUG_PRINT("RuleMatchWord break\n");
            break;
        }
        int index = ch_ptr - word;
        mask |= (1 << index);
        ch_ptr++;
        DEBUG_PRINT("RuleMatchWord iterate %p %c %x\n", ch_ptr, *ch_ptr, mask);
      }

    DEBUG_PRINT("RuleMatchWord Pre compare  %c %s %x %x\n", letter,  word, mask, letter_status->green_position);
    if (letter_status->letter == LetterState_Unknown) {
        DEBUG_PRINT("RuleMatchWord unknown  %c %s %x\n", letter,  word, mask);
        return true; 
    }

    if (!mask && (letter_status->letter == LetterState_Yes)) {
        return false; // remove it
    }

    if (letter_status->letter == LetterState_No) {
        DEBUG_PRINT("RuleMatchWord letter is no  %c %s status %x %x\n", letter, word, letter_status->letter, mask);
    }
    if ((mask) && (letter_status->letter == LetterState_No)) {
        DEBUG_PRINT("RuleMatchWord letter is no   %c %s\n", letter, word);
        return false;  // remove word
    } 

    if (mask & letter_status->green_position) {
        // This is a good word. KEEPIT!
        DEBUG_PRINT("RuleMatchWord Keep green  %c %s %x %x\n", letter,  word, mask, letter_status->green_position);
        return true;
    }
    if (!(mask & letter_status->yellow_position) && (!letter_status->green_position)) {
        // This is a good word. KEEPIT!
        DEBUG_PRINT("RuleMatchWord yellow  %c %s %x %x\n", letter,  word, mask, letter_status->yellow_position);
        return true;
    }
    DEBUG_PRINT("RuleMatchWord nothing more so remove?  %s\n", word);
    return false;
}

