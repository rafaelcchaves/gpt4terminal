#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "requests.h"
#include "querydata.h"

#define OPENAI_API "https://api.openai.com/v1/chat/completions"
#define KEY_FILENAME "/.gpt4terminal"

FILE* openKeyFile(const char* mode){
    char key_path[128];
    strcpy(key_path, getenv("HOME"));
    strcat(key_path, KEY_FILENAME);
    return fopen(key_path, mode);
}

int getKey(char* key, int len){
    FILE* file = openKeyFile("r"); 
    if(file == NULL)
	return 0;
    fread(key, sizeof(char), len, file);
    for(int i = 0; i < len; i++){
	if(key[i] == '\n'){
	    key[i] = '\0';
	}
    }
    fclose(file);
    return 1;
}

int saveKey(char* key, int len){
    FILE* file = openKeyFile("w+");
    if(file == NULL)
	return 0;
    fwrite(key, 1, len, file);
    fclose(file);
    return 1;
}

int main(int argc, char**argv){
    if(argc == 1){
	printf("To make a query use the following command:" );
	printf("\'gpt4terminal \"Query question\"'.\n");
	printf("Do you want to configure your OpenAI Key?(y/n)");
	char option = getchar();
	if(option == 'y' || option == 'Y'){
	    printf("Enter your OpenAI Key:");
	    char key[128];
	    scanf("%s", key);
	    if(saveKey(key, 128)){
		printf("Configuration done!\n");
	    }else{
		printf("An error has occurred.\n");
	    }
	}
	return 0;
    }
    
    char content_type[64], key[128], auth[256];
    
    if(getKey(key, 128) == 0){
	printf("OpenAI Key not found! To continue enter your Key: ");
	char key[128];
	scanf("%s", key);
	if(saveKey(key, 128)){
	    printf("Configuration done!\n");
	}else{
	    printf("An error has occurred.\n");
	    return 0;
	}
    }
    sprintf(content_type, "Content-Type: application/json");
    sprintf(auth, "Authorization: Bearer %s", key);
    char* headers[] = {content_type, auth}; 
    char* data = mountQueryData(argv[1], "gpt-3.5-turbo");
    char* response  = make_request(OPENAI_API, 2, headers, data);
    if(response != NULL){
	char* content = getQueryAnswer(response);
	if(content != NULL){
	    printf("CHATGPT:\n\n%s\n\n", content);
	    int total_tokens = getQueryTokenUsage(response);
	    if(total_tokens >= 0)
		printf("Tokens gastos: %d\n", total_tokens);
	} else {
	    char* error_message = getQueryError(response);
	    if(error_message != NULL)
		printf("ERROR:\n\n%s\n", error_message);
	    else
		printf("ERROR:\n\n%s\n",response);
	}
    } else {
	printf("%s\n",REQUEST_ERROR);
    }
    return 0;
}
