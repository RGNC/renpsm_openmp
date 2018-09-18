# renpsm_openmp
Simulation of Rapidly-Exploring Random Trees in Membrane Computing with P-Lingua and Automatic Programming

This program parses P-Lingua files defining RENPSM models and generates ad-hoc simulators. A model for robot
path planning using the bidirectional RRT algorithm is included as example.

You can find more information in:

I. Perez-Hurtado, G. Zang, M.J. Perez-Jimenez, D. Orellana
Simulation of Rapidly-Exploring Random Trees in Membrane Computing with P-Lingua and Automatic Programing
International Journal of Computers, Communications and Control, in press.

## Dependencies
You should install the next programs:

* GNU Bison: https://www.gnu.org/software/bison/
* Flex: https://github.com/westes/flex
* gcc compiler with OpenMP support

You can install all of the dependencies in a Debian or Ubuntu style Linux following the next steps:

- sudo apt-get install build-essential
- sudo apt-get install flex
- sudo apt-get install bison

## Compiling the source code
You can compile the source code following the next steps:

-  bison -yd renpsm.y
-  flex renpsm.l
-  gcc y.tab.c lex.yy.c -lfl -O3 -Wall -o renpsm_openmp

## Using the P-Lingua RENPSM parser

The generated renpsm_openmp program is a command-line executable with the next syntax:

./renpsm_openmp < model.pli

Where ''model.pli'' is a P-Lingua file defining a RENPSM.model.

It generates as output a file called ''simulator.c'' containing the source code
in C language and OpenMP for an ad-hoc simulator following the model defined in the P-Lingua file.

You can compile the generated ad-hoc simulator with the next line:

- gcc simulator.c pgm.c -lm -Wall -O3 -fopenmp -o simulator

All the production functions must be implemented in ''functions.h'' file. You could include custom production functions by adding the C code to
the file. 

## Using the generated ad-hoc simulator

The generated ad-hoc simulator has the next command-line syntax:

./simulator [-t threads] [-s steps] [-d] [-m obstacles.pgm] [-o output.pgm] 

Where:

- ''-t threads'' is the number of threads to be used. Default is 4. If you set 1 thread, the simulator will be sequential.
- ''-s steps'' is the maximum number of computational steps to simulate. The simulator stops if the variable Halt{mem} is set to 1 or the number of steps is reached. Default is 1048576 steps.
- If ''-d'' is set, debug information will be prompted.
- ''-m obstacles.pgm'' is the PGM file defining the obstacle grid for the collision function (optional).
- ''-o output.pgm'' is the PGM file to print the membrane tree (only for RRT algorithms).

## Running the example

-  bison -yd renpsm.y
-  flex renpsm.l
-  gcc y.tab.c lex.yy.c -lfl -O3 -Wall -o renpsm_openmp
- ./renpsm_openmp < birrt_renpsm.pli
- ./simulator -t 32 -d -m map.pgm -o output.pgm

*Programming a program inside a program is something like...*

![Inception](inception.jpg)
 

