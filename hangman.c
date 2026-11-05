#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "hangman.h"

int get_word(char secret[]){
    FILE *fp = fopen(WORDLIST_FILENAME, "rb");
    if (fp == NULL){
        fprintf(stderr, "No such file or directory: %s\n", WORDLIST_FILENAME);
        return 1;
    }

    struct stat st;
    if (stat(WORDLIST_FILENAME, &st) != 0 || st.st_size <= 0){
        fprintf(stderr, "Unable to read word list: %s\n", WORDLIST_FILENAME);
        fclose(fp);
        return 1;
    }
    long int size = st.st_size;

    do{
        long int random = (rand() % size) + 1;
        if (fseek(fp, random, SEEK_SET) != 0){
            fprintf(stderr, "Unable to seek in word list: %s\n", WORDLIST_FILENAME);
            fclose(fp);
            return 1;
        }
        int result = fscanf(fp, "%*s %20s", secret);
        if (result != EOF)
            break;
    }while(1);

    fclose(fp);
    return 0;
}

int is_word_guessed(const char secret[], const char letters_guessed[]){
    for (int i = 0; secret[i] != '\0'; i++){
        int found = 0;
        for (int j = 0; letters_guessed[j] != '\0'; j++){
            if (secret[i] == letters_guessed[j]){
                found = 1;
                break;
            }
        }
        if (!found){
            return 0;
        }
    }

    return 1;
}

void get_guessed_word(const char secret[], const char letters_guessed[], char guessed_word[]){
    int i;

    for (i = 0; secret[i] != '\0'; i++){
        guessed_word[i] = '_';
        for (int j = 0; letters_guessed[j] != '\0'; j++){
            if (secret[i] == letters_guessed[j]){
                guessed_word[i] = secret[i];
                break;
            }
        }
    }
    guessed_word[i] = '\0';
}

void get_available_letters(const char letters_guessed[], char available_letters[]){
    int index = 0;

    for (char letter = 'a'; letter <= 'z'; letter++){
        int guessed = 0;
        for (int i = 0; letters_guessed[i] != '\0'; i++){
            if (letters_guessed[i] == letter){
                guessed = 1;
                break;
            }
        }
        if (!guessed){
            available_letters[index] = letter;
            index++;
        }
    }
    available_letters[index] = '\0';
}

static void print_guessed_word(const char guessed_word[]){
    for (size_t i = 0; guessed_word[i] != '\0'; i++){
        if (i > 0){
            putchar(' ');
        }
        putchar(guessed_word[i]);
    }
    putchar('\n');
}

void hangman(const char secret[]){
    char letters_guessed[27] = "";
    char guessed_word[strlen(secret) + 1];
    char available_letters[27];
    char input[256];
    int guesses_left = 8;

    printf("Welcome to the game, Hangman!\n");
    printf("I am thinking of a word that is %lu letters long.\n",
           (unsigned long)strlen(secret));

    while (guesses_left > 0 && !is_word_guessed(secret, letters_guessed)){
        size_t input_length;

        printf("-------------\n");
        printf("You have %d guesses left.\n", guesses_left);
        get_available_letters(letters_guessed, available_letters);
        printf("Available letters: %s\n", available_letters);
        printf("Please guess a letter: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL){
            return;
        }

        input_length = strcspn(input, "\r\n");
        if (input_length == sizeof(input) - 1){
            int character;
            while ((character = getchar()) != '\n' && character != EOF){
            }
        }
        input[input_length] = '\0';

        if (input_length > 1){
            int contains_non_letter = 0;
            for (size_t i = 0; i < input_length; i++){
                unsigned char character = (unsigned char)input[i];
                if (!((character >= 'a' && character <= 'z') ||
                      (character >= 'A' && character <= 'Z'))){
                    contains_non_letter = 1;
                    break;
                }
            }

            if (contains_non_letter){
                get_guessed_word(secret, letters_guessed, guessed_word);
                printf("Oops! Please enter letters only: ");
                print_guessed_word(guessed_word);
                continue;
            }

            int matches = strlen(secret) == input_length;
            for (size_t i = 0; matches && i < input_length; i++){
                if (tolower((unsigned char)input[i]) !=
                    tolower((unsigned char)secret[i])){
                    matches = 0;
                }
            }

            if (matches){
                printf("Congratulations, you won!\n");
            } else {
                printf("Sorry, bad guess. The word was %s.\n", secret);
            }
            return;
        }

        {
            unsigned char character = (unsigned char)input[0];
            char letter = (char)tolower(character);
            int already_guessed = 0;

            if (letter < 'a' || letter > 'z'){
                get_guessed_word(secret, letters_guessed, guessed_word);
                printf("Oops! '%s' is not a valid letter: ", input);
            } else {
                for (size_t i = 0; letters_guessed[i] != '\0'; i++){
                    if (letters_guessed[i] == letter){
                        already_guessed = 1;
                        break;
                    }
                }

                if (already_guessed){
                    get_guessed_word(secret, letters_guessed, guessed_word);
                    printf("Oops! You've already guessed that letter: ");
                } else {
                    size_t guessed_count = strlen(letters_guessed);
                    letters_guessed[guessed_count] = letter;
                    letters_guessed[guessed_count + 1] = '\0';
                    get_guessed_word(secret, letters_guessed, guessed_word);

                    if (strchr(secret, letter) != NULL){
                        printf("Good guess: ");
                    } else {
                        guesses_left--;
                        printf("Oops! That letter is not in my word: ");
                    }
                }
            }

            print_guessed_word(guessed_word);
        }
    }

    if (is_word_guessed(secret, letters_guessed)){
        printf("Congratulations, you won!\n");
    } else {
        printf("Sorry, you ran out of guesses. The word was %s.\n", secret);
    }
}
