#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <cjson/cJSON.h>


int getToken(char* token, int len){
    char token_path[128];
    strcpy(token_path, getenv("HOME"));
    strcat(token_path, "/.gpt4terminal");
    FILE *file = fopen(token_path, "rb");
    if(file == NULL)
	return 0;
    fread(token, sizeof(char), 512, file);
    return 1;
}

int main(int argc, char**argv){
    if(argc == 1){
	printf("Olá eu sou o gpt4terminal!\n\n");
	printf("Para fazer uma consulta ao gpt use o seguinte comando:" );
	printf("\'gpt4terminal \"pergunta...\"'.\n");
	return 0;
    }
    
    char buffer[512];
    getToken(buffer, 512);
    printf("%s\n", buffer);
    
    return 0;
}
