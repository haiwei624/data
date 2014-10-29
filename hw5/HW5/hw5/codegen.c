#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"

#define MAX_REG 18
#define POOLSIZE 1000000

void push_ConstText(char* s,int len);
void pop_ConstText();
void clear_ConstText();
int get_reg();
int get_freg();
int get_offset(IdentifierSemanticValue IDVal);
int get_strlabel();
int get_fltlabel();
int get_iflabel();
int get_whilelabel();
int get_label();
int calc_SymSize(AST_NODE* node);
void gen_head(char* name);
void gen_prologue(char* name);
void gen_epilogue(char* name, int ARsize);
void code_gen(AST_NODE* node);
void visit_sym(AST_NODE* node);
void visit_const(AST_NODE* node);
void visit_operater(AST_NODE* node);
void visit_program(AST_NODE* node);
void visit_GlobalSymDeclList(AST_NODE* node);
void visit_FuncDecl(AST_NODE* node);
void visit_GlobalSymDecl(AST_NODE* node);
void visit_block(AST_NODE* node);
void visit_StmtList(AST_NODE* node);
void visit_stmt(AST_NODE* node);
//stmt:
void visit_assignment(AST_NODE* node);
void visit_WhileStmt(AST_NODE* node);
void visit_ForStmt(AST_NODE* node);
void visit_IfStmt(AST_NODE* node);
void visit_FunctionCall(AST_NODE* node);
void visit_ReturnStmt(AST_NODE* node);
void visit_read(AST_NODE* node);
void visit_fread(AST_NODE* node);
void visit_write(AST_NODE* node);


char *FuncNameNow;
char ConstPool[POOLSIZE],TempPool[POOLSIZE];
int ConstPoolLen;

void clear_ConstText()
{
	ConstPoolLen = 0;
}
void push_ConstText(char* s,int len)
{
	int i;
	for(i=0;i<len;i++)ConstPool[ConstPoolLen++] = s[i];
}
void pop_ConstText()
{
	int i;
	for(i=0;i<ConstPoolLen;i++)printf("%c", ConstPool[i]);
	ConstPoolLen = 0;
}


int get_reg()
{
	static int reg_num=0;
	reg_num = (reg_num+1)%MAX_REG;
	return reg_num+8;
}

int get_freg()
{
	static int reg_num=0;
	reg_num = (reg_num+1)%31;
	if(reg_num == 11)reg_num++;
	return reg_num+1;
}

int get_offset(IdentifierSemanticValue IDVal)
{
	return IDVal.symbolTableEntry->offset;
}

int get_strlabel()
{
	static int num=0;
	num++;
	return num;
}

int get_fltlabel()
{
	static int num=0;
	num++;
	return num;
}

int get_iflabel()
{
	static int num=0;
	num++;
	return num;
}

int get_whilelabel()
{
	static int num=0;
	num++;
	return num;
}

int get_label()
{
	static int num=0;
	num++;
	return num;
}



void gen_head(char* name)
{
	printf(".text\n%s:\n",name);
}

void gen_prologue(char* name)
{
	printf("# prologue sequence\n");

	printf("sw	$ra, 0($sp)\nsw	$fp, -4($sp)\nadd	$fp, $sp, -4\nadd	$sp, $sp, -8\nlw	$2, _framesize_%s\nsub	$sp,$sp,$2\n",name);
	int i;
	for(i=1; i<=MAX_REG; i++)
	{
		printf("sw  $%d,%d($sp)\n", i+7, 4*(MAX_REG-i+1));
	}
	printf("_begin_%s:\n",name);

	printf("# prologue sequence end\n");
}

void gen_epilogue(char* name, int ARsize)
{
	printf("# epilogue sequence\n");

	printf("_end_%s:\n",name);
	int i;
	for(i=1; i<=MAX_REG; i++)
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
	printf(".data\n_framesize_%s: .word %d\n",name, ARsize+1024);//need to be finished.
	pop_ConstText();

	printf("# epilogue sequence end\n");
}

