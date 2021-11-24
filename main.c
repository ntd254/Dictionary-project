// gcc main.c soundex.c -o main -lbt -Lbtree/lib -Ibtree/inc

#include <stdio.h>
#include "soundex.h"
#include "btree/inc/btree.h"
#include <string.h>
#include <stdlib.h>

#define maxSizeWord 200

BTA *createDictionary(char name[])
{
    btinit();
    BTA *dictionary = btcrt(name, 0, 0);
    return dictionary;
}

void addWord(BTA *dictionary, BTA *soundexDictionary, char word[], char meaning[])
{
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    btins(dictionary, word, meaning, strlen(meaning) + 1);
    btins(soundexDictionary, soundexCode, word, strlen(word) + 1);
}

void readFile(BTA *dictionary, BTA *soundexDictionary, char fileName[])
{
    FILE *file_dict = fopen(fileName, "r");
    char word[maxSizeWord];
    char meaning[maxSizeWord];
    char temp[maxSizeWord];
    int flag = 1;
    while (!feof(file_dict))
    {
        fgets(temp, maxSizeWord, file_dict);
        if (temp[0] == '@')
        {
            temp[strlen(temp) - 3] = '\0';
            strcpy(word, &temp[1]);
            flag = 1;
        }
        if (temp[0] == '-' && flag == 1)
        {
            temp[strlen(temp) - 1] = '\0';
            strcpy(meaning, &temp[2]);
            flag = 0;
            addWord(dictionary, soundexDictionary, word, meaning);
        }
    }
}

int findWordSameSoundex(BTA *soundexDictionary, char word[], char *sameSoundexWords[])
{
    int size;
    int length = 0;
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    char temp[] = "    ";
    char soundexWord[maxSizeWord];
    int flag = btsel(soundexDictionary, soundexCode, soundexWord, maxSizeWord, &size);
    if (flag == 0)
    {
        sameSoundexWords[length++] = strdup(soundexWord);
        flag = btseln(soundexDictionary, temp, soundexWord, maxSizeWord, &size);
        while (strcmp(soundexCode, temp) == 0 && flag == 0)
        {
            sameSoundexWords[length++] = strdup(soundexWord);
            flag = btseln(soundexDictionary, temp, soundexWord, maxSizeWord, &size);
        }
    }
    return length;
}

int findMeaning(BTA *dictionary, BTA *soundexDictionary, char word[], char meaning[], int *length, char *sameSoundexWords[])
{
    int size;
    int flag = btsel(dictionary, word, meaning, 200, &size);
    if (flag != 0)
    {
        *length = findWordSameSoundex(soundexDictionary, word, sameSoundexWords);
    }
    return flag;
}

void printDictionary(BTA *dictionary)
{
    char *meaning = (char *)malloc(maxSizeWord);
    char *word = (char *)malloc(maxSizeWord);
    int size;
    int flag = btpos(dictionary, 1);
    while (flag == 0)
    {
        flag = btseln(dictionary, word, meaning, maxSizeWord, &size);
        if (!flag)
            printf("%s: %s\n", word, meaning);
    }
    free(meaning);
    free(word);
}

int main()
{
    BTA *dictionary = createDictionary("dictionary");
    BTA *soundexDictionary = createDictionary("soundexDictionary");
    btdups(soundexDictionary, 1);
    readFile(dictionary, soundexDictionary, "dict.txt");
    addWord(dictionary, soundexDictionary, "taylor", "dat");
    addWord(dictionary, soundexDictionary, "tailer", "tuan");
    char meaning[maxSizeWord];
    char *sameSoundexWords[100];
    int length = 0;
    int flag = findMeaning(dictionary, soundexDictionary, "taylor", meaning, &length, sameSoundexWords);
    if (flag == 0)
    {
        printf("%s", meaning);
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            printf("%s ", sameSoundexWords[i]);
        }
    }
    printf("\n");
    // printDictionary(soundexDictionary);
    // printf("\n");
    // printDictionary(dictionary);
}