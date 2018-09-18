#ifndef _RENPSM_COMPILER_H_
#define _RENPSM_COMPILER_H_

#include <stdlib.h>
#include <string.h>

#include "y.tab.h" 

#define TABS 2

#define MAX_DEFINITIONS 64
#define MAX_PARAMS 64
#define MAX_INSTRUCTIONS 2048
#define MAX_INDEXES 8
#define MAX_ITERATORS 8
#define MAX_ARGUMENTS 64
#define MAX_MEMBRANES 64
#define MAX_VARIABLES 1024


extern int yylineno;

struct Arguments;
typedef struct Expr
{
	int type;
	char* id;
	struct Arguments* arguments;
	int intValue;
	double doubleValue;
	struct Expr* left;
	struct Expr* right;
	
} EXPR;


EXPR variables[MAX_VARIABLES];
int variables_size = 0;

typedef struct Iterator
{
	int type;
	char* id;
	EXPR* left;
	EXPR* right;
} ITERATOR;

typedef struct Iterators
{
	int size;
	ITERATOR* iterators[MAX_ITERATORS];
} ITERATORS;


typedef struct Arguments
{
	int size;
	EXPR* args[MAX_ARGUMENTS];
	ITERATORS* iterators;
} ARGUMENTS;

typedef struct Membrane
{
	EXPR* label;
	int size;
	struct Membrane* membranes[MAX_MEMBRANES];
} MEMBRANE;


typedef struct Instruction
{
	int type;
	char* id;
	EXPR* object;
	EXPR* expr;
	EXPR* protein;
	EXPR* enzyme;
	EXPR* label;
	MEMBRANE* mu;
	ARGUMENTS* arguments;
	ITERATORS* iterators;
} INSTRUCTION;




typedef struct Params
{
	int size;
	char* params[MAX_PARAMS];
} PARAMS;


typedef struct Definition
{
	char* id;
	PARAMS* params;
	int size;
	INSTRUCTION* instructions[MAX_INSTRUCTIONS];
	
} DEFINITION;


typedef struct Definitions
{
	char* model;
	int size;
	DEFINITION* definitions[MAX_DEFINITIONS];
} DEFINITIONS;

void printExpr(FILE* fp, EXPR* expr, int tabs);



EXPR *getVariable(char* name)
{
	for (int i=0;i<variables_size;i++) {
		if (strcmp(variables[i].id,name)==0) {
			return &variables[i];
		}
	}
	return NULL;
}

void delVariable(char* name)
{
	for (int i=0;i<variables_size;i++) {
		if (strcmp(variables[i].id,name)==0) {
			strcpy(variables[i].id,"0");
			return ;
		}
	}
	
}

EXPR *setIntVariable(char* name, int value)
{
	EXPR* var = getVariable(name);
	if (var==NULL) {
		var = &variables[variables_size++];
		var->id = strdup(name);
	}
	var->type = INTEGER;
	var->intValue = value;
	return var;
}


EXPR *setDoubleVariable(char* name, double value)
{
	EXPR* var = getVariable(name);
	if (var==NULL) {
		var = &variables[variables_size++];
		var->id = strdup(name);
	}
	var->type = REAL;
	var->doubleValue = value;
	return var;
}

DEFINITIONS* createDefinitions(DEFINITION* definition)
{
	DEFINITIONS* definitions = (DEFINITIONS*)malloc(sizeof(DEFINITIONS));
	definitions->size=1;
	definitions->definitions[0] = definition;
	return definitions;
}

DEFINITIONS* addDefinition(DEFINITIONS* definitions, DEFINITION* definition)
{
	if (definitions->size==MAX_DEFINITIONS) {
		fprintf(stderr,"Line %d: Too much definitions\n",yylineno);
		exit(1);
	}
	definitions->definitions[definitions->size++] = definition;
	return definitions;
}


DEFINITION* createDefinition(char* id, PARAMS* params, DEFINITION* definition)
{
	definition->id = id;
	definition->params = params;
	return definition;
}


