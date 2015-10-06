#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analisador_lexico.h"

int LINE = 0;
int COLUMN = 0;
char CURRENT_CHAR  = 0;
char LAST_CHAR = 0;
FILE* SOURCE_CODE;
StateMachine* STATEMACHINE;
char CLASS_NAME[][4] = { "INT", "FLT", "IDN", "OPR", "CHR", "STR", "KEY", "VAR", "ERR", "EOA" }; // Use the state name to determine the class
int NB_CLASS_NAME = 10; // The number of state names
int LINE;
int COLUMN;
int FILE_ENDED = 0;
const char *KEYWORDS[50];

/*
*
* Inicializa a maquina de estados a partir de um arquivo que a identifica. Retorna a maquina de estados.
*
*/
StateMachine* initStateMachine(FILE* file){
    /* Variaveis para index */
    int i = 0;
    int j = 0;
    int numOfStates = 0;
    int numOfAcceptionStates = 0;
    int numOfIgnoringStates = 0;

	/* Alocacao da maquina de estados */
	StateMachine* machine = (StateMachine*) malloc(sizeof(StateMachine));

	/* Leitura do numero de estados da maquinaSTATEMACHINE de estados */
	fscanf(file, "%d", &numOfStates);
	machine->numOfStates = numOfStates;

	/* Alocacao dos estados da maquina de estados */
	machine->states = (State*) malloc(numOfStates * sizeof(State));

	/* Inicializacao dos estados da maquina de estados */
	for(i = 0; i < numOfStates; i++){
		State s;
		s.isAcceptionState = 0;
		s.isIgnoringState = 0;
		s.index = i;
		machine->states[i] = s;
	}

	/* Leitura do numero de estados de aceitacao da maquina de estados */
	fscanf(file, "%d", &numOfAcceptionStates);

	/* Identificacao dos estados de aceitacao da maquina de estados */
	for(i = 0; i < numOfAcceptionStates; i++){

		int index;
		char name[32];

		fscanf(file, "%d %s", &index, name);

		machine->states[index].isAcceptionState = 1;
		machine->states[index].name = (char*) malloc((strlen(name) + 1)* sizeof(char));
		strcpy(machine->states[index].name, name);
	}

    /* Identificacao dos estados a serem ignorados da maquina de estados */
	fscanf(file, "%d", &numOfIgnoringStates);
	for(i = 0; i < numOfIgnoringStates; i++)
	{
		int state_index;

		// We read the state index and the name from the file
		fscanf(file, "%d", &state_index);

		machine->states[state_index].isIgnoringState = 1;
	}

	/* Alocacao das transicoes da maquina de estados */
	machine->transitions = (State***) malloc(machine->numOfStates * sizeof(State**));

	/* Leitura das transicoes de estados da maquina de estados */
	for(i = 0; i < machine->numOfStates; i++)
	{
		machine->transitions[i] = (State**) malloc(128 * sizeof(State*));
		for(j = 0; j < 128; j++)
		{
			int index = -1;
			State* state;

			fscanf(file, "%d", &index);
			if(index == -1)
			{
				state = NULL;
			}
			else
			{
				state = &(machine->states[index]);
			}

			machine->transitions[i][j] = state;
		}
	}

	return machine;
}

/*
*
* Retorna o primeiro estado SOURCE_CODEda maquina de estados
*
*/
State* getFirstState(StateMachine* machine){
	return &(machine->states[0]);
}


/*
*
* Retorna o proximo estado da maquina de estados
*
*/
State* GetNextState(StateMachine* machine, State* s, char c)
{
	int i = c;

	return machine->transitions[s->index][i];
}


/*
*
* Retorna o proximo estado da maquina de estados
*
*/
void PrintStateMachine(StateMachine* machine)
{

	int i;

	for(i = 0; i < machine->numOfStates; i++)
	{
		int j;

		State* s = &(machine->states[i]);
		State* s1 = NULL;

		printf("State %d", i);

		if(machine->states[i].isAcceptionState)
		{
			printf(" -> accepting state! Returns %s", machine->states[i].name);
		}

		printf("\n");

		printf("Accepts transitions with: ");
		for(j = 0; j < 128; j++)
		{
			s1 = GetNextState(machine, s, j);

			if(s1 != NULL)
			{
				printf("%c", j);
			}
		}

		printf("\n");
	}
}

/*
*
* Carrega o arquivo com a maquina de estados e a inicializa
*CLASS_NAME
*/
void initConfigFile(FILE *sourceFile){
    printf("Carregando arquivo de configuracao\n");
	FILE *configFile;
	configFile = fopen("stateMachineConfig.txt", "r");
	if(configFile == NULL){
        printf("\nArquivo nao encontrado\n");
        return;
	}
	printf("Arquivo de configuracao carregado\n");
	printf("Criando state machine\n");
	STATEMACHINE = initStateMachine(configFile);
	if(STATEMACHINE){
        PrintStateMachine(STATEMACHINE);
	}
	printf("State machine criada\n");

    SOURCE_CODE = sourceFile;

    CURRENT_CHAR = fgetc(SOURCE_CODE);

	fclose(configFile);
}

