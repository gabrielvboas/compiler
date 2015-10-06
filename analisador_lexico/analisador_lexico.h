#ifndef ANALISADOR_LEXICO_H_INCLUDED
#define ANALISADOR_LEXICO_H_INCLUDED

typedef enum{
	INT,
	FLT,
	IDN,
	OPR,
	CHR,
	STR,
	KEY,
	VAR,
	ERR,
	EOA
} TokenClass;

typedef struct{
	TokenClass tokenClass;
	char* value;
	int line;
	int column;
} Token;

typedef struct{
	int index;
	int isAcceptionState;
	int isIgnoringState;
	char* name;
} State;

typedef struct{
	State* states;
	int numOfStates;
	State*** transitions;
} StateMachine;

StateMachine* initStateMachine(FILE* file);

State* getFirstState(StateMachine* machine);

State* GetNextState(StateMachine* machine, State* s, char c);

void PrintStateMachine(StateMachine* machine);

void initConfigFile();

Token* getToken();

void append_char(char** str, char c);

TokenClass get_class(char* state_name, char* buffer);

void init_keyword();

void update_line_column();

void print_token(Token* t, FILE * output);

int search_keyword(char* idn);

#endif // ANALISADOR_LEXICO_H_INCLUDED
