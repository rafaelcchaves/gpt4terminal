#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "requests.h"
#define OPENAI_API "https://api.openai.com/v1/chat/completions"

int getToken(char* token, int len){
    char token_path[128];
    strcpy(token_path, getenv("HOME"));
    strcat(token_path, "/.gpt4terminal");
    FILE *file = fopen(token_path, "rb");
    if(file == NULL)
	return 0;
    fread(token, sizeof(char), len, file);
    for(int i = 0; i < len; i++){
	if(token[i] == '\n'){
	    token[i] = '\0';
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
    
    char content_type[64], token[128], auth[256], data[1024];
    getToken(token, 128);
    sprintf(content_type, "Content-Type: application/json");
    sprintf(auth, "Authorization: Bearer %s", token);
    char* headers[] = {content_type, auth}; 
    sprintf(
	data,
	"{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"user\",\"content\": \"%s\"}]}",
	argv[1]
    );
    char *response  = make_request(OPENAI_API, 2, headers, data);
    if(response != NULL) 
	printf("%s\n",response);
    else
	printf("%s\n",REQUEST_ERROR);
    return 0;
}
