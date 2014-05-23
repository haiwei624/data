#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// This file is for reference only, you are not required to follow the implementation. //

int HASH(char * str) {
	int idx=0;
	while (*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}
	return (idx & (HASH_TABLE_SIZE-1));
}

SymbolTable symbolTable;

SymbolTableEntry* newSymbolTableEntry(char* name,int nestingLevel)//include the init of the name
{
    SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    symbolTableEntry->nextInHashChain = NULL;
    symbolTableEntry->prevInHashChain = NULL;
    symbolTableEntry->nextInSameLevel = NULL;
    symbolTableEntry->sameNameInOuterLevel = NULL;
    symbolTableEntry->attribute = NULL;
    int length=0;
    while(name[length]) length++;
    symbolTableEntry->name = (char *)malloc((length+1) * sizeof(char));//need to be careful
    symbolTableEntry->nestingLevel = nestingLevel;
    return symbolTableEntry;
}

void removeFromHashTrain(SymbolTableEntry* sym)//int hashIndex can be calculated from entry
{
    int hash = HASH(sym.name);
    SymbolTableEntry* nextsym = sym.nextInHashChain;
    SymbolTableEntry* prevsym = sym.prevInHashChain;
    if(nextsym != NULL)
        nextsym.prevInHashChain = prevsym;
    if(prevsym != NULL) 
        prevsym.nextInHashChain = nextsym;
    if(symbolTable.hashTable[hash] == sym)
        symbolTable.hashTable[hash] = nextsym;
    sym.nextInHashChain = NULL;
    sym.prevInHashChain = NULL;
}

void enterIntoHashTrain(SymbolTableEntry* sym)
{
    int hash = HASH(sym.name);
    SymbolTableEntry* oldsym = symbolTable.hashTable[hash];
    sym.nextInHashChain = oldsym;
    oldsym.prevInHashChain = sym;
    symbolTable.hashTable[hash] = sym;

    sym.prevInHashChain = NULL;
}

void initializeSymbolTable()
{
    memset(symbolTable.hashTable, 0, sizeof(symbolTable.hashTable));
    memset(symbolTable.scopeDisplay, 0, sizeof(symbolTable.scopeDisplay));
    symbolTable.currentLevel = 0;
    sybolTable.scopeDisplay[0] = NULL;
    //symbolTable.scopeDisplayElementCount=1;
    //I don't know what the usage of scopeDisplayElementCount is.
}

void symbolTableEnd()
{
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
    int hash = HASH(symbolName);
    SymbolTableEntry* sym = symbolTable.hashTable[hash];
    while(sym != NULL)
    {
        if(strcmp(symbolName, sym->name) == 0)
            return sym;
        sym = sym->nextInHashChain;
    }
    return NULL;
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute)
{
    SymbolTableEntry* oldsym = retrieveSymbol(symbolName);
    if(oldsym != NULL && oldsym.nestingLevel == sybolTable.currentLevel)
    {
        printf("Error: %s has been redecalred!");
        g_anyErrorOccur = 1;
        return oldsym;
    }
    //new entry
    SymbolTableEntry* newsym = newSymbolTableEntry(symbolName, sybolTable.currentLevel);
    newsym.attribute = attribute;//the space of attribute may be changed,need to be careful.
    //add to scope display
    newsym.nextInSameLevel = symbolTable.scopeDisplay[symbolTable.currentLevel];
    symbolTable.scopeDisplay[symbolTable.currentLevel] = newsym;
    //add to hash table
    if(oldsym == NULL) enterIntoHashTrain(newsym);
    else
    {
        removeFromHashTrain(oldsym);
        enterIntoHashTrain(newsym);
    }
    newsym.sameNameInOuterLevel = oldsym;
}

//remove the symbol from the current scope
void removeSymbol(char* symbolName)
{
    //this function will be programed when needed.
}

int declaredLocally(char* symbolName)
{
    //this function will be programed when needed.

    symbolTableEntry* sym = retrieveSymbol(symbolName);
    if(sym == NULL)return 0;
    return (sym.nestingLevel == symbolTable.currentLevel);
}

void openScope()
{
    sybolTable.currentLevel++;
    sybolTable.scopeDisplay[symbolTable.currentLevel]=NULL;
}

void closeScope()
{
    SymbolTableEntry* sym;
    sym = sybolTable.scopeDisplay[sybolTable.currentLevel];
    while(sym!=NULL)
    {
        SymbolTableEntry* nextInSameLevel = sym.nextInSameLevel;
        SymbolTableEntry* prevsym = sym.

        removeFromHashTrain(sym);
        if(prevsym != NULL) enterIntoHashTrain(prevsym);                

        sym=nextInSameLevel;
    }
    symbolTable.currentLevel--;
}
