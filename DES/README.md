Author          : Ruixuan Zhang
Created         : February 3, 2020
Last Modified   : February 3, 2020

Affiliation          : Georgia Institute of Technology


Description
-------------

This program is to simulate the manufacturing by discrete event simulation. 


The configuration file has the following form:

8
0 G 15.0 3.0 1
1 Q 20.0 1 1.0 2
2 Q 8.0 1 1.0 6
3 Q 10.0 1 1.0 6
4 G 12.0 3.0 6
5 G 9.0 2.0 6
6 Q 5.0 1 1.0 7
7 E

The first line represent the number of components; The first column of following lines denotes the component ID while the second column denotes the type of components(Generator,Queue or Exit), the third column indicates the average experience time of that component. 
The forth column indicates the number(K) of possible destination components to which customers completing service may be routed, followed by K probabilities of each possible destination components and their IDs.
Specifically, the third and forth column of a Generator are the mean and variance of Gaussian distributed operational time.



Installation
------------

To install, simply run

    gcc -std=c99 -c -O2 -Wall  -o engine.o  engine.c
    gcc -O2 -Wall -o sim6730  pq.o engine.o application.o

Note: The -std=c99 flag is required on some C compilers to allow variable declarations at arbitrary locations in a function.

Execution
-----------

Assuming your executable is called "sim6730", run it using

    ./sim6730 1000.0 config output

where the second value indicates the amount of time the simulator should run. You may specify the file name of the output file.
    


