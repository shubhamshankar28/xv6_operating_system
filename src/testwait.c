#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" 

int
main()
{
	
	int i;
	int n=7;
	int j = 0;
	int k;
	int retime;
	int rutime;
	int stime;
	
	
	i = n; 
	int pid;
	for (i = 0; i < 3 * n; i++) {
		j = i % 3;
		pid = fork();
		if (pid == 0) {
			j = (getpid() - 4) % 3; 
			switch(j) {
				case 0: 
					for (k = 0; k < 100; k++){
						for (j = 0; j < 1000000; j++){}
					}
					break;
				case 1: 
					for (k = 0; k < 1000; k++){
						for (j = 0; j < 1000000; j++){}
						
					}
					break;
				case 2:
					for(k = 0; k < 100; k++){
						sleep(1);
					}
					break;
			}
			exit(); 
		}
		continue; 
	}
	for (i = 0; i < 3 * n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		
		
			
printf(1, "CPU-bound, pid: %d, ready: %d, running: %d, sleeping: %d\n", pid, retime, rutime, stime);
				
		}
	
	
	exit();
}