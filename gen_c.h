/* 
 * gen_c.h:
 *
 * This file contains function for generation of ad-hoc RENPSM simulators.
 * 
 * More information can be found in:
 * 
 * I. Perez-Hurtado, G. Zang, M.J. Perez-Jimenez, D. Orellana
 * Simulation of Rapidly-Exploring Random Trees in Membrane Computing 
 * with P-Lingua and Automatic Programing
 * International Journal of Computers, Communications and Control, in press.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Copyright (C) 2018  Ignacio Perez-Hurtado (perezh@us.es)
 *                     Research Group On Natural Computing
 *                     http://www.gcn.us.es
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef _GEN_C_H_
#define _GEN_C_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "renpsm_parser.h"

#define SIM_MAX_MEMBRANES 524288

#define SIM_MAX_VARS 1024
#define SIM_MAX_ITERS 1024*1024

char *masks[8] = {"0x01000000","0x02000000","0x04000000","0x08000000","0x10000000","0x20000000","0x40000000","0x80000000"}; 

typedef struct Var
{
	char name[64];
	int indexes;
	int limits[64];
} VAR;

VAR vars[SIM_MAX_VARS];
int vars_count = 0;

int functions=0;

int labels[8];
int labels_count=0;

VAR* searchVar(char* id, int indexes)
{
	for (int i=0;i<vars_count;i++) {
		if (strcmp(vars[i].name,id)==0 && indexes==vars[i].indexes) {
			return &vars[i];
		}
	}
	return NULL;
}


void create_vars(DEFINITIONS* defs)
{
	for (int i=0; i< defs->size;i++) {
		DEFINITION* def = defs->definitions[i];
		for (int j=0;j<def->size;j++) {
			INSTRUCTION* inst = def->instructions[j];
			if (inst->type == PRODUCTION_RULE ||
				 inst->type == INIT_VARIABLE) {
			    EXPR* obj = inst->object;
				VAR* var = searchVar(obj->id,obj->arguments->size);
				if (var==NULL) {
					var = &vars[vars_count++];
					strcpy(var->name,obj->id);
					var->indexes = obj->arguments->size;
					for (int k=0;k<var->indexes;k++) {
						var->limits[k] = 1;
					}
				}
				for (int k=0;k<var->indexes;k++) {
					if (obj->arguments->args[k]->type==INTEGER) {
						int x = obj->arguments->args[k]->intValue+1;
						if (x>var->limits[k]) {
							var->limits[k]=x;
						}
					} else if (obj->arguments->args[k]->type==OBJECT) {
						int find = 0;
						for (int l = 0; l< inst->iterators->size; l++) {
							ITERATOR *it = inst->iterators->iterators[l];
							if (it->type == RANGE_ITERATOR && strcmp(it->id,obj->arguments->args[k]->id)==0) {
								int x = it->right->intValue+1;
								if (x>var->limits[k]) {
									var->limits[k]=x;
								}
								find = 1;
							}
						}
						if (!find) {
							var->limits[k] = SIM_MAX_MEMBRANES;
						}
					}
				}
			}
		}
	}
}


void generate_loop(FILE* fp, DEFINITIONS* defs)
{
	fprintf(fp,"// MAIN LOOP\n");
	fprintf(fp,"\nvoid loop()\n");
	fprintf(fp,"{\n");
	fprintf(fp,"\tint step=0;\n");
	fprintf(fp,"\twhile(step<max_steps && (isnan(Halt1[0]) || Halt1[0]==0))\n");
	fprintf(fp,"\t{\n");
	fprintf(fp,"\t\tif(debug) {\n");
	fprintf(fp,"\t\t\tprintf(\"\\n\\n------ STEP %%d protein = %%d------\\n\",step+1,protein);\n");
	fprintf(fp,"\t\t}\n");
	fprintf(fp,"\t\t#pragma omp parallel num_threads(threads)\n");
	fprintf(fp,"\t\t{\n");
	fprintf(fp,"\t\t\t#pragma omp sections\n");
	fprintf(fp,"\t\t\t{\n");
	for (int i=0;i<functions;i++) {
		fprintf(fp,"\t\t\t\t#pragma omp section\n");
		fprintf(fp,"\t\t\t\trule%d();\n",i);
	}		
	fprintf(fp,"\t\t\t}\n");
	fprintf(fp,"\t\t}\n");
	fprintf(fp,"\t\tprotein = next_protein;\n");
	fprintf(fp,"\t\tif(debug) {\n");
	fprintf(fp,"\t\t\tprintf(\"\\n----MEMBRANES---\\n\");\n");
	fprintf(fp,"\t\t\tfor (int i=0;i<%d;i++) {\n",SIM_MAX_MEMBRANES);
	fprintf(fp,"\t\t\t\tif (membranes[i]!=0) {\n");
	fprintf(fp,"\t\t\t\t\tprintf(\"p(%%d) = %%d \",i,(membranes[i] & 0x00FFFFFF));\n");
	fprintf(fp,"\t\t\t\t}\n");
	fprintf(fp,"\t\t\t}\n");
	
	fprintf(fp,"\t\t\tprintf(\"\\n\\n----VARIABLES---\\n\");\n");
	
	for (int i=0;i<vars_count;i++) {
		if (vars[i].indexes==1) {
			
			fprintf(fp,"\t\t\tfor (int i=0;i<%d;i++) {\n",vars[i].limits[0]);
			fprintf(fp,"\t\t\t\tif(!isnan(%s%d[i])) printf(\"%s%d[%%d] = %%.2f \",i,%s%d[i]);\n",
			  vars[i].name,
			  vars[i].indexes,
			  vars[i].name,
			  vars[i].indexes,
			  vars[i].name,
			  vars[i].indexes);	
			fprintf(fp,"\t\t\t}\n");
			
		} else {
			fprintf(fp,"\t\t\tfor (int i=0;i<%d;i++) {\n",vars[i].limits[0]);
			fprintf(fp,"\t\t\t\tfor (int j=0;j<%d;j++) {\n",vars[i].limits[1]);
			fprintf(fp,"\t\t\t\t\tif(!isnan(%s%d[i][j])) printf(\"%s[%%d][%%d] = %%.2f \",i,j,%s%d[i][j]);\n",
			  vars[i].name,
			  vars[i].indexes,
			  vars[i].name,
			  vars[i].name,
			  vars[i].indexes);	
			fprintf(fp,"\t\t\t\t}\n");	  
			fprintf(fp,"\t\t\t}\n");	
		}
	}
	
	fprintf(fp,"\t\t\tprintf(\"\\n\\nPress ENTER for next step\");\n");
	fprintf(fp,"\t\t\tgetchar();\n");
	fprintf(fp,"\t\t}\n");
	
	fprintf(fp,"\t\t++step;\n");
	fprintf(fp,"\t}\n");
	fprintf(fp,"}\n");
	
}

void generate_var(FILE* fp, EXPR* obj,int in);

void print_index(FILE* fp, EXPR* obj, int index, int in)
{
	VAR *v = searchVar(obj->id,obj->arguments->size);
	
	if (obj->arguments->args[index]->type==INTEGER) {
		fprintf(fp,"%d",obj->arguments->args[index]->intValue);
	} else if (strcmp(obj->arguments->args[index]->id,"h")==0) {
		fprintf(fp,"membranes_in_%d[h]",in);
	} else {
		fprintf(fp,"(int)round(");
		generate_var(fp,obj->arguments->args[index],in);
		fprintf(fp,")");
	}
}

void generate_var(FILE* fp, EXPR* obj,int in)
{
	VAR *v = searchVar(obj->id,obj->arguments->size);
	fprintf(fp,"%s%d[",v->name,v->indexes);
	if (v->indexes==1) {
		if (obj->arguments->args[0]->type==INTEGER) {
			fprintf(fp,"%d]",obj->arguments->args[0]->intValue);
		} else {
			fprintf(fp,"membranes_in_%d[h]]",in);
		}
		
	} else if (v->indexes==2) {
		if (obj->arguments->args[0]->type==INTEGER && obj->arguments->args[1]->type==INTEGER) {
			int v1 = obj->arguments->args[0]->intValue;
			int v2 = obj->arguments->args[1]->intValue;	
			fprintf(fp,"%d][%d]",v1,v2);	
		} else if (obj->arguments->args[0]->type==INTEGER && obj->arguments->args[1]->type!=INTEGER) {
			int v1 = obj->arguments->args[0]->intValue;
			if (strcmp(obj->arguments->args[1]->id,"h")==0) {
				fprintf(fp,"%d][membranes_in_%d[h]]",v1,in);
			} else {
				fprintf(fp,"%d][(int)round(",v1);
				generate_var(fp,obj->arguments->args[1],in);
				fprintf(fp,")]");
			}
		} else if (obj->arguments->args[0]->type!=INTEGER && obj->arguments->args[1]->type==INTEGER) {
			int v2 = obj->arguments->args[1]->intValue;
			if (strcmp(obj->arguments->args[0]->id,"h")==0) {
				fprintf(fp,"membranes_in_%d[h]][%d]",in,v2);
			} else {
				fprintf(fp,"(int)round(");
				generate_var(fp,obj->arguments->args[0],in);
				fprintf(fp,")][%d]",v2);
			}
		} 
	}
}

void generate_min(FILE* fp, EXPR* expr, int val)
{
	if (expr->arguments->iterators->size>0) {
		val = expr->arguments->iterators->iterators[0]->left->intValue;
		fprintf(fp,"function_min(%s%d,membranes_in_%d,membranes_in_%d_size)",
		  expr->arguments->args[0]->id,expr->arguments->args[0]->arguments->size,val,val);
	}
}

void generate_arg_min(FILE* fp, EXPR* expr, int val)
{
	if (expr->arguments->iterators->size>0) {
		val = expr->arguments->iterators->iterators[0]->left->intValue;
		fprintf(fp,"function_arg_min(%s%d,membranes_in_%d,membranes_in_%d_size)",
		  expr->arguments->args[0]->id,expr->arguments->args[0]->arguments->size,val,val);
	}
}

void generate_expr(FILE* fp, EXPR* expr, int val)
{
	if (expr==NULL) {
		return;
	}
	switch(expr->type) {
		case FUNCTION:
			if (strcmp(expr->id,"min")==0) {
				generate_min(fp,expr,val);
			} else if (strcmp(expr->id,"arg_min")==0) {
				generate_arg_min(fp,expr,val);
			} else {
				fprintf(fp,"function_%s(",expr->id);
					if (expr->arguments->size>0) {
						generate_expr(fp,expr->arguments->args[0],val);
					}
					for (int i=1;i<expr->arguments->size;i++) {
						fprintf(fp,", ");
						generate_expr(fp,expr->arguments->args[i],val);
					}
					fprintf(fp,")");
				}
				break;
		case OBJECT:
			generate_var(fp,expr,val);
			break;
		case INTEGER:
			fprintf(fp,"%d",expr->intValue);
			break;
		case REAL:
			fprintf(fp,"%f",expr->doubleValue);
			break;
		case ADD:case SUB:case MUL:case DIV:case MOD:
		case LT:case GT:case EQ:case NEQ:case NOT:case LE:
		case GE:case AND:case OR:
			fprintf(fp,"(");
			generate_expr(fp,expr->left,val);
			printType(fp,expr->type);
			generate_expr(fp,expr->right,val);
			fprintf(fp,")");
		break;
	}
}


void generate_guard(FILE* fp, INSTRUCTION* inst)
{
	if (inst->protein!=NULL) {
		fprintf(fp,"\tif (protein != %d) {\n",inst->protein->arguments->args[0]->intValue);
		fprintf(fp,"\t\treturn;\n");
		fprintf(fp,"\t}\n");
	}
	if (inst->enzyme!=NULL) {
		fprintf(fp,"\tif (");
		generate_expr(fp,inst->enzyme,0);
		fprintf(fp," == 0) {\n");
		fprintf(fp,"\t\treturn;\n");
		fprintf(fp,"\t}\n");
	}
	
}

void generate_function(FILE* fp, INSTRUCTION* inst)
{
	char tabs[16];
	tabs[0]='\t';
	tabs[1]=0;
	fprintf(fp,"\n");
	fprintf(fp,"// RULE: %d\n",functions);
	fprintf(fp,"// ");
	printInstruction(fp,inst,0);
	fprintf(fp,"\nvoid rule%d()\n",functions++);
	fprintf(fp,"{\n");
	generate_guard(fp,inst);
	int val=0;
	if (inst->iterators->size>0) {
		val = inst->iterators->iterators[0]->left->intValue;
		fprintf(fp,"\t#pragma omp parallel for\n");
		fprintf(fp,"\tfor(int h=0;h<membranes_in_%d_size;++h) {\n",val);
		tabs[1]='\t';
		tabs[2]=0;
	}
	if (inst->type == PRODUCTION_RULE) {
		fprintf(fp,"%s",tabs);
		generate_var(fp,inst->object,val);
		fprintf(fp," = ");
		generate_expr(fp,inst->expr, val);
		fprintf(fp,";\n");
		
		fprintf(fp,"%s",tabs);
		fprintf(fp,"if (debug) {\n");
		fprintf(fp,"%s",tabs);
		fprintf(fp,"\tprintf(\"%s",inst->object->id);
		for (int i=0;i< inst->object->arguments->size;i++) {
			fprintf(fp,"[%%d]");
		}
	
		fprintf(fp," = %%f; // ");
		
		printInstruction(fp,inst,0);
		fprintf(fp,"\\n\"");
		
		for (int i=0;i< inst->object->arguments->size;i++) {
			fprintf(fp,",");
			print_index(fp,inst->object,i,val);
		}
		fprintf(fp,",");
		generate_var(fp,inst->object,val);
		fprintf(fp,");\n");
		fprintf(fp,"%s",tabs);
		fprintf(fp,"}\n");
		
		
	} else if (inst->type == EVOLUTION_RULE) {
		fprintf(fp,"\tif (protein != %d) {\n",inst->object->arguments->args[0]->intValue);
		fprintf(fp,"\t\treturn;\n");
		fprintf(fp,"\t}\n");
		fprintf(fp,"\tnext_protein = %d;\n",inst->expr->arguments->args[0]->intValue);
		
	} else {
		EXPR *parent = inst->expr;
		EXPR *child = inst->object;
		fprintf(fp,"\tint child = (int)round(");
		generate_expr(fp,child, val);
		fprintf(fp,");\n");
		fprintf(fp,"\tint parent = (int)round(");
		generate_expr(fp,parent, val);
		fprintf(fp,");\n");
		fprintf(fp,"\tmembranes[child] = parent;\n");
		fprintf(fp,"\tmembranes[child] |= (membranes[parent] & 0xFF000000);\n");
		for (int i=0;i<labels_count;i++) {
			fprintf(fp,"\tif ((membranes[child] & %s)!=0) membranes_in_%d[membranes_in_%d_size++] = child;\n",masks[i],labels[i],labels[i]);
		}
		fprintf(fp,"%s",tabs);
		fprintf(fp,"if (debug) {\n");
		fprintf(fp,"%s",tabs);
		fprintf(fp,"\tprintf(\"[ [ ]'%%d ]'%%d; // ");
		printInstruction(fp,inst,0);
		fprintf(fp,"\\n\",child,parent);\n%s}\n",tabs);
		
	}
	if (inst->iterators->size>0) {
		fprintf(fp,"\t}\n");
	}
	
	fprintf(fp,"}\n");
}

void create_membranes(FILE* fp, DEFINITIONS* defs)
{
	fprintf(fp,"\n// MEMBRANES\n");
	fprintf(fp,"int *membranes;\n");
	for (int i=0;i<defs->size;i++) {
		DEFINITION* def = defs->definitions[i];
		for (int j=0;j<def->size;j++) {
			INSTRUCTION* inst = def->instructions[j];
			if (inst->type==MU) {
				int label = inst->mu->label->intValue;
				fprintf(fp,"int* membranes_in_%d;\n",label);
				fprintf(fp,"int membranes_in_%d_size = 0;\n",label);
				labels[labels_count++] = label;
				for (int k=0;k<inst->mu->size;k++) {
					int label = inst->mu->membranes[k]->label->intValue;
					fprintf(fp,"int* membranes_in_%d;\n",label);
					fprintf(fp,"int membranes_in_%d_size = 0;\n",label);
					labels[labels_count++] = label;
				}
			}
		}
	}
}


void generate_c_simulator(FILE* fp, DEFINITIONS* defs)
{
	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <stdlib.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"#include <time.h>\n");
	fprintf(fp,"#include <omp.h>\n");
	fprintf(fp,"#include \"functions.h\"\n");	
	fprintf(fp,"#include \"pgm.h\"\n");	
	fprintf(fp,"char map_file[64];\n");
	fprintf(fp,"char out_file[64];\n");
	fprintf(fp,"extern PGM *map;\n");
	fprintf(fp,"int debug = 0;\n");
	fprintf(fp,"int threads = 4;\n");
	fprintf(fp,"int max_steps = %d;\n",SIM_MAX_ITERS);
	fprintf(fp,"\nvoid loop();\n");

	create_membranes(fp,defs);
	create_vars(defs);	
	fprintf(fp,"\n//PROTEIN\n");
	fprintf(fp,"int protein = 1;\n");
	fprintf(fp,"int next_protein = 1;\n");

	
	fprintf(fp,"\n//VARIABLES\n");
	for (int i=0;i<vars_count;i++) {
		fprintf(fp,"double ");
		for(int j=0;j<vars[i].indexes;j++) {
			fprintf(fp,"*");
		}
		fprintf(fp,"%s%d;\n",vars[i].name,vars[i].indexes);
	}
		
	fprintf(fp,"\nint main(int argc, char* argv[])\n");
	fprintf(fp,"{\n");
	fprintf(fp,"\tunsigned int seed = time(NULL);\n");
	fprintf(fp,"\tstrcpy(map_file,\"office.pgm\");\n");
	fprintf(fp,"\tstrcpy(out_file,\"out.pgm\");\n");
	fprintf(fp,"\tparse_input(argc,argv,&debug,&threads,&max_steps,map_file,out_file,&seed);\n");
	fprintf(fp,"\tsrand(seed);\n");
	fprintf(fp,"\tprint_header(debug,threads,max_steps,map_file,out_file);\n");
	fprintf(fp,"\tmap = load_pgm(map_file);\n");
	fprintf(fp,"\t// SET MEMORY FOR MEMBRANES\n");
	fprintf(fp,"\tmembranes = (int*)malloc(sizeof(int)*%d);\n",SIM_MAX_MEMBRANES);
	fprintf(fp,"\tmemset(membranes,0,sizeof(int)*%d);\n",SIM_MAX_MEMBRANES);
	for (int i=0;i<labels_count;i++) {
		fprintf(fp,"\tmembranes_in_%d = (int*)malloc(sizeof(int)*%d);\n",labels[i],SIM_MAX_MEMBRANES);
		
	}
	fprintf(fp,"\t// SET MEMORY FOR VARIABLES\n");
	
	for (int i=0;i<vars_count;i++) {
		
		if (vars[i].indexes==1) {
			fprintf(fp,"\t%s%d = (double*)malloc(sizeof(double)*%d);\n",vars[i].name,vars[i].indexes,vars[i].limits[0]);
			fprintf(fp,"\tmemset(%s%d,0xFF,sizeof(double)*%d);\n",vars[i].name,vars[i].indexes,vars[i].limits[0]);	
		} else {
			fprintf(fp,"\t%s%d = (double**)malloc(sizeof(double*)*%d);\n",vars[i].name,vars[i].indexes,vars[i].limits[0]);	
			fprintf(fp,"\tfor(int i=0;i<%d;i++) {%s%d[i] = (double*)malloc(sizeof(double)*%d);memset(%s%d[i],0xFF,sizeof(double)*%d);}\n",
			  vars[i].limits[0],vars[i].name,vars[i].indexes,vars[i].limits[1],
			  vars[i].name,vars[i].indexes,vars[i].limits[1]);
		}
	
	}
	fprintf(fp,"\t// INIT MEMBRANES AND VARIABLES\n");
	for (int i=0;i<defs->size;i++) {
		DEFINITION* def = defs->definitions[i];
		for (int j=0;j<def->size;j++) {
			INSTRUCTION* inst = def->instructions[j];
			if (inst->type==INIT_VARIABLE) {
				fprintf(fp,"\t");
				generate_var(fp,inst->object,0);
				fprintf(fp," = ");
				generate_expr(fp,inst->expr, 0);
				fprintf(fp,";\n");
			} else if (inst->type==MU) {
				int label0 = inst->mu->label->intValue;
				for (int k=0;k<inst->mu->size;k++) {
					int label1 = inst->mu->membranes[k]->label->intValue;
					fprintf(fp,"\tmembranes_in_%d[membranes_in_%d_size++] = %d;\n",label0,label0,label1);
					fprintf(fp,"\tmembranes_in_%d[membranes_in_%d_size++] = %d;\n",label1,label1,label1);
				}
				
			} 
		}
	}
	for (int i=1;i<labels_count;i++) {
		fprintf(fp,"\tmembranes[%d] = %d;\n",labels[i],labels[0]);
		fprintf(fp,"\tmembranes[%d] |= %s;\n",labels[i],masks[0]);
		fprintf(fp,"\tmembranes[%d] |= %s;\n",labels[i],masks[i]);
	}
	
	fprintf(fp,"\t// MAIN LOOP\n");
	
	fprintf(fp,"\tloop();\n");
	
	fprintf(fp,"\t// WRITE OUTPUT FILE\n");
	fprintf(fp,"\tfor (int i=0;i<membranes_in_%d_size;i++) {\n",labels[0]);
	fprintf(fp,"\t\tint child = membranes_in_%d[i];\n",labels[0]);
	fprintf(fp,"\t\tint parent = membranes[child] & 0x00FFFFFF;\n");
	fprintf(fp,"\t\tif (parent == %d) continue;\n",labels[0]);
	fprintf(fp,"\t\tint x0 = (int)round(Y2[1][child]);\n");
	fprintf(fp,"\t\tint y0 = (int)round(Y2[2][child]);\n");
	fprintf(fp,"\t\tint x1 = (int)round(Y2[1][parent]);\n");
	fprintf(fp,"\t\tint y1 = (int)round(Y2[2][parent]);\n");
	fprintf(fp,"\t\tdraw_line(map,x0,y0,x1,y1,0);\n");
	fprintf(fp,"\t}\n");
	fprintf(fp,"\tstrcpy(map->file,out_file);\n");
	fprintf(fp,"\tsave_pgm(map);\n");
	fprintf(fp,"}\n");
		
	for (int i=0;i<defs->size;i++) {
		DEFINITION* def = defs->definitions[i];
		for (int j=0;j<def->size;j++) {
			INSTRUCTION* inst = def->instructions[j];
			if (inst->type==PRODUCTION_RULE || inst->type==CREATION_RULE || inst->type==EVOLUTION_RULE) {
				generate_function(fp,inst);
			} 
		}
	}
	generate_loop(fp,defs);
}

#endif
