#include "9cc.h"

char *user_input;
Token *token;

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Invalid arguments.\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize();
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	codegen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
