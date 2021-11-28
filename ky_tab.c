// for compiling
// gcc main.c soundex.c -o main -Lbtree/lib -lbt -Ibtree/inc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soundex.h"
#include "btree.h"

#define maxSizeWord 100
// prototype of functions
BTA* crt_dict(char name[]);
void load_file(BTA* dictionary, BTA* soundexDictionary, char filename[]); // load from short_dict.txt
void printDictionary(BTA* dictionary);
//tasks
void addWord(BTA* dictionary, BTA* soundexDictionary, char word[], char meaning[]);
void search(BTA* dictionary, char word[], char meaning[]);
void delete(BTA* dictionary, BTA* soundexDictionary, char word[]);
int same_soundex(BTA* soundexDictionary, char word[], char* sameSoundexWords[]); // return number of sameSoundexWords soundex words
// using same_soundex before using tab
// to find the words with same soundex for easy complete
void tab(BTA* dictionary);// press \t and then \n to complete the word (but in another line)
int tab_list(BTA* dictionary, char str[], char* same[]);
// main function
int main(){
    BTA* dictionary = crt_dict("dictionary");
    BTA* soundexDictionary = crt_dict("soundexDictionary");
    btdups(soundexDictionary,1);
    int length = 0;
    char word[20];
    char meaning[maxSizeWord];
    char* sameSoundexWords[10];
    load_file(dictionary, soundexDictionary, "test.txt");

    tab(dictionary);
    // else
    // printf("finish!\n");
    printDictionary(dictionary);
    return 0;
}
// init funtions
BTA* crt_dict(char name[]){
    btinit();
    BTA *dictionary = btcrt(name, 0, 0);
    return dictionary;
}
void load_file(BTA *dictionary, BTA *soundexDictionary, char fileName[])
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
            temp[strlen(temp) - 2] = '\0';
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
    fclose(file_dict);
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
            printf("%s : %s\n", word, meaning);
    }
    free(meaning);
    free(word);
}
// 1st feature
void addWord(BTA* dictionary, BTA* soundexDictionary, char word[], char meaning[]){
    char soundexCode[] = "    ";// 4 spaces
    soundex(word, soundexCode);
    btins(dictionary, word, meaning, strlen(meaning) + 1);
    btins(soundexDictionary, soundexCode, word, strlen(word) + 1);
}
void search(BTA *dictionary, char word[], char meaning[])
{
    int size;
    int flag = btsel(dictionary, word, meaning, maxSizeWord, &size);
    if (flag != 0){
        printf("not found!\n");
    }else{
        printf("%s\n",meaning);
    }
}
void delete(BTA* dictionary, BTA* soundexDictionary, char word[]){
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    int sig1 = btdel(dictionary, word);
    int sig2 = btdel(soundexDictionary, soundexCode);
    if(sig1 == 0){
        printf("successful deleting in dict\n");
    }else printf("error 1!\n");
    if(sig2 == 0){
        printf("successful deleting in soundex dict\n");
    }else printf("error 2!\n");
}
//2nd feature
int same_soundex(BTA *soundexDictionary, char word[], char *sameSoundexWords[]){
    int size;
    int length = 0;
    char soundexCode[] = "    ";// 4 spaces
    soundex(word, soundexCode);
    char temp[] = "    ";//4 spaces
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
// 3rd feature
void tab(BTA* dictionary){
    char str[10];
    char* same[20];
    char c;
    int j = 0;
    do{
        c = getchar();
        if(c != '\t'){
            strncat(str,&c,1);
            j++;
        }
    }while((c != ' ') && (c != '\t'));

    int l = tab_list(dictionary, str, same);
    if(l != 0){
        for(int i = 0; i < l;i ++){
            if(strncmp(str,same[i],j) == 0){
                printf("%s  ",same[i]); //break;
            }
        }printf("\n");
    }
    else{
        printf("not found!\n");
    } 
}
int tab_list(BTA* dictionary, char str[], char* same[]){
    int total = 0;
    int value;
    char key[50];
    int flag  = bfndky(dictionary, str, &value);
    if(flag == 0){
        same[total++] = strdup(str);
    }
    while(1){
        flag  = bnxtky(dictionary, key, &value);
        if(flag != 0  || strncmp(key, str, strlen(str)) != 0){
            break;
        }else{
            same[total++] = strdup(key);
            if(total >= 10) break;
        }
    }
    return total;
}