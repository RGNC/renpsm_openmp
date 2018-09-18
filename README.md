# renpsm_openmp
Simulation of Rapidly-Exploring Random Trees in Membrane Computing with P-Lingua and Automatic Programming

This program parses P-Lingua files defining RENPSM models and generate ad-hoc simulators. A model for robot
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
-  gcc y.tab.c lex.yy.c -lfl -O3 -Wall -o plingua_renpsm

# Using the P-Lingua RENPSM parser






