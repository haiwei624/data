#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"

#define MAX_REG 8

int get_reg();
int get_offset(IdentifierSemanticValue IDVal);
void gen_head(char* name);
void gen_prologue(char* name);
void gen_epilogue(char* name);
void code_gen(AST_NODE* node);
void visit_sym(AST_NODE* node);
void visit_const(AST_NODE* node);
void visit_operater(AST_NODE* node);
void visit_assignment(AST_NODE* node);
void visit_program(AST_NODE* node);
void visit_GlobalSymDeclList(AST_NODE* node);
void visit_FuncDecl(AST_NODE* node);
void visit_GlobalSymDecl(AST_NODE* node);
int calc_TypeSize(AST_NODE* node);
int calc_DimSize(AST_NODE* node);

void gen_head(char* name)
{
	printf(".text\n%s:\n",name);
}

void gen_prologue(char* name)
{
	printf("# prologue sequence\n");

	printf("sw	$ra, 0($sp)\nsw	$fp, -4($sp)\nadd	$fp, $sp, -4\nadd	$sp, $sp, -8\nlw	$2, _framesize_%s\nsub	$sp,$sp,$2\n",name);
	for(int i=1; i<=MAX_REG; i++)
	{
		printf("sw  $%d,%d($sp)\n", i+7, 4*(MAX_REG-i+1));
	}
	printf("_begin_%s:\n",name);

	printf("# prologue sequence end\n");
}

void gen_epilogue(char* name)
{
	printf("# epilogue sequence\n");

	printf("_end_%s:\n",name);
	for(int i=1; i<=MAX_REG; i++)
	{
		printf("lw  $%d,%d($sp)\n", i+7, 4*(MAX_REG-i+1));
	}
	printf("lw	$ra, 4($fp)\nadd	$sp, $fp, 4\nlw	$fp, 0($fp)\n");
	if(strcmp(name, "main") == 0)
	{
		printf("li	$v0, 10\n  syscall\n");
	}
	else
	{
		printf("jr	$ra\n");
	}
	printf(".data\nframesize_%s: .word %d\n",name,1024);//need to be finished.

	printf("# epilogue sequence end\n");
}

void code_gen(AST_NODE *node)
{
	if(node == NULL) return;

	switch(node->nodeType)
	{
		case CONST_VALUE_NODE:
			visit_const(node);
			break;
		case IDENTIFIER_NODE:
			visit_sym(node);
			break;
		case EXPR_NODE:
			visit_operater(node);//!!need to be changed.
			break;
		case PROGRAM_NODE:
			visit_program(node);
			break;
		default:
			printf("This ASTnode type is not supported now.");
			break;
	}
}

void visit_program(AST_NODE* node)
{
	AST_NODE *child = node->child;
	while(child)
	{
		if(child->nodeType == VARIABLE_DECL_LIST_NODE)
		{
			visit_GlobalSymDeclList(child);
		}
		else
		if(child->nodeType == DECLARATION_NODE)//function decl
		{
			visit_FuncDecl(child);
		}
		else printf("error in program node.\n");
		child = child->rightSibling;
	}
}

void visit_GlobalSymDeclList(AST_NODE* node)
{
	printf("data.\n");

	AST_NODE* child = node->child;
	while(child)
	{
		if(child->semantic_value.declSemanticValue.kind == VARIABLE_DECL)
			visit_GlobalSymDecl(child);

		child = child->rightSibling;
	}

}