void code_gen(AST_NODE *node)
{
	if(node == NULL) return;

	switch(node->nodeType)
	{
		case NUL_NODE:
			break;
		case CONST_VALUE_NODE:
			visit_const(node);
			break;
		case IDENTIFIER_NODE:
			visit_sym(node);
			break;
		case EXPR_NODE:
			visit_operater(node);//!!need to be changed.
			break;
		case BLOCK_NODE:
			visit_block(node);
			break;
		case PROGRAM_NODE:
			visit_program(node);
			break;
		case VARIABLE_DECL_LIST_NODE:
			break;
		case STMT_LIST_NODE:
			visit_StmtList(node);
			break;
		case STMT_NODE:
			visit_stmt(node);
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
	printf(".data\n");

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
	AST_NODE* child = node->child;//type node
	

	child = child->rightSibling;
	while(child)
	{
		int typesize = calc_SymSize(child);
		printf("_globalsym_%s:    .space   %d\n",child->semantic_value.identifierSemanticValue.identifierName, typesize*4);
		/*if(child->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
		{
			int dimsize = calc_DimSize(child);
			symsize = symsize * dimsize;
			printf(".space   %d\n",symsize*4);// symsize*4 bytes
		}
		else
		if(child->semantic_value.identifierSemanticValue.kind == NORMAL_ID)
		{
			printf(".word   0\n");
		}
		else printf("variable init not supported now.");
		*/
		
		child = child->rightSibling;
	}
}

int calc_SymSize(AST_NODE* node)
{
	TypeDescriptor* typeDescriptor = node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
	if(typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR)
	{
		return 1;
	}
	else // ARRAY_TYPE_DESCRIPTOR
	{
		int dim = typeDescriptor->properties.arrayProperties.dimension;
		int tot=1,i;
		for(i=0; i<dim; i++) tot = tot * typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
		return tot;
	}
}

void visit_FuncDecl(AST_NODE* node)
{
	AST_NODE* namenode = node->child->rightSibling;
	AST_NODE* paralist = namenode->rightSibling;
	AST_NODE* blocknode = paralist->rightSibling;
	
	char* funcname = namenode->semantic_value.identifierSemanticValue.identifierName;
	FuncNameNow = funcname;
	SymbolTableEntry *symbolTableEntry = namenode->semantic_value.identifierSemanticValue.symbolTableEntry;
	FunctionSignature* functionSignature = symbolTableEntry->attribute->attr.functionSignature;
	clear_ConstText();
	gen_head(funcname);
	gen_prologue(funcname);
	
	code_gen(blocknode);
	

	gen_epilogue(funcname,functionSignature->ARsize);
}

void visit_block(AST_NODE* node)
{
	AST_NODE* listnode = node->child;
	while(listnode)
	{
		code_gen(listnode);
		listnode = listnode->rightSibling;
	}
}

void visit_StmtList(AST_NODE* node)
{
	AST_NODE* child = node->child;//stmt
	while(child)
	{
		code_gen(child);
		child = child->rightSibling;
	}
}

void visit_stmt(AST_NODE* node)
{
	switch(node->semantic_value.stmtSemanticValue.kind)
    {
        case WHILE_STMT:

            visit_WhileStmt(node);
            break;
        case FOR_STMT:
            visit_ForStmt(node);
            break;
        case ASSIGN_STMT:
            visit_assignment(node);
            break;
        case IF_STMT:
            visit_IfStmt(node);
            break;
        case FUNCTION_CALL_STMT:
            visit_FunctionCall(node);
            break;
        case RETURN_STMT:
            visit_ReturnStmt(node);
            break;
        default:
            printf("Unhandle case in void processStmtNode(AST_NODE* node)\n");
            node->dataType = ERROR_TYPE;
            break;
    }
}

void visit_sym(AST_NODE* node)
{
	int offset,reg;
	if(node->dataType == INT_TYPE) reg = get_reg();
		else reg = get_freg();
	node->regplace = reg;
	IdentifierSemanticValue IdValue = node->semantic_value.identifierSemanticValue;
	offset = get_offset(IdValue);
	if(IdValue.symbolTableEntry->nestingLevel > 0)
		printf("\t la   $v1, %d($fp)\n", offset);
	else
		printf("\t la   $v1, _globalsym_%s\n", IdValue.identifierName);
	if(IdValue.kind == ARRAY_ID)
	{
		AST_NODE* dimnode = node->child;
		code_gen(dimnode);
		int reg2 = dimnode->regplace;
		printf("\t mul  $%d, $%d, 4\n", reg2, reg2);
		if(IdValue.symbolTableEntry->nestingLevel > 0) printf("\t neg  $%d, $%d\n", reg2, reg2);
		printf("\t add  $v1, $v1, $%d\n", reg2);

	}
	if(node->dataType == INT_TYPE)
		printf("\t lw   $%d, 0($v1)\n", reg);
	else
		printf("\t l.s   $f%d, 0($v1)\n", reg);
	/*if(node->dataType == INT_TYPE)
	{
		if(IdValue.symbolTableEntry->nestingLevel > 0)//if local
			printf("\t lw    $%d, %d($fp)\n", reg, offset);
		else// ...   if global
			printf("\t lw    $%d, _globalsym_%s\n", reg, IdValue.identifierName);
	}
	else
	{
		if(IdValue.symbolTableEntry->nestingLevel > 0)//if local
			printf("\t l.s    $f%d, %d($fp)\n", reg, offset);
		else// ...   if global
			printf("\t l.s    $f%d, _globalsym_%s\n", reg, IdValue.identifierName);
	}*/
	
}

void visit_const(AST_NODE* node)//float unsupport
{
	//if(!node->semantic_value.exprSemanticValue.isConstEval) printf("#!!!!!!!ERROR!!!!!!!Not const!!!\n");
	int reg;
    
    if (node->semantic_value.const1->const_type==INTEGERC)
    {
    	reg = get_reg();
    	node->regplace = reg;
    	printf("\t li    $%d, %d\n", reg, node->semantic_value.const1->const_u.intval);
    }
    if (node->semantic_value.const1->const_type==FLOATC)
    {
    	int num = get_fltlabel();
    	int len = sprintf(TempPool,"_flt%d: .float %f\n", num, node->semantic_value.const1->const_u.fval);
    	push_ConstText(TempPool, len);
    	reg = get_freg();
    	node->regplace = reg;
    	printf("\t l.s    $f%d, _flt%d\n", reg, num);
    }
    if (node->semantic_value.const1->const_type==STRINGC)
    {
    	reg = get_strlabel();
    	int len = sprintf(TempPool,"_str%d: .asciiz %s\n", reg, node->semantic_value.const1->const_u.sc);
    	push_ConstText(TempPool, len);
    	node->regplace = reg;
    }
}

void visit_operater(AST_NODE* node)//only support interger now,float reg need to be considered.
{
	int reg1,reg2,reg3,label;
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
	if(node->dataType == INT_TYPE) reg3 = get_reg();
		else reg3 = get_freg();
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
					printf("\t add.s   $f%d, $f%d, $f%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_SUB:
					printf("\t sub.s   $f%d, $f%d, $f%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_MUL:
					printf("\t mul.s   $f%d, $f%d, $f%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_DIV:
					printf("\t div.s   $f%d, $f%d, $f%d\n", reg3, reg1, reg2);
					break;
				case BINARY_OP_EQ://not support
					printf("\t c.eq.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 1\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 0\n", reg3);
					printf("_%d:\n",label);
					break;
				case BINARY_OP_GE://not support
					printf("\t c.lt.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 0\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 1\n", reg3);
					printf("_%d:\n",label);
					break;
				case BINARY_OP_LE://not support
					printf("\t c.le.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 1\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 0\n", reg3);
					printf("_%d:\n",label);
					break;
				case BINARY_OP_NE://not support
					printf("\t c.eq.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 0\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 1\n", reg3);
					printf("_%d:\n",label);
					break;
				case BINARY_OP_GT://not support
					printf("\t c.le.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 0\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 1\n", reg3);
					printf("_%d:\n",label);
					break;
				case BINARY_OP_LT://not support
					printf("\t c.lt.s   $f%d, $f%d\n", reg1, reg2);
					node->dataType = INT_TYPE;
					reg3 = get_reg();
					printf("\t li   $%d, 1\n", reg3);
					label = get_label();
					printf("\t bc1t   _%d\n",label);
					printf("\t li   $%d, 0\n", reg3);
					printf("_%d:\n",label);
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
		else 
			switch(node->semantic_value.exprSemanticValue.op.unaryOp)
			{
				case UNARY_OP_POSITIVE:
					reg3 = reg1;
					break;
				case UNARY_OP_NEGATIVE:
					printf("\t neg.s   $f%d, $f%d\n", reg3, reg1);
					break;
				case UNARY_OP_LOGICAL_NEGATION:
					printf("\t not   $f%d, $f%d\n", reg3, reg1);
					break;
				default:
					printf("This operater is not supported now.\n");
					break;
			}
	}
	
	node->regplace = reg3;
}

void visit_assignment(AST_NODE* node)
{
	int offset,reg;
	AST_NODE *lchild, *rchild;
	lchild = node->child;
	rchild = lchild->rightSibling;
	IdentifierSemanticValue IdValue = lchild->semantic_value.identifierSemanticValue;
	code_gen(rchild);
	reg = rchild->regplace;
	if(IdValue.symbolTableEntry->nestingLevel > 0)
	{
		offset = get_offset(IdValue);
		printf("\t la   $v1, %d($fp)\n", offset);
	}
	else
		printf("\t la   $v1, _globalsym_%s\n", IdValue.identifierName);
	if(IdValue.kind == ARRAY_ID)
	{
		AST_NODE* dimnode = lchild->child;
		code_gen(dimnode);
		int reg2 = dimnode->regplace;
		printf("\t mul  $%d, $%d, 4\n", reg2, reg2);
		if(IdValue.symbolTableEntry->nestingLevel > 0) printf("\t neg  $%d, $%d\n", reg2, reg2);
		printf("\t add  $v1, $v1, $%d\n", reg2);
	}
	if(rchild->dataType == FLOAT_TYPE) 
		printf("\t s.s   $f%d, 0($v1)\n",reg);
	else 
		printf("\t sw   $%d, 0($v1)\n",reg);
	/*if(entry->nestingLevel > 0)//local
	{
		offset = get_offset(lchild->semantic_value.identifierSemanticValue);
		if(rchild->dataType == FLOAT_TYPE) printf("\t s.s   $f%d, %d($fp)\n", reg, offset);
		else printf("\t sw   $%d, %d($fp)\n", reg, offset);
	}
	else//global
	{
		printf("\t la   $v1, _globalsym_%s\n",lchild->semantic_value.identifierSemanticValue.identifierName);
		if(rchild->dataType == FLOAT_TYPE) printf("\t s.s   $f%d, 0($v1)\n",reg);
		else printf("\t sw   $%d, 0($v1)\n",reg);
	}*/
}

void visit_WhileStmt(AST_NODE* node)
{
	AST_NODE* testnode = node->child;
	AST_NODE* stmtnode = testnode->rightSibling;
	int label = get_whilelabel();
	printf("_Wtest%d:\n", label);
	code_gen(testnode);
	int reg = testnode->regplace;
	printf("\t beqz   $%d, _Wexit%d\n", reg, label);
	code_gen(stmtnode);
	printf("\t j   _Wtest%d\n", label);
	printf("_Wexit%d:\n", label);
}

void visit_ForStmt(AST_NODE* node)
{

}

void visit_IfStmt(AST_NODE* node)
{
	AST_NODE* testnode = node->child;
	AST_NODE* ifnode = testnode->rightSibling;
	AST_NODE* elsenode = ifnode->rightSibling;
	code_gen(testnode);
	int reg = testnode->regplace;
	int label = get_iflabel();
	printf("\t beqz   $%d, _Lelse%d\n", reg, label);
	code_gen(ifnode);
	printf("\t j   _Lexit%d\n",label);
	printf("_Lelse%d:\n", label);
	code_gen(elsenode);
	printf("_Lexit%d:\n", label);
}

void visit_FunctionCall(AST_NODE* node)
{
	AST_NODE* FuncIdNode = node->child;
	if(strcmp(FuncIdNode->semantic_value.identifierSemanticValue.identifierName,"read") == 0)
	{
		visit_read(node);
		return;
	}
	if(strcmp(FuncIdNode->semantic_value.identifierSemanticValue.identifierName,"fread") == 0)
	{
		visit_fread(node);
		return;
	}
	if(strcmp(FuncIdNode->semantic_value.identifierSemanticValue.identifierName,"write") == 0)
	{
		visit_write(node);
		return;
	}
	printf("\t jal %s\n",FuncIdNode->semantic_value.identifierSemanticValue.identifierName);
	if(node->dataType == INT_TYPE)
	{
		node->regplace = get_reg();
		printf("\t move   $%d, $v0\n",node->regplace);
	}
	else
	if(node->dataType == FLOAT_TYPE)
	{
		node->regplace = get_freg();
		printf("\t mov.s   $f%d, $f0\n",node->regplace);
	}
}

void visit_ReturnStmt(AST_NODE* node)
{
	AST_NODE* child = node->child;
	if(child->nodeType != NUL_NODE)
	{
		code_gen(child);
		int reg = child->regplace;
		if(child->dataType == INT_TYPE)
			printf("\t move   $v0, $%d\n", reg);
		else
			printf("\t mov.s   $f0, $f%d\n", reg);
	}
	printf("\t j   _end_%s\n",FuncNameNow);
}

void visit_read(AST_NODE* node)
{
	printf("\t li   $v0, 5\n\t syscall\n");
	node->regplace = get_reg();
	printf("\t move   $%d, $v0\n",node->regplace);
}

void visit_fread(AST_NODE* node)
{
	printf("\t li   $v0, 6\n\t syscall\n");
	node->regplace = get_freg();
	printf("\t mov.s   $f%d, $f0\n",node->regplace);
}

void visit_write(AST_NODE* node)
{
	AST_NODE* paranode = node->child->rightSibling->child;
	code_gen(paranode);
	if(paranode->nodeType == CONST_VALUE_NODE && paranode->semantic_value.const1->const_type == STRINGC)
	{
		int labelnum = paranode->regplace;
		printf("\t li   $v0, 4\n\t la   $a0, _str%d\n\t syscall\n", labelnum);
		return ;
	}
	int reg = paranode->regplace;
	if(paranode->dataType == FLOAT_TYPE) printf("\t li   $v0, 2\n\t mov.s   $f12, $f%d\n\t syscall\n",reg);
	else printf("\t li   $v0, 1\n\t move   $a0, $%d\n\t syscall\n",reg);
}
