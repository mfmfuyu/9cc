#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * util
 */

void error(char *fmt, ...) __attribute__((noreturn));
void error_at(char *loc, char *fmt, ...) __attribute__((noreturn));

/*
 * Tokenize
 */

typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

Token *tokenize();

/*
 * Parser
 */

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
	ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

Node *expr();

void codegen(Node *node);

extern char *user_input;
extern Token *token;
