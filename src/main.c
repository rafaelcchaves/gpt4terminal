#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "requests.h"
#include "querydata.h"

#define OPENAI_API "https://api.openai.com/v1/chat/completions"

int getKey(char* key, int len){
    char key_path[128];
    strcpy(key_path, getenv("HOME"));
    strcat(key_path, "/.gpt4terminal");
    FILE *file = fopen(key_path, "rb");
    if(file == NULL)
	return 0;
    fread(key, sizeof(char), len, file);
    for(int i = 0; i < len; i++){
	if(key[i] == '\n'){
	    key[i] = '\0';
	}
    }
    return 1;
}

int main(int argc, char**argv){
    if(argc == 1){
	printf("Olá eu sou o gpt4terminal!\n\n");
	printf("Para fazer uma consulta ao gpt use o seguinte comando:" );
	printf("\'gpt4terminal \"pergunta...\"'.\n");
	return 0;
    }
    
    char content_type[64], key[128], auth[256];
    getKey(key, 128);
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
