#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "requests.h"
#define OPENAI_API "https://api.openai.com/v1/chat/completions"

char* setQueryData(char* content_value,  char* model_value){
    if(content_value == NULL ||  model_value == NULL)
	return NULL;
    cJSON* data = cJSON_CreateObject();
    cJSON* model = cJSON_CreateString(model_value);
    cJSON_AddItemToObject(data, "model", model);
    cJSON* messages = cJSON_CreateArray();
    cJSON_AddItemToObject(data, "messages", messages);
    cJSON* message = cJSON_CreateObject();
    cJSON_AddItemToArray(messages, message);
    cJSON* role = cJSON_CreateString("user");
    cJSON* content = cJSON_CreateString(content_value);
    cJSON_AddItemToObject(message, "role", role);
    cJSON_AddItemToObject(message, "content", content);
    return cJSON_Print(data);
}

int getQueryAmountTokenUsed(char *response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* usage = cJSON_GetObjectItem(response, "usage");
    cJSON* total_tokens = cJSON_GetObjectItem(usage, "total_tokens");
    if(total_tokens == NULL){
	return -1;
    }
    return total_tokens->valueint;
}

char* getQueryAnswer(char *response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* choices = cJSON_GetObjectItem(response, "choices");
    cJSON* item = cJSON_GetArrayItem(choices, 0);  
    cJSON* message = cJSON_GetObjectItem(item, "message");
    cJSON* content = cJSON_GetObjectItem(message, "content");
    if(content == NULL)
	return NULL;
    return content->valuestring;
}	

char* getQueryError(char* response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* error = cJSON_GetObjectItem(response, "error");
    cJSON* message = cJSON_GetObjectItem(error, "message");
    if(message == NULL)
	return NULL;
    return message->valuestring;
}

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
    char* data = setQueryData(argv[1], "gpt-3.5-turbo");
    char* response  = make_request(OPENAI_API, 2, headers, data);
    if(response != NULL){
	char* content = getQueryAnswer(response);
	if(content != NULL){
	    printf("CHATGPT:\n\n%s\n\n", content);
	    int total_tokens = getQueryAmountTokenUsed(response);
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