void visit_GlobalSymDecl(AST_NODE* node)//array not supported, variable init not supported
{
	AST_NODE* child = node->child;
	int typesize = calc_TypeSize(child);

	child = child->rightSibling;
	while(child)
	{
		int symsize = typesize;
		if(child->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
		{
			int dimsize = calc_DimSize(child);
			symsize = symsize * dimsize;
		}
		else
		if(child->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
		{
			
		}
		else printf("variable init not supported now.");


	}
}

int calc_TypeSize(AST_NODE* node)
{
	int ans=1;
	return ans;
}

int calc_DimSize(AST_NODE* node)
{
	int ans=1;
	return ans;
}

void visit_FuncDecl(AST_NODE* node)
{

}

void visit_sym(AST_NODE* node)
{
	int offset,reg;
	reg = get_reg();
	node->regplace = reg;
	offset = get_offset(node->semantic_value.identifierSemanticValue);
	printf("\t lw    $%d, %d($fp)\n", reg, offset);//if local
	// ...   if global
}

void visit_const(AST_NODE* node)
{
	//if(!node->semantic_value.exprSemanticValue.isConstEval) printf("#!!!!!!!ERROR!!!!!!!Not const!!!\n");
	int reg;
    reg = get_reg();
    node->regplace = reg;
    if (node->semantic_value.const1->const_type==INTEGERC) printf("\t li    $%d, %d\n", reg, node->semantic_value.const1->const_u.intval);
    if (node->semantic_value.const1->const_type==FLOATC) printf("\t li    $%d, %f\n", reg, node->semantic_value.const1->const_u.fval);
    if (node->semantic_value.const1->const_type==STRINGC) printf("\t li    $%d, %s\n", reg, node->semantic_value.const1->const_u.sc);
    
}

void visit_operater(AST_NODE* node)//only support interger now,float reg need to be considered.
{
	int reg1,reg2,reg3;
	AST_NODE *lchild, *rchild;
	lchild = node->child;
	if(lchild != NULL)
	{
		code_gen(lchild);
		reg1 = lchild->regplace;
		rchild = lchild->rightSibling;
		if(rchild != NULL) 
		{
			code_gen(rchild);
			reg2 = rchild->regplace;
		}
	}
	reg3 = get_reg();
	if(node->semantic_value.exprSemanticValue.kind == BINARY_OPERATION)
	{
		if(node->dataType == INT_TYPE)
			switch(node->semantic_value.exprSemanticValue.op.binaryOp)
			{
				case BINARY_OP_ADD:
					printf("\t add   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_SUB:
					printf("\t sub   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_MUL:
					printf("\t mul   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_DIV:
					printf("\t div   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_AND:
					printf("\t and   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_OR:
					printf("\t or   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_EQ:
					printf("\t seq   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_GE:
					printf("\t sge   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_LE:
					printf("\t sle   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_NE:
					printf("\t sne   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_GT:
					printf("\t sgt   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_LT:
					printf("\t slt   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				default:
					printf("This operater is not supported now.\n");
					break;
			}
		else
			switch(node->semantic_value.exprSemanticValue.op.binaryOp)
			{
				case BINARY_OP_ADD:
					printf("\t add.s   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_SUB:
					printf("\t sub.s   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_MUL:
					printf("\t mul.s   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_DIV:
					printf("\t div.s   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_EQ:
					printf("\t seq   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_GE:
					printf("\t sge   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_LE:
					printf("\t sle   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_NE:
					printf("\t sne   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_GT:
					printf("\t sgt   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_LT:
					printf("\t slt   $%d, $%d, $%d\n", reg3, reg1, reg2);
					break;
				default:
					printf("This operater is not supported now.\n");
					break;
			}
	}
	else
	{
		if(node->dataType == INT_TYPE)
			switch(node->semantic_value.exprSemanticValue.op.unaryOp)
			{
				case UNARY_OP_POSITIVE:
					reg3 = reg1;
					break;
				case UNARY_OP_NEGATIVE:
					printf("\t neg   $%d, $%d\n", reg3, reg1);
					break;
				case UNARY_OP_LOGICAL_NEGATION:
					printf("\t not   $%d, $%d\n", reg3, reg1);
					break;
				default:
					printf("This operater is not supported now.\n");
					break;
			}
		else printf("unaryop for float not supported.");
	}
	
	node->regplace = reg3;
}

void visit_assignment(AST_NODE* node)
{
	int offset,reg;
	AST_NODE *lchild, *rchild;
	lchild = node->child;
	rchild = lchild->rightSibling;
	reg = rchild->regplace;
	SymbolTableEntry *entry = lchild->semantic_value.identifierSemanticValue.symbolTableEntry;
	if(entry->nestingLevel > 0)//local
	{
		offset = get_offset(lchild->semantic_value.identifierSemanticValue);
		printf("\tsw   $%d, %d($fp)\n", reg, offset);
	}
	else//global
	{
		printf("\tla   $v1, _globalsym_%s\n",lchild->semantic_value.identifierSemanticValue.identifierName);
		printf("\tsw   $%d, 0($v1)\n",reg);
	}
}