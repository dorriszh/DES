Author          : Ruixuan Zhang
Created         : November 8, 2019
Last Modified   : November 8, 2019

Affiliation          : Georgia Institute of Technology


Description
-------------

This program is to simulate the queueing network by discrete event simulation. 


The configuration file has the following form:

12
0 G 3 1 
1 Q 0.1 4 0.2 0.4 0.2 0.2 2 3 4 5
2 Q 3.2 2 0.5 0.5 6 7
3 Q 1.7	2 0.5 0.5 6 7
4 Q 3.3	2 0.5 0.5 6 7
5 Q 2.8 2 0.5 0.5 6 7
6 Q 0.5 2 0.5 0.5 11 12
7 Q 0.5 2 0.5 0.5 11 12
8 E
9 Q 0 4 0.2 0.4 0.2 0.2 2 3 4 5
11 Q 1 2 0.95 0.05 8 9
12 Q 1 2 0.95 0.05 8 9

The first line represent the number of components; The first column of following lines denotes the component ID while the second column denotes the type of components(Generator,Queue or Exit), the third column indicates the average experience time of that component. 
The forth column indicates the number(K) of possible destination components to which customers completing service may be routed, followed by K probabilities of each possible destination components and their IDs.


The output file has the following format:

Comp:ID GENERATOR  
  NUM_ENTER_SYSTEM

Comp:ID QUEUE STATION
  Arrival: NUM_ARRIVAL
          MAX_QUEUE_TIME_IN_SIMU
          MIN_QUEUE_TIME_IN_SIMU
          AVG_QUEUE_TIME_IN_SIMU
          NUM_QUEUE

          MAX_SERVICE_TIME_IN_SIMU
          MIN_SERVICE_TIME_IN_SIMU
          AVG_SERVICE_TIME_IN_SIMU
          NUM_DEPARTURE(number of customers' departure from this component to next destination)

According to the problem requirement, there is a fork queue station in the queue components which is just to distribute the return customers who do not have enough funds to make their order and return to make another food order bypassing the tray station. 
In this station, all the queue time and service time will not be larger than 0.

Comp:ID EXIT
  Number: NUM_EXIT
          MAX_EXPR_TIME (experience time: overall traveling time of each customer in the system)
          MIN_EXPR_TIME
          AVG_EXPR_TIME

          MAX_SERVICE_TIME
          MIN_SERVICE_TIME
          AVG_SERVICE_TIME

          MAX_QUEUING_TIME(queuing time: overall queuing time of each customer in all components in the whole simulation)
          MIN_QUEUING_TIME
          AVG_QUEUING_TIME
          AVG_QUEUING_TIME/AVG_EXPR_TIME

          AVG_QUEUING_STATION(average number of stations customers will be in queue in the simulation) 

In system:(Now)
          CURRENT_TIME

          MAX_QUEUING_TIME
          MIN_QUEUING_TIME
          AVG_QUEUING_TIME
          CUSTOMER_IN_SYSTEM


Installation
------------

To install, simply run

    gcc -std=c99 -c -O2 -Wall  -o pq.o  pq.c
    gcc -std=c99 -c -O2 -Wall  -o a6_engine.o  a6_engine.c
    gcc -std=c99 -c -O2 -Wall  -o a6_application.o a6_application.c
    gcc -O2 -Wall -o cpssim   pq.o a6_engine.o a6_application.o

Note: The -std=c99 flag is required on some C compilers to allow variable declarations at arbitrary locations in a function.

Execution
-----------

Assuming your executable is called "cpssim", run it using

    ./cpssim 1000.0 config_a6 output

where the second value indicates the amount of time the simulator should run. You may specify the file name of the output file.
    


