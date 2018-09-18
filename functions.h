/* 
 * functions.h:
 *
 * This file contains the C/OpenMP implementation of the production functions
 * used to simulate the bidirectional RRT algorithm for Robot Path Planning 
 * by means of Random Enzymatic Numerical P Systems with Shared Memory (RENPSM). 
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

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "pgm.h"

PGM *map;

double function_round(double x)
{
	return round(x);
}

double function_random(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;
    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; 
    } else {
        low_num = max_num + 1; 
        hi_num = min_num;
    }
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

double function_euclideanDistance(double x0, double y0, double x1, double y1)
{
	return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
}

double function_min(double** values, int* indexes, int size_indexes)
{
	double min_val = values[indexes[0]][0];
	#pragma omp parallel for reduction(min : min_val)
	for (int i=1;i<size_indexes;i++) {
		if (values[indexes[i]][0]<min_val) {
			min_val = values[indexes[i]][0];
		}
	}
	return min_val;
}

double function_arg_min(double** values, int* indexes, int size_indexes)
{
	double min_val = values[indexes[0]][0];
	double min_arg = indexes[0];
	#pragma omp parallel for reduction(min : min_val)
	for (int i=1;i<size_indexes;i++) {
		if (values[indexes[i]][0]<min_val) {
			min_val = values[indexes[i]][0];
			min_arg = indexes[i];
		}
	}
	return min_arg;
}

double function_if(double cond, double yes, double no)
{
	if (round(cond)>0) {
		return yes;
	}
	return no;
}

double function_rm(double a, double b)
{
	int x = (int)round(a);
	int y = (int)round(b);
	return x%y;
}

double function_qt(double a, double b)
{
	int x = (int)round(a);
	int y = (int)round(b);
	return x/y;
}

double function_collision(double a, double b, double u0, double u1, double delta)
{
	int x0 = (int)round(a);
	int y0 = (int)round(b);
	int x1 = (int)round(a+u0*delta);
	int y1 = (int)round(b+u1*delta);
	return detect_obstacle(map,x0,y0,x1,y1,250);
}

void parse_input(int argc, char* argv[], int *debug, int *threads, int *steps, char *map_file, char *out_file, unsigned int *seed)
{
	int c;
	while ((c = getopt (argc, argv, "dt:s:m:o:r:")) != -1)
    switch (c)
      {
      case 'd':
        *debug = 1;
        break;
      case 't':
        *threads = atoi(optarg);
        break;
      case 's':
		*steps = atoi(optarg);
		break;
	  case 'm':
	    strcpy(map_file,optarg);
	    break;
	  case 'o':
	    strcpy(out_file,optarg);
	    break;
	  case 'r':
		*seed = atoi(optarg);
		break;
      default:
       ;
      }
}

void print_header(int debug, int threads,int max_steps, char *map_file, char* out_file) {
	printf("Ad-hoc generated RENPSM OPENMP simulator\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions; see GNU GPL v3.0 for details.\n");
    printf("DEBUG: %d\n",debug);
    printf("THREADS: %d\n",threads);
    printf("STEPS: %d\n",max_steps);
    printf("MAP: %s\n",map_file);
    printf("OUTPUT: %s\n",out_file);
}

#endif

