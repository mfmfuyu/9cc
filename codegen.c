#include "9cc.h"

void codegen_lval(Node *node)
{
	if (node->kind != ND_LVAR)
		error("");

	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}

int label_count = 0;
void codegen_if_stmt(Node *node)
{
	int id = label_count++;

	codegen(node->if_stmt.cond);
	printf("	pop rax");
	printf("	cmp rax, 0\n");

	if (node->if_stmt.els) {
		printf("	je .Lelse%d\n", id);
		codegen(node->if_stmt.then);
		printf("	jmp .Lend%d\n", id);
		printf(".Lelse%d:\n", id);
		codegen(node->if_stmt.els);
	} else {
		printf("	je .Lend%d\n", id);
		codegen(node->if_stmt.then);
	}

	printf(".Lend%d:\n", id);
}

void codegen_while_stmt(Node *node)
{
	int id = label_count++;

	printf(".Lbegin%d:\n", id);
	codegen(node->while_stmt.cond);
	printf("	pop rax\n");
	printf("	cmp rax, 0\n");
	printf("	je .Lend%d\n", id);
	codegen(node->while_stmt.stmt);
	printf("	jmp .Lbegin%d\n", id);
	printf(".Lend%d\n:", id);
}

void codegen_for_stmt(Node *node)
{
	int id = label_count++;

	if (node->for_stmt.clause)
		codegen(node->for_stmt.clause);

	printf(".Lbegin%d:\n", id);

	if (node->for_stmt.expression2) {
		codegen(node->for_stmt.expression2);

		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n", id);
	}

	if (node->for_stmt.stmt)
		codegen(node->for_stmt.stmt);

	if (node->for_stmt.expression3)
		codegen(node->for_stmt.expression3);

	printf("	jmp .Lbegin%d\n", id);

	printf(".Lend%d:\n", id);
}

void codegen_block(Node *node)
{
	for (Node *n = node->block.stmts; n; n = n->next)
		codegen(n);
}

void codegen(Node *node)
{
	if (node->kind == ND_NUM) {
		printf("	push %d\n", node->val);
		return;
	} else if (node->kind == ND_LVAR) {
		codegen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;
	} else if (node->kind == ND_ASSIGN) {
		codegen_lval(node->lhs);
		codegen(node->rhs);

		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		return;
	} else if (node->kind == ND_RETURN) {
		codegen(node->lhs);
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;
	} else if (node->kind == ND_IF) {
		codegen_if_stmt(node);
		return;
	} else if (node->kind == ND_WHILE) {
		codegen_while_stmt(node);
		return;
	} else if (node->kind == ND_FOR) {
		codegen_for_stmt(node);
		return;
	} else if (node->kind == ND_BLOCK) {
		codegen_block(node);
		return;
	} else if (node->kind == ND_EXPR_STMT) {
		codegen(node->lhs);
		printf("	pop rax\n");
		return;
	}

	codegen(node->lhs);
	codegen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch (node->kind) {
	case ND_ADD:
		printf("	add rax, rdi\n");
		break;
	case ND_SUB:
		printf("	sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("	imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("	cqo\n");
		printf("	idiv rdi\n");
		break;
	case ND_EQ:
		printf("	cmp rax, rdi\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_NE:
		printf("	cmp rax, rdi\n");
		printf("	setne al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_LT:
		printf("	cmp rax, rdi\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_LE:
		printf("	cmp rax, rdi\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
		break;
	}

	printf("	push rax\n");
}