PARAMS* createEmptyParams()
{
	PARAMS* params = (PARAMS*)malloc(sizeof(PARAMS));
	params->size=0;
	return params;
}

PARAMS* createParams(char* param)
{
	PARAMS* params = (PARAMS*)malloc(sizeof(PARAMS));
	params->params[0] = param;
	params->size=1;
	return params;
}

MEMBRANE* addInnerMembrane(MEMBRANE* container, MEMBRANE* membrane)
{
	if (container->size==MAX_MEMBRANES) {
		fprintf(stderr,"Line: %d: Too much membranes\n",yylineno);
		exit(0);
	}
	container->membranes[container->size++] = membrane;
	return container;
}

MEMBRANE* createMembrane(MEMBRANE* inner, EXPR* label)
{
	
	if (inner==NULL) {
		MEMBRANE* membrane = (MEMBRANE*)malloc(sizeof(MEMBRANE));
		membrane->size=0;
		membrane->label=label;
		return membrane;
	} 
	inner->label = label;
	return inner;

}

INSTRUCTION* createMu(MEMBRANE* membrane)
{
	INSTRUCTION* mu = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	mu->type = MU;
	mu->mu = membrane;
	return mu;
	
}

MEMBRANE* createInnerMembrane(MEMBRANE* inner)
{
	MEMBRANE* membrane = (MEMBRANE*)malloc(sizeof(MEMBRANE));
	membrane->size=1;
	membrane->membranes[0] = inner;
	return membrane;
	
}

PARAMS* addParam(PARAMS* params, char* param)
{
	if (params->size==MAX_PARAMS) {
		fprintf(stderr,"Line: %d: Too much params\n",yylineno);
		exit(1);
	}
	params->params[params->size++] = param;
	return params;
}

DEFINITIONS* createCode(char* model, DEFINITIONS* code)
{
	code->model = model;
	return code;
}

DEFINITION* createInstructions(INSTRUCTION* instruction)
{
	DEFINITION* instructions = (DEFINITION*)malloc(sizeof(DEFINITION));
	instructions->size = 1;
	instructions->instructions[0] = instruction;
	return instructions;
}

DEFINITION* addInstruction(DEFINITION* instructions, INSTRUCTION* instruction)
{
	if (instructions->size==MAX_INSTRUCTIONS) {
		fprintf(stderr,"Line: %d: Too much instructions\n",yylineno);
		exit(1);
	}
	instructions->instructions[instructions->size++] = instruction;
	return instructions;
}

