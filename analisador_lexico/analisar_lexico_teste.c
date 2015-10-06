#include <stdio.h>
#include <stdlib.h>
#include "analisador_lexico.h"

#define DEBUG_MODE 1

int main(){
	printf("Inicializando execucao\n");

    FILE *src_file = fopen("teste.txt", "r");

    FILE *output = fopen("output.txt", "w");

	if(src_file != NULL)
	{
		initConfigFile(src_file);

		init_keyword();

		Token* t;

		for(t = getToken(); t->tokenClass != EOA; t = getToken())
		{
			print_token(t, output);

			if(t->tokenClass == ERR)
			{
				printf("Lexical error at: line %d, column %d\n", t->line, t->column);
				printf("Exiting...\n");

				return 1;
			}
		}

		print_token(t, output);
	}
	else
	{
		printf("Error opening file!");
	}

	fclose(src_file);
    fclose(output);

	printf("End of file\n");
	return 0;

}
