#include "9cc.h"

LVar *locals;

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *mul();
Node *add();
Node *unary();
Node *primary();

Node *new_node(NodeKind kind)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_num(int val)
{
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

Node *new_assign(Node *lhs, Node *rhs)
{
	Node *node = new_node(ND_ASSIGN);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_expr_stmt(Node *lhs)
{
	Node *node = new_node(ND_EXPR_STMT);
	node->lhs = lhs;
	return node;
}

LVar *find_lvar(Token *tok)
{
	for (LVar *var = locals; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;
	return NULL;
}

void program() {
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

// stmt = expr ";"
Node *stmt()
{
	Node *node;

	if (consume("{")) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;

		Node **cur = &node->block.stmts;

		while (!consume("}")) {
			*cur = stmt();
			cur = &(*cur)->next;
		}

		return node;
	} else if (consume_token(TK_RETURN)) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();

		expect(";");

		return node;
	} else if (consume_token(TK_IF)) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_IF;

		expect("(");
		node->if_stmt.cond = expr();
		expect(")");

		node->if_stmt.then = stmt();

		if (consume_token(TK_ELSE))
			node->if_stmt.els = stmt();

		return node;
	} else if (consume_token(TK_WHILE)) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;

		expect("(");
		node->while_stmt.cond = expr();
		expect(")");

		node->while_stmt.stmt = stmt();

		return node;
	} else if (consume_token(TK_FOR)) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;

		expect("(");

		if (!consume(";")) {
			node->for_stmt.clause = new_expr_stmt(expr());
			expect(";");
		} else {
			node->for_stmt.clause = NULL;
		}

		if (!consume(";")) {
			node->for_stmt.expression2 = expr();
			expect(";");
		} else {
			node->for_stmt.expression2 = NULL;
		}

		if (!consume(")")) {
			node->for_stmt.expression3 = new_expr_stmt(expr());
			expect(")");
		} else {
			node->for_stmt.expression3 = NULL;
		}

		node->for_stmt.stmt = stmt();

		return node;
	} else {
		node = new_expr_stmt(expr());
	}

	if (!consume(";"))
		error_at(token->str, "';' expected.");

	return node;
}

// expr = equality
Node *expr()
{
	return assign();
}

Node *assign()
{
	Node *node = equality();
	if (consume("="))
		node = new_assign(node, assign());
	return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality()
{
	Node *node = relational();

	for (;;) {
		if (consume("=="))
			node = new_binary(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_binary(ND_NE, node, relational());
		else
			return node;
	}
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational()
{
	Node *node = add();

	for (;;) {
		if (consume("<"))
			node = new_binary(ND_LT, node, add());
		else if (consume("<="))
			node = new_binary(ND_LE, node, add());
		else if (consume(">"))
			node = new_binary(ND_LT, add(), node);
		else if (consume(">="))
			node = new_binary(ND_LE, add(), node);
		else
			return node;
	}
}

// add = mul ("+" mul | "-" mul)*
Node *add()
{
	Node *node = mul();

	for (;;) {
		if (consume("+"))
			node = new_binary(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_binary(ND_SUB, node, mul());
		else
			return node;
	}
}

// mul = unary ("*" unary | "/" unary)*
Node *mul()
{
	Node *node = unary();

	for (;;) {
		if (consume("*"))
			node = new_binary(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_binary(ND_DIV, node, unary());
		else
			return node;
	}
}

// unary = ("+" | "-")? unary
//       | primary
Node *unary()
{
	if (consume("+"))
		return unary();
	if (consume("-"))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

// primary = "(" expr ")" | num | ident
Node *primary()
{
	if (consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_token(TK_IDENT);
	if (tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		LVar *lvar = find_lvar(tok);
		if (lvar) {
			node->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			lvar->offset = locals ? locals->offset + 8 : 8;
			node->offset = lvar->offset;
			locals = lvar;
		}

		return node;
	}

	return new_num(expect_number());
}
