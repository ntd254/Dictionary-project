#include <stdio.h>
#include "soundex.h"
#include "btree/inc/btree.h"
#include <string.h>
#include <stdlib.h>

BTA *createDictionary(char name[])
{
    btinit();
    BTA *dictionary = btcrt(name, 0, 0);
    return dictionary;
}

void addWord(BTA *dictonary, BTA *soundexDictionary, char word[], char meaning[])
{
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    btins(dictonary, word, meaning, strlen(meaning) + 1);
    btins(soundexDictionary, soundexCode, word, strlen(word) + 1);
}

int findWordSameSoundex(BTA *soundexDictionary, char word[], char *sameSoundexWords[])
{
    int size;
    int length = 0;
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    char temp[] = "    ";
    char soundexWord[50];
    int flag = btsel(soundexDictionary, soundexCode, soundexWord, 50, &size);
    if (flag == 0)
    {
        sameSoundexWords[length++] = strdup(soundexWord);
        flag = btseln(soundexDictionary, temp, soundexWord, 50, &size);
        while (strcmp(soundexCode, temp) == 0 && flag == 0)
        {
            sameSoundexWords[length++] = strdup(soundexWord);
            flag = btseln(soundexDictionary, temp, soundexWord, 50, &size);
        }
    }
    return length;
}

int findMeaning(BTA *dictionary, BTA *soundexDictionary, char word[], char meaning[], int *length, char *sameSoundexWords[])
{
    int size;
    int flag = btsel(dictionary, word, meaning, 50, &size);
    if (flag != 0)
    {
        *length = findWordSameSoundex(soundexDictionary, word, sameSoundexWords);
    }
    return flag;
}

void printDictionary(BTA *dictionary)
{
    char *meaning = (char *)malloc(50);
    char *word = (char *)malloc(50);
    int size;
    int flag = btpos(dictionary, 1);
    while (flag == 0)
    {
        flag = btseln(dictionary, word, meaning, 50, &size);
        if (!flag)
            printf("%s: %s\n", word, meaning);
    }
}

int main()
{
    BTA *dictionary = createDictionary("dictionary");
    BTA *soundexDictionary = createDictionary("soundexDictionary");
    btdups(soundexDictionary, 1);
    addWord(dictionary, soundexDictionary, "taylor", "dat");
    addWord(dictionary, soundexDictionary, "tailor", "ky");
    addWord(dictionary, soundexDictionary, "tailer", "tuan");
    char meaning[50];
    char *sameSoundexWords[100];
    int length = 0;
    int flag = findMeaning(dictionary, soundexDictionary, "abc", meaning, &length, sameSoundexWords);
    printf("%d\n", length);
    if (length == 0 && flag == 0)
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
    printDictionary(soundexDictionary);
    printf("\n");
    printDictionary(dictionary);
}