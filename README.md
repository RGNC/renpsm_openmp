# renpsm_openmp
Simulation of Rapidly-Exploring Random Trees in Membrane Computing with P-Lingua and Automatic Programming

This program parses P-Lingua files defining RENPSM models and generates ad-hoc simulators. A model for robot
path planning using the bidirectional RRT algorithm is included as example.

You can find more information in:

I. Perez-Hurtado, G. Zang, M.J. Perez-Jimenez, D. Orellana
Simulation of Rapidly-Exploring Random Trees in Membrane Computing with P-Lingua and Automatic Programing
International Journal of Computers, Communications and Control, in press.

# Dependencies
You should install the next programs:

* GNU Bison: https://www.gnu.org/software/bison/
* Flex: https://github.com/westes/flex
* gcc compiler with OpenMP support

You can install all of the dependencies in a Debian or Ubuntu style Linux following the next steps:

- sudo apt-get install build-essential
- sudo apt-get install flex
- sudo apt-get install bison

# Compiling the source code
You can compile the source code following the next steps:

-  bison -yd renpsm.y
-  flex renpsm.l
-  gcc y.tab.c lex.yy.c -lfl -O3 -Wall -o renpsm_openmp

# Using the P-Lingua RENPSM parser

The generated renpsm_openmp program is a command-line executable with the next syntax:

- ./renpsm_openmp < birrt_renpsm.pli

Where ''birrt_renpsm.pli'' is a P-Lingua file defining a RENPSM.model.

It generates as output a file called ''simulator.c'' containing the source code
in C language and OpenMP for an ad-hoc simulator following the model defined in the P-Lingua file.

You can compile the generated ad-hoc simulator with the next line:

- gcc simulator.c pgm.c -lm -Wall -O3 -fopenmp -o simulator

# Using the generated ad-hoc simulators





