#include "9cc.h"

bool consume(char *op)
{
	if (token->kind != TK_RESERVED ||
			strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		return false;

	token = token->next;
	return true;
}

Token *consume_token(TokenKind kind)
{
	if (token->kind != kind) {
		return NULL;
	}

	Token *t = token;
	token = token->next;

	return t;
}

void expect(char *op)
{
	if (token->kind != TK_RESERVED ||
			strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		error_at(token->str, "expected \"%s\"", op);

	token = token->next;
}

int expect_number()
{
	if (token->kind != TK_NUM)
		error_at(token->str, "expected a number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof()
{
	return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
}

bool is_alnum(char c) {
	return ('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z') ||
		('0' <= c && c <= '9') ||
		(c == '_');
}

Token *tokenize()
{
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (startswith(p, "==") || startswith(p, "!=") ||
			startswith(p, "<=") || startswith(p, ">=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (strchr("+-*/()<>=;", *p)) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if (is_alnum(*p)) {
			cur = new_token(TK_IDENT, cur, p, 0);
			char *q = p;
			while (is_alnum(*p)) {
				p++;
			}
			cur->len = p - q;
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}
