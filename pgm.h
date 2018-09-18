/* 
 * pgm.h:
 *
 * This file contains function prototypes for processing PGM files.
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

#ifndef _PGM_H_
#define _PGM_H_

typedef struct
{
	char file[64];
	int width;	
	int height;
	int maxval;
	unsigned char *raster;
} PGM;

int save_pgm(PGM* pgm);

PGM* load_pgm(const char* file);

void draw_line(PGM* pgm, int x0, int y0, int x1, int y1, unsigned char color);

int detect_obstacle(PGM* pgm, int x0, int y0, int x1, int y1, unsigned char threshold);
    
void destroy_pgm(PGM* pgm);

#endif