INSTRUCTION* createInstruction()
{
	return (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
}

INSTRUCTION* createCall(char* id, ARGUMENTS* arguments)
{
	INSTRUCTION* call = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	call->type = CALL;
	call->id = id;
	call->arguments = arguments;
	return call;
}

INSTRUCTION* createAsigInst(EXPR* obj, EXPR* expr, int initModelVariable)
{
	INSTRUCTION* asig = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	asig->type = initModelVariable? INIT_VARIABLE: ASIG;
	asig->object = obj;
	asig->expr = expr;
	if (asig->type == ASIG && expr->type==REAL) {
		setDoubleVariable(obj->id,expr->doubleValue);
	} else if (asig->type==ASIG &&expr->type==INTEGER) {
		setIntVariable(obj->id,expr->intValue);
	}
	return asig;
}

INSTRUCTION* createProductionRule(EXPR* obj, EXPR* expr, EXPR* protein, EXPR* enzyme)
{
	INSTRUCTION* rule = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	rule->type = PRODUCTION_RULE;
	rule->object = obj;
	rule->expr = expr;
	rule->protein = protein;
	rule->enzyme = enzyme;
	return rule;
	
}


INSTRUCTION* createCreationRule(EXPR* inner_label, EXPR* outer_label, EXPR* protein, EXPR* enzyme)
{
	INSTRUCTION* rule = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	rule->type = CREATION_RULE;
	rule->object = inner_label;
	rule->expr = outer_label;
	rule->protein = protein;
	rule->enzyme = enzyme;
	return rule;
	
}

INSTRUCTION* createEvolutionRule(EXPR* obj0, EXPR* obj1, EXPR* label, EXPR* protein, EXPR* enzyme)
{
	INSTRUCTION* rule = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	rule->type = EVOLUTION_RULE;
	rule->object = obj0;
	rule->expr = obj1;
	rule->label = label;
	rule->protein = protein;
	rule->enzyme = enzyme;
	return rule;
	
}


INSTRUCTION* createMultisetInst(EXPR* label, ARGUMENTS* arguments)
{
	INSTRUCTION* ms = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	ms->type = MS;
	ms->object = label;
	ms->arguments = arguments;
}

int isNumeric(EXPR* expr) {
	return expr==NULL || expr->type==REAL || expr->type==INTEGER || 
	(expr->type==OBJECT && getVariable(expr->id)!=NULL) ||
	(expr->type==ID && getVariable(expr->id)!=NULL);
}

EXPR* reduceInteger(int type, int op1, int op2) {
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	expr->type = INTEGER;
	
	switch(type) {
		case ADD:
			expr->intValue = op1+op2;
		break;
		case SUB:
			expr->intValue = op1-op2;
		break;
		case MUL:
			expr->intValue = op1*op2;
		break;
		case DIV:
			expr->intValue = op1 / op2;
		break;
		case MOD:
			expr->intValue = op1 % op2;
		break;
		case EQ:
			expr->intValue = op1 == op2;
		break;
		case NEQ:
			expr->intValue = op1 != op2;
		break;
		case LE:
			expr->intValue = op1 <= op2;
		break;
		case LT:
			expr->intValue = op1 < op2;
		break;
		case GE:
			expr->intValue = op1 >= op2;
		break;
		case GT:
			expr->intValue = op1 > op2;
		break;
		case NOT:
			expr->intValue = !op2;
		break;
		case AND:
			expr->intValue = op1 && op2;
		break;
		case OR:
			expr->intValue = op1 || op2;
		break;
		default:
		;
	}
	
}

EXPR* reduceDouble(int type, double op1, double op2) {
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	expr->type = REAL;
	
	switch(type) {
		case ADD:
			expr->doubleValue = op1+op2;
		break;
		case SUB:
			expr->doubleValue = op1-op2;
		break;
		case MUL:
			expr->doubleValue = op1*op2;
		break;
		case DIV:
			expr->doubleValue = op1 / op2;
		break;
		case MOD:
			expr->doubleValue = 0;
		break;
		case EQ:
			expr->type = INTEGER;
			expr->intValue = op1 == op2;
		break;
		case NEQ:
			expr->type = INTEGER;
			expr->intValue = op1 != op2;
		break;
		case LE:
			expr->type = INTEGER;
			expr->intValue = op1 <= op2;
		break;
		case LT:
			expr->type = INTEGER;
			expr->intValue = op1 < op2;
		break;
		case GE:
			expr->type = INTEGER;
			expr->intValue = op1 >= op2;
		break;
		case GT:
			expr->type = INTEGER;
			expr->intValue = op1 > op2;
		break;
		case NOT:
			expr->type = INTEGER;
			expr->intValue = !op2;
		break;
		case AND:
			expr->type = INTEGER;
			expr->intValue = op1 && op2;
		break;
		case OR:
			expr->type = INTEGER;
			expr->intValue = op1 || op2;
		break;
		default:
		;
	}
	
}


EXPR* reduce(int type, EXPR* op1, EXPR* op2) {
	EXPR* expr;
	
	if (op1->type==OBJECT || op1->type==ID) {
		op1 = getVariable(op1->id);
	}
	if (op2->type==OBJECT || op2->type==ID) {
		op2 = getVariable(op2->id);
	}
	
	if (op1==NULL && op2->type==REAL) {
		expr = reduceDouble(type,0.0,op2->doubleValue);
	} else if (op1==NULL && op2->type==INTEGER) {
		expr = reduceInteger(type,0,op2->intValue);
	} else if (op2==NULL && op1->type==REAL) {
		expr = reduceDouble(type,op1->doubleValue,0.0);
	} else if (op2==NULL && op1->type==INTEGER) {
		expr = reduceInteger(type,op1->intValue,0);
	} else if (op1->type==REAL && op2->type==INTEGER) {
		expr = reduceDouble(type,op1->doubleValue,(double)(op2->intValue));
	} else if (op1->type==INTEGER && op2->type==REAL) {
		expr = reduceDouble(type,(double)(op1->intValue),op2->doubleValue);
	} else if (op1->type==REAL) {
		expr = reduceDouble(type,op1->doubleValue,op2->doubleValue);
	} else {
		expr = reduceInteger(type,op1->intValue,op2->intValue);
	}
	
	return expr;
	
	
}

EXPR* createExpr(int type, EXPR* op1, EXPR* op2)
{
	EXPR* expr;	
	if (isNumeric(op1) && isNumeric(op2)) {
		expr = reduce(type,op1,op2);
	} else {
		expr = (EXPR*)malloc(sizeof(EXPR));
		expr->type = type;
		expr->left = op1;
		expr->right = op2;
	}
	return expr;
}

EXPR* createId(char* id)
{
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	EXPR* value = getVariable(id); 
	if (value!=NULL) {
		expr->type=value->type;
		expr->id = id;
		if (value->type==INTEGER) {
			expr->intValue = value->intValue;
		} else {
			expr->doubleValue = value->doubleValue;
		}
	} else {
		expr->type = ID;
		expr->id = id;
		expr->left=NULL;
	}
	return expr;
	
}


EXPR* createInteger(int value)
{
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	expr->type = INTEGER;
	expr->intValue = value;
	return expr;
}

EXPR* createReal(double value)
{
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	expr->type = REAL;
	expr->doubleValue = value;
	return expr;
}

EXPR* createObject(char* id, ARGUMENTS* indexes)
{
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	EXPR* value = indexes==NULL?getVariable(id):NULL; 
	if (value!=NULL) {
		expr->type=value->type;
		expr->id = id;
		if (value->type==INTEGER) {
			expr->intValue = value->intValue;
		} else {
			expr->doubleValue = value->doubleValue;
		}
	} else {
		expr->type = OBJECT;
		expr->id = id;
		expr->left=NULL;
		expr->arguments = indexes;
	}
	return expr;
}

EXPR* addMultiplicity(EXPR* object,EXPR* multiplicity)
{
	object->left = multiplicity;
	return object;
}



EXPR* createFunction(char* id, ARGUMENTS* arguments)
{
	EXPR* expr = (EXPR*)malloc(sizeof(EXPR));
	expr->type = FUNCTION;
	expr->id = id;
	expr->arguments = arguments;
	return expr;
}

ITERATORS* createEmptyIterators()
{
	ITERATORS* iterators = (ITERATORS*)malloc(sizeof(ITERATORS));
	iterators->size = 0;
	return iterators;
}

ITERATORS* createIterators(ITERATOR* iterator)
{
	ITERATORS* iterators = (ITERATORS*)malloc(sizeof(ITERATORS));
	iterators->size = 1;
	iterators->iterators[0] = iterator;
	return iterators;
}

ITERATORS* addIterator(ITERATORS* iterators, ITERATOR* iterator)
{
	if (iterators->size==MAX_ITERATORS) {
		fprintf(stderr,"Line: %d: Too much iterators\n",yylineno);
		exit(1);
	}
	iterators->iterators[iterators->size++] = iterator;
	return iterators;
}


INSTRUCTION* addIterators(INSTRUCTION* instruction, ITERATORS* iterators)
{
	instruction->iterators = iterators;
}

ITERATOR* createSetIterator(char* id, EXPR* expr)
{
	ITERATOR* iterator = (ITERATOR*)malloc(sizeof(ITERATOR));
	iterator->type = SET_ITERATOR;
	iterator->id = id;
	iterator->left = expr;
	iterator->right = NULL;
	return iterator;
}

ITERATOR* createRangeIterator(char* id, EXPR* left, EXPR* right)
{
	ITERATOR* iterator = (ITERATOR*)malloc(sizeof(ITERATOR));
	iterator->type = RANGE_ITERATOR;
	iterator->id = id;
	iterator->left = left;
	iterator->right = right;
	return iterator;
	
}

ARGUMENTS* createEmptyArguments()
{
	ARGUMENTS* args = (ARGUMENTS*)malloc(sizeof(ARGUMENTS));
	args->size=0;
	args->iterators = NULL;
	return args;
}

ARGUMENTS* createArguments(EXPR* expr)
{
	ARGUMENTS* args = (ARGUMENTS*)malloc(sizeof(ARGUMENTS));
	args->size=1;
	args->args[0] = expr;
	args->iterators = NULL;
	return args;
}

ARGUMENTS* createIteratorArguments(EXPR* expr, ITERATORS* iterators)
{
	ARGUMENTS* args = (ARGUMENTS*)malloc(sizeof(ARGUMENTS));
	args->size=1;
	args->args[0] = expr;
	args->iterators = iterators;
	return args;
}


ARGUMENTS* addArgument(ARGUMENTS* args, EXPR* expr)
{
	if (args->size==MAX_ARGUMENTS) {
		fprintf(stderr,"Line: %d: Too much arguments\n",yylineno);
		exit(1);
	}
	args->args[args->size++]=expr;
	return args;
}

void printTabs(FILE* fp,int tabs)
{
	for (int i=0;i<tabs;i++) {
		fprintf(fp," ");
	}
}


void printParams(FILE* fp,PARAMS* params)
{
	for (int i=0;i<params->size;i++) {
		fprintf(fp,"%s",params->params[i]);
		if (i!=params->size-1) {
			fprintf(fp,",");
		}
	}
}

void printType(FILE* fp,int type)
{
	switch(type) {
		case ADD:
			fprintf(fp,"+");
		break;
		case SUB:
			fprintf(fp,"-");
		break;
		case MUL:
			fprintf(fp,"*");
		break;
		case DIV:
			fprintf(fp,"/");
		break;
		case MOD:
			fprintf(fp,"%%");
		break;
		case LT:
			fprintf(fp,"<");
		break;
		case GT:
			fprintf(fp,">");
		break;
		case EQ:
			fprintf(fp,"==");
		break;
		case NEQ:
			fprintf(fp,"!=");
		break;
		case LE:
			fprintf(fp,"<=");
		break;
		case GE:
			fprintf(fp,">=");
		break;
		case NOT:
			fprintf(fp,"!");
		break;
		case AND:
			fprintf(fp,"&&");
		break;
		case OR:
			fprintf(fp,"||");
		break;
		default:
			;
	}
	
	
}

void printArguments(FILE* fp,ARGUMENTS* args, char init, char end, int tabs)
{
	if (args!=NULL && args->size>0) {
		printTabs(fp,tabs);
		fprintf(fp,"%c",init);
		printExpr(fp,args->args[0],0);
		for (int i=1;i<args->size;i++) {
			fprintf(fp,",");
			printExpr(fp,args->args[i],0);
		}
		fprintf(fp,"%c",end);
	}
}

void printIterator(FILE* fp,ITERATOR* iterator)
{
	if (iterator->type==RANGE_ITERATOR) {
		printExpr(fp,iterator->left,0);
		fprintf(fp," <= %s <= ",iterator->id);
		printExpr(fp,iterator->right,0);
	} else {
		fprintf(fp," %s in ",iterator->id);
		printExpr(fp,iterator->left,0);
	}
	
	
}


void printIterators(FILE* fp, ITERATORS* iterators)
{
	if (iterators!=NULL && iterators->size>0) {
		fprintf(fp," : ");
		printIterator(fp,iterators->iterators[0]);
		for (int i=1;i<iterators->size;i++) {
			fprintf(fp,", ");
			printIterator(fp,iterators->iterators[1]);
		}
	}
}

void printExpr(FILE* fp, EXPR* expr, int tabs)
{
	printTabs(fp,tabs);
	switch(expr->type) {
		case INTEGER:
			fprintf(fp,"%d",expr->intValue);
			break;
		case REAL:
			fprintf(fp,"%f",expr->doubleValue);
			break;
		case OBJECT:
		case ID:
			fprintf(fp,"%s",expr->id);
			printArguments(fp,expr->arguments,'{','}',0);
			if (expr->intValue>1) {
				fprintf(fp," * %d",expr->intValue);
			}
		break;
		case FUNCTION:
			fprintf(fp,"%s",expr->id);
			printArguments(fp,expr->arguments,'(',')',0);
		break;
		case ADD:case SUB:case MUL:case DIV:
		case MOD:case AND:case OR:case LE:
		case GE:case LT:case GT:case EQ:
		case NEQ:
			fprintf(fp,"(");
			printType(fp,expr->type);
			if (expr->left!=NULL) {
				fprintf(fp," ");
				printExpr(fp,expr->left,0);
			}
			fprintf(fp," ");
			printExpr(fp,expr->right,0);
			fprintf(fp,")");
		break;
		default:
		;
		
	}
	
	
}



void printMembrane(FILE* fp, MEMBRANE* membrane,int tabs)
{
	printTabs(fp,tabs);
	fprintf(fp," [");
	for (int i=0;i<membrane->size;i++) {
		printMembrane(fp,membrane->membranes[i],0);
	}
	fprintf(fp," ]'");
	printExpr(fp,membrane->label,0);
}

void printRule(FILE* fp, INSTRUCTION* instruction)
{
	switch(instruction->type) {
		case PRODUCTION_RULE:
			printExpr(fp,instruction->object,0);
			fprintf(fp," <- ");
			printExpr(fp,instruction->expr,0);
			
		break;
		case CREATION_RULE:
			fprintf(fp,"[ [ ]'");
			printExpr(fp,instruction->object,0);
			fprintf(fp," ]'");
			printExpr(fp,instruction->expr,0);
		break;	
		case EVOLUTION_RULE:
			fprintf(fp,"[");
			printExpr(fp,instruction->object,0);
			fprintf(fp," -> ");
			printExpr(fp,instruction->expr,0);
			fprintf(fp,"]'");
			printExpr(fp,instruction->label,0);
		break;
		
		default:
		;	
		
	}
	if (instruction->protein!=NULL) {
		fprintf(fp,", ");
		printExpr(fp,instruction->protein,0);
	}
	if (instruction->enzyme!=NULL) {
		fprintf(fp," ?");
		printExpr(fp,instruction->enzyme,0);
	}
}


void printInstruction(FILE* fp, INSTRUCTION* instruction, int tabs)
{
	printTabs(fp,tabs);
	switch(instruction->type) {
		case ASIG:
		case INIT_VARIABLE:
			fprintf(fp,"%s",instruction->type==INIT_VARIABLE?"@":"");
			printExpr(fp,instruction->object,0);
			fprintf(fp," = ");
			printExpr(fp,instruction->expr,0);
			break;
		case CALL:
			fprintf(fp,"call ");
			fprintf(fp,"%s",instruction->id);
			printArguments(fp,instruction->arguments,'(',')',0);
			break;
		case MU:
			fprintf(fp,"@mu =");
			printMembrane(fp,instruction->mu,0);
			break;
		case MS:
			fprintf(fp,"@ms(");
			printExpr(fp,instruction->object,0);
			fprintf(fp,") = ");
			printArguments(fp,instruction->arguments,0,0,0);
		break;
		case PRODUCTION_RULE:
		case CREATION_RULE:
		case EVOLUTION_RULE:
			printRule(fp,instruction);
		break;
		default:
			;
	}
	printIterators(fp,instruction->iterators);

}

void printDefinition(FILE* fp, DEFINITION* def)
{
	fprintf(fp,"\n%s(",def->id);
	printParams(fp,def->params);
	fprintf(fp,")\n");
	for (int i=0;i<def->size;i++) {
		printInstruction(fp,def->instructions[i],TABS);
		fprintf(fp,"\n");
	}
}





void printTree(FILE* fp, DEFINITIONS* code)
{
	for (int i=0;i<code->size;i++) {
		printDefinition(fp,code->definitions[i]);
	}
}

DEFINITION *searchDefinition(DEFINITIONS* definitions, char* id)
{
	for (int i = 0;i<definitions->size;i++) {
		if (strcmp(definitions->definitions[i]->id,id)==0) {
			return definitions->definitions[i];
		}
	}
	return NULL;
}
EXPR* unrollExpr(EXPR* expr);

ARGUMENTS* unrollArguments(ARGUMENTS* arguments)
{
	if (arguments==NULL) {
		return NULL;
	}
	
	ARGUMENTS *args = createEmptyArguments();
	for (int i=0;i<arguments->size;i++) {
		addArgument(args,unrollExpr(arguments->args[i]));
	}

	return args;
	
}

EXPR* unrollExpr(EXPR* expr)
{
	if (expr==NULL) {
		return expr;
	}
	switch(expr->type) {
		case FUNCTION:
			return createFunction(expr->id,unrollArguments(expr->arguments));
		case OBJECT:
			return createObject(expr->id, unrollArguments(expr->arguments));
		case ID: case INTEGER: case REAL:
			return expr;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case MOD:
		case NOT:
		case AND:
		case OR:
		case GE:
		case GT:
		case LE:
		case LT:
		case EQ:
		case NEQ:
		 return createExpr(expr->type,unrollExpr(expr->left),unrollExpr(expr->right));
	}
	
}


INSTRUCTION* unrollInst(INSTRUCTION* instruction)
{
	INSTRUCTION* inst = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
	
	inst->type = instruction->type;
	inst->id = instruction->id;
	
	inst->object = unrollExpr(instruction->object);
	inst->expr = unrollExpr(instruction->expr);
	inst->protein = unrollExpr(instruction->protein);
	inst->enzyme = unrollExpr(instruction->enzyme);
	inst->label = unrollExpr(instruction->label);
	
	
	inst->mu=NULL;
	inst->arguments=NULL;
	
	inst->iterators = createEmptyIterators();
	for (int i=0;i<instruction->iterators->size-1;i++) {
		addIterator(inst->iterators,instruction->iterators->iterators[i]);
		
	}
	
	return inst;
	
}

void rollOutDefinition(DEFINITIONS* code, DEFINITION* def, DEFINITION* def1)
{
	char id[8];
	strcpy(id,"i");
	for (int i=0;i<def->size;i++) {
		INSTRUCTION *inst = def->instructions[i];
		int init=1;
		int end=1;
		
		ITERATORS *its = inst->iterators;
		if (its->size>0) {
			ITERATOR* it = its->iterators[its->size-1];
			if (it->type == RANGE_ITERATOR) {
				init = it->left->intValue;
				end = it->right->intValue;
				strcpy(id,it->id);
			}
		}
		for (int j=init;j<=end;j++) {
			if (init!=end) {
				setIntVariable(id,j);
			}
			switch (inst->type) {
			case CALL:
				rollOutDefinition(code,searchDefinition(code,inst->id),def1);
			break;
			case ASIG:
			break;
			default:
				if (init==end) {
					def1->instructions[def1->size++] = inst;
				} else {
					def1->instructions[def1->size++] = unrollInst(inst);
				}
				
				;
			}
			if (init!=end) {
				delVariable(id);
			}
		}
		
	}
} 

DEFINITIONS* rollOut(DEFINITIONS* code)
{
	DEFINITIONS* code1 = (DEFINITIONS*) malloc(sizeof(DEFINITIONS));
	code1->model = code->model;
	code1->size = 1;
	code1->definitions[0] = (DEFINITION*)malloc(sizeof(DEFINITION));
	code1->definitions[0]->size=0;
	DEFINITION* main = searchDefinition(code,"main");
	code1->definitions[0]->id = main->id;
	code1->definitions[0]->params = createEmptyParams();
	rollOutDefinition(code,main,code1->definitions[0]);
	return code1;
}


#endif
