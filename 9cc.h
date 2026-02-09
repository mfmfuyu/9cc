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
	TK_IDENT,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
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

bool consume(char *op);
Token *consume_token();
void expect(char *op);
int expect_number();
Token *tokenize();
bool at_eof();

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
	ND_ASSIGN,
	ND_LVAR,
	ND_RETURN,
	ND_IF,
	ND_ELSE,
	ND_WHILE,
	ND_FOR,
	ND_BLOCK,
	ND_NUM, // Integer
	ND_EXPR_STMT
} NodeKind;

typedef struct Node Node;
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
	int offset;

	Node *next;

	union {
		struct { Node *cond; Node *then; Node *els; } if_stmt;
		struct { Node *cond; Node *stmt; } while_stmt;
		struct { Node *clause; Node *expression2; Node *expression3; Node *stmt; } for_stmt;
		struct { Node *stmts; } block;
	};
};

void program(void);

void codegen(Node *node);

extern char *user_input;
extern Token *token;
extern Node *code[100];

typedef struct LVar LVar;
struct LVar {
	LVar *next;
	char *name;
	int len;
	int offset;
};

extern LVar *locals;
