// for compiling
// gcc main.c soundex.c -o main -Lbtree/lib -lbt -Ibtree/inc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "soundex.h"
#include "btree.h"

#define maxSizeWord 100
// prototype of functions
BTA* crt_dict(char name[]);
void load_file(BTA* dictionary, BTA* soundexDictionary, char filename[]); // load from short_dict.txt
void printDictionary(BTA* dictionary);
void menu(){
    printf("------------DICTIONARY------------\n");
    printf("0 - print out the dictionary\n");
    printf("1 - add word\n");
    printf("2 - search meaning\n");
    printf("3 - delete word\n");
    printf("4 - auto complete \n");
    printf("5 - exit\n");
}
//tasks
void input(char word[]);
void addWord(BTA* dictionary, BTA* soundexDictionary, char word[], char meaning[]);
void search(BTA* dictionary, BTA* soundexDictionary, char word[], char meaning[]);
void delete(BTA* dictionary, BTA* soundexDictionary, char word[]);
int same_soundex(BTA* soundexDictionary, char word[], char* sameSoundexWords[]); // return number of sameSoundexWords soundex words
// using same_soundex before using tab
// to find the words with same soundex for easy complete
void tab(BTA* dictionary);// press \t and then \n to complete the word (but in another line)
int tab_list(BTA* dictionary, char str[], char same[]);
// main function
int main(){
    BTA* dictionary = crt_dict("dictionary");
    BTA* soundexDictionary = crt_dict("soundexDictionary");
    btdups(soundexDictionary,1);
    int length = 0;
    char word[20];
    char meaning[maxSizeWord];
    char* sameSoundexWords[20];
    load_file(dictionary, soundexDictionary, "test.txt");
    printDictionary(dictionary);
    menu();
    while(1){
        int cmd;
        printf("your command: "); 
        scanf("%d",&cmd);
        if(cmd == 0){
            printDictionary(dictionary);
        }else if(cmd == 1){
            getchar();
            printf("enter word: ");
            input(word);
            printf("enter meaning: ");
            input(meaning);
            addWord(dictionary, soundexDictionary, word, meaning);
        }else if(cmd == 2){
            getchar();
            printf("enter word for searching: ");
            input(word);
            search(dictionary, soundexDictionary,word, meaning);
        }else if(cmd == 3){
            getchar();
            printf("enter word for deleting: ");
            input(word);
            int rsize;
            int flag = btsel(dictionary, word, meaning, maxSizeWord, &rsize);
            if(flag != 0){
                printf("not found!\n");
            }else{
            delete(dictionary, soundexDictionary, word);}
        }else if(cmd == 4){
            getchar();
            printf("enter word for complete: ");
            tab(dictionary);
        }else{
            break;
        }
    }
    printf("finish!\n");
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
void input(char word[]){
    char* str = (char*)malloc(sizeof(char)* maxSizeWord);
    fgets(str, maxSizeWord, stdin);
    for(int i = 0; i < strlen(str); i ++){ str[i] = tolower(str[i]); } 
    str[strlen(str) -1 ] = '\0';
    strcpy(word,str);
}
// 1st feature
void addWord(BTA* dictionary, BTA* soundexDictionary, char word[], char meaning[]){
    char soundexCode[] = "    ";// 4 spaces
    soundex(word, soundexCode);
    word[strlen(word)] = '\0';// modified
    btins(dictionary, word, meaning, strlen(meaning) + 1);
    btins(soundexDictionary, soundexCode, word, strlen(word) + 1);
}
void search(BTA *dictionary, BTA* soundexDictionary, char word[], char meaning[])
{
    int size;
    int flag = btsel(dictionary, word, meaning, maxSizeWord, &size);
    if (flag == 0){
        printf("%s\n", meaning);
    }else{
        char* same[20];
        int l = same_soundex(soundexDictionary, word, same);
        if(l == 0){
            printf("not found!\n");
        }else{
        for(int i = 0; i < l; i ++){
            printf("%s  ",same[i]);
        }printf("\n");}
    }
}
void delete(BTA* dictionary, BTA* soundexDictionary, char word[]){
    char soundexCode[] = "    ";
    soundex(word, soundexCode);
    int sig1 = btdel(dictionary, word);
    int sig2 = btdel(soundexDictionary, soundexCode);
    if(sig1 == 0){
        printf("successful deleting\n");
    }
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
    char str[50];
    char same[maxSizeWord];
    scanf("%[^\t]%*c", str);

    int l = tab_list(dictionary, str, same);
    if(l == 0){
        printf("error\n");
    }else{
            printf("%s\n",same);
    } 
}
int tab_list(BTA* dictionary, char str[], char same[]){
    int total = 0;
    int value;
    char key[50];
    int flag = bfndky(dictionary, str, &value);
    // printf("%d",flag);
    if(flag == 0){
        strcpy(same, str); 
        do{
            flag = bnxtky(dictionary, key, &value);
            if(flag == 0 && strncmp(key, str, strlen(str)) == 0){
                strcpy(same, key); total = 1;
                break;
            }
        }while(flag == 0);
     }else{
        int sig = btpos(dictionary, 1);
        while(sig == 0){
            sig = bnxtky(dictionary, key, &value);
            if(sig == 0 && strncmp(key, str, strlen(str)) == 0){
                strcpy(same, key); total = 1; 
                break;
            }
        }
    }
    return total;
}
