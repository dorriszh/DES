Author          : Ruixuan Zhang
Created         : February 21, 2020
Last Modified   : February 21, 2020

Affiliation          : Georgia Institute of Technology


Description
-------------

This program is to simulate the manufacturing by discrete event simulation.  
In this project, we have 2 production line designated for three parts(components C1,C2,C3) of our products, 2 external suppliers for the other two parts(C4,C5). They'll be assembled to final products in Assembly line. 
Given parameters for costs and operational time, we'll input our demand and calculate the final cost for our products.

Please use the config file in the folder for pre-set parameters.
The configuration file has the following form:

19
0 G 300 0 20 2 0
1 G 300 0 15 2 1
2 Q 300 60 10 1 1.0 3
3 Q 480 90 9 1 1.0 4
4 S 5 6
5 Q 540 105 13 1 1.0 6
6 Q 390 70 15 1 1.0 7
7 Q 600 120 9 1 1.0 12
8 Q 480 120 5 1 1.0 9
9 Q 360 90 10 1 1.0 10
10 Q 240 45 10 1 1.0 11
11 Q 120 30 5 1 1.0 12
12 A 13 5 7 7 11 15 16
13 Q 1200 300 30 2 0.95 0.05 17 18
14 G 5 0 40 8 0
15 G 2400 300 10 12 0
16 G 2000 350 5 12 0
17 E 1
18 E 0

The first line represent the number of components(--the modules/production stations in the system). The first column contains the ID of those components, and the second column indicates the type of the components.
There are 5 types of components: G,Q,S,A,E, their structures are detailed as following:

COMP_ID G(Generator for raw materials) MEAN STD COST DEST_COMP TAG(indicating if there exists selections of routes, e.g. C1 and C2 uses compatible routes and need SELECT Struct in the program) 

COMP_ID Q(Server, specifically the production line and assembly in our project) MEAN STD COST ROUTE PROB_FOR_EACH_ROUTE DEST_COMP

COMP_ID S(designed for selection between two different routes) DEST_FOR_ROUTE_ONE DEST_FOR_ROUTE_TWO

COMP_ID A(designed for a triggering condition for next components - assembly) DEST_COMP NUM_ORIGIN ORIGIN1 ORIGIN2 ... 

COMP_ID E(END of simulation, calculate costs seperately) ID_EXIT(In this program, acceptable and defective products)



Installation
------------

To install, simply run

    gcc -std=c99 -c -g  -Wall -lm  -o application.o  application.c
    gcc -g  -Wall -lm -o sim6730.exe   pq.o engine.o application.o	

Note: The -std=c99 flag is required on some C compilers to allow variable declarations at arbitrary locations in a function.

If you are using the Makefile provided, just type in

    make

in your terminal.

Execution
-----------

Assuming your executable is called "sim6730", run it using

    ./sim6730 -1 config outfile

where the second value indicates cut-off time for running the simulation, you can set it to -1 if there is no cut-off time and get the final output without time limitation.
The second value is the config file we use for pre-set parameters.
The third is the output file for cost and other info. You may specify the file name of the output file.
    


