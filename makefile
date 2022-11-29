# build an executable named myprog from myprog.c
  all: Overseer Controller
 	  
Overseer: Overseer.c Overseer.h
	   gcc -g -Wall -o Overseer Overseer.c -pthread

Controller: Controller.c Controller.h
		gcc -g -Wall -o Controller Controller.c

  clean: 
	  $(RM) Overseer $(RM) Controller