/*
*
* Retorna o token
*
*/
Token* getToken()
{
	Token* token;
	char* buffer;

	token = (Token*) malloc(sizeof(Token));

	buffer = (char*) malloc(sizeof(char));
	buffer[0] = '\0';

	// We start the machine
	State* current_state;
	State* next_state;
	current_state = NULL;
	next_state = getFirstState(STATEMACHINE);

	// We iterate while the next_state isn't NULL
	int token_noted = 0;
	while(next_state != NULL)
	{
		// We get the next state
		current_state = next_state;
		if(CURRENT_CHAR != EOF)
		{
			next_state = GetNextState(STATEMACHINE, current_state, CURRENT_CHAR);
		}
		else
		{
			next_state = NULL;
		}

		// If the next state is NULL, the state machine has finished reading the string
		if(next_state != NULL)
		{
			// We write the contents on the buffer:
			// If it is in a ignoring state, ignore the buffer (commentary and blank characters
			if(!(next_state->isIgnoringState))
			{
				// Append the char to the buffer
				append_char(&buffer, CURRENT_CHAR);

				// Write the first line and column of the token, if it wasn't
				if(!token_noted)
				{
					token->line = LINE;
					token->column = COLUMN;

					token_noted = 1;
				}
			}

			// We get the next char
			LAST_CHAR = CURRENT_CHAR;
			CURRENT_CHAR = fgetc(SOURCE_CODE);
			update_line_column();
		}
	}

	// We complete the token using the information from the state where we are
	if(current_state != NULL && !(current_state->isIgnoringState)) // If the current state is not null and not a ignoring state
	{
		token->tokenClass = get_class(current_state->name, buffer);

		token->value = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(token->value, buffer);
	}
	else
	{
		if(CURRENT_CHAR == EOF)
		{
			FILE_ENDED = 1;

			token->tokenClass = EOA;
			token->value = NULL;
		}
		else
		{
			token->tokenClass = ERR;

			token->value = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
			strcpy(token->value, buffer);

			token->line = LINE;
			token->column = COLUMN;
		}
	}

	return token;
}

void update_line_column(){
	if(LAST_CHAR == '\n')
	{
		LINE++;
		COLUMN = 0;
	}
	else
	{
		COLUMN++;
	}
}

void print_token(Token* t, FILE * output)
{
	printf("-------------------------------\n");
	printf("Token\n");
	printf("Class: %s\n", CLASS_NAME[t->tokenClass]);
	printf("Value: %s\n", t->value);
	printf("Position: line %d, column %d\n", t->line, t->column);
	printf("-------------------------------\n\n");

	fprintf(output, "-------------------------------\n");
	fprintf(output, "Token\n");
	fprintf(output, "Class: %s\n", CLASS_NAME[t->tokenClass]);
	fprintf(output, "Value: %s\n", t->value);
	fprintf(output, "Position: line %d, column %d\n", t->line, t->column);
	fprintf(output, "-------------------------------\n\n");
}

void append_char(char** str, char c)
{
	char* new_str = NULL;

	new_str = (char*) malloc((strlen(*str) + 2) * sizeof(char));
	strcpy(new_str, *str);
	new_str[strlen(*str)] = c;
	new_str[strlen(*str) + 1] = '\0';

	free(*str);

	*str = new_str;
}

TokenClass get_class(char* state_name, char* buffer)
{
	TokenClass class = -1;
	int i;

	for(i = 0; i < NB_CLASS_NAME; i++)
	{
		if(!strcmp(CLASS_NAME[i], state_name))
		{
			class = i;
		}
	}

	if(class == -1)
	{
		class = ERR;
	}

	if(class == IDN)
	{
		if(search_keyword(buffer))
		{
			class = KEY;
		}
		else
		{
			class = VAR;

		}
	}

	return class;
}

void init_keyword()
{
	FILE* init_file;
	char keyword_aux[32];
	int i = 0;

	init_file = fopen("keywords.txt", "r");

	for(i = 0; i < 50; i++){
        KEYWORDS[i] = "VAZIO";
	}

	i = 0;

	while((fscanf(init_file, "%s", keyword_aux)) != EOF){
		char* keyword;

		// Populates the linked list
		keyword = (char*) malloc((strlen(keyword_aux) + 1) * sizeof(char));
		strcpy(keyword, keyword_aux);

		KEYWORDS[i] = keyword;
		i++;
	}

	fclose(init_file);
}

int search_keyword(char* idn)
{
    int i = 0;

    for(i = 0; i < 50; i++){
        if(idn == KEYWORDS[i])
            return 1;
    }

    return 0;
}

