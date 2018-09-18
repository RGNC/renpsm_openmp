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
-  gcc y.tab.c lex.yy.c -lfl -o renpsm_openmp

## Using the P-Lingua RENPSM parser

The generated renpsm_openmp program is a command-line executable with the next syntax:

./renpsm_openmp < model.pli

Where ''model.pli'' is a P-Lingua file defining a RENPSM.model.

It generates as output a file called ''simulator.c'' containing the source code
in C language and OpenMP for an ad-hoc simulator following the model defined in the P-Lingua file.

You can compile the generated ad-hoc simulator with the next line:

- gcc simulator.c pgm.c -lm -O3 -fopenmp -o simulator

All the production functions must be implemented in ''functions.h'' file. You could include custom production functions by adding the C code to
the file. 

## Using the generated ad-hoc simulator

The generated ad-hoc simulator has the next command-line syntax:

./simulator [-t threads] [-s steps] [-d] [-r seed] [-m obstacles.pgm] [-o output.pgm] 

Where:

- ''-t threads'' is the number of threads to be used. Default is 4. If you set 1 thread, the simulator will be sequential.
- ''-s steps'' is the maximum number of computational steps to simulate. The simulator stops if the variable Halt{mem} is set to 1 or the number of steps is reached. Default is 1048576 steps.
- If ''-d'' is set, debug information will be prompted.
- ''-r seed'' defines the pseudo-random number generator seed. If no seed is configured, an arbitrary seed based on the current clock time will be used.
- ''-m obstacles.pgm'' is the PGM file defining the obstacle grid for the collision function (optional).
- ''-o output.pgm'' is the PGM file to print the membrane tree (only for RRT algorithms).


## Running the test 1

- ./renpsm_openmp < birrt_renpsm_test1.pli
- gcc simulator.c pgm.c -lm -O3 -fopenmp -o test1
- For debug the computation step by step:
	- ./test1 -t 8 -d -m map.pgm -o test1_output.pgm
- For run the computation until halting condition:
	- ./test1 -t 8 -m map.pgm -o test1_output.pgm
- For run with a specific pseudo-random number generator seed (for instance, 42)
	- ./test1 -t 8 -m map.pgm -r 42 -o test1_output.pgm
### Example outputs with arbitrary pseudo-random number generator seeds

![Example1](/examples/example1.jpg)
![Example2](/examples/example2.jpg)
![Example3](/examples/example3.jpg)
![Example4](/examples/example4.jpg)


## Running the test 2

- ./renpsm_openmp < birrt_renpsm_test2.pli
- gcc simulator.c pgm.c -lm -O3 -fopenmp -o test2
- For debug the computation step by step:
	- ./test2 -t 8 -d -m office.pgm -o test2_output.pgm
- For run the computation until halting condition:
	- ./test2 -t 8 -m office.pgm -o test2_output.pgm
- For run with a specific pseudo-random number generator seed (for instance, 42)
	- ./test2 -t 8 -m office.pgm -r 42 -o test2_output.pgm

### Example outputs with arbitrary pseudo-random number generator seeds

![Example1](/examples/example5.jpg)
![Example2](/examples/example6.jpg)
![Example3](/examples/example7.jpg)
![Example4](/examples/example8.jpg)

## Benchmarking

You can run the script ''benchmark.sh'' to run 240 simulations: 30
simulations for 8 different number of threads for test 1 (map.pgm file)
using the same pseudo-random number generator seed and obtaining wall times
for the simulations (excludind the reading and writing of files).
We have used a computer running Ubuntu Linux 16.04 over an AMD A10-9600P 
processor with 4 cores and 12 GB of RAM obtaining the results in ''benchmark_output.txt''.

If you want to write your benchmark output to a file:

- ./benchmark.sh > file.txt

where file.txt is the name of the output file.

In order to process the data and compute average values and standard deviations, you
can use the file ''process_results.c''. Firstly, you should compile it:

- gcc process_results.c -lm -o process_results

Then, you can process your output file as follows:

- ./process_results file.txt

where file.txt is, again, the name of the output file containing the benchmark results.

Average values and standard deviations will be prompted to the standard output, including speed-up values.

The output for the file benchmark_output.txt is as follows:


Threads: 1. Average time: 26.251848 seconds. Sd: 7.615338. Speed-up 1.000000

Threads: 2. Average time: 0.242932 seconds. Sd: 0.082716. Speed-up 108.062612

Threads: 4. Average time: 0.498864 seconds. Sd: 0.112994. Speed-up 52.623243

Threads: 8. Average time: 1.159005 seconds. Sd: 0.382271. Speed-up 22.650337

Threads: 16. Average time: 1.964375 seconds. Sd: 0.607856. Speed-up 13.363970

Threads: 32. Average time: 4.046735 seconds. Sd: 0.941201. Speed-up 6.487168

Threads: 64. Average time: 7.215875 seconds. Sd: 1.526436. Speed-up 3.638068

Threads: 128. Average time: 17.404175 seconds. Sd: 4.225569. Speed-up 1.508365


Obtaining a speed-up of **108x** when using two threads.



*Programming a program inside a program is something like...*

![Inception](inception.jpg)
 

