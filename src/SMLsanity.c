#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf(1, "Wrong syntax, use: SMLsanity <n>\n");
        exit();
    }

    int retime;
    int rutime;
    int stime;
    int stats[3][3];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            stats[i][j] = 0;
        }
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < 3 * n; i++)
    {
       
         int pid = fork();
        if (pid == 0)
        {
            int j = (getpid() ) % 3;
			#ifdef SML
				switch (j)
				{
				case 0:
					set_prio(1);
					break;
				case 1:
					set_prio(2);
					break;
				case 2:
					set_prio(3);
					break;
				}
			#endif 
            j++;
            j--; 
			for (int k = 0; k < 100; k++)
			{
				for (int l = 0; l < 1000000; l++)
				{
				}
			}
			exit(); // children exit here
		}
        continue; // parent makes next child
    }

    for (int i = 0; i < 3 * n; i++)
    {
        int pid = wait2(&retime, &rutime, &stime);
        int res = (pid ) % 3; // correlates to j in the dispatching loop
        switch (res)
        {
        case 0: // CPU bound processes
            printf(1, "Priority 1, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
            stats[0][0] += retime;
            stats[0][1] += rutime;
            stats[0][2] += stime;
            break;
        case 1: // CPU bound processes, short tasks
            printf(1, "Priority 2, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
            stats[1][0] += retime;
            stats[1][1] += rutime;
            stats[1][2] += stime;
            break;
        case 2: // simulating I/O bound processes
            printf(1, "Priority 3, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
            stats[2][0] += retime;
            stats[2][1] += rutime;
            stats[2][2] += stime;
            break;
        }
    }

	int avg[3][3];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            avg[i][j] = stats[i][j]/n;
        }
    }

    printf(1, "\n\nPriority 1:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", avg[0][0], avg[0][1], avg[0][2], avg[0][0] + avg[0][1] + avg[0][2]);
	printf(1, "Total ready time: %d\nTotal running time: %d\nTotal sleeping time: %d\nTotal turnaround time: %d\n\n\n", stats[0][0], stats[0][1], stats[0][2], stats[0][0] + stats[0][1] + stats[0][2]);
    printf(1, "Priority 2:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", avg[1][0], avg[1][1], avg[1][2], avg[1][0] + avg[1][1] + avg[1][2]);
	printf(1, "Total ready time: %d\nTotal running time: %d\nTotal sleeping time: %d\nTotal turnaround time: %d\n\n\n", stats[1][0], stats[1][1], stats[1][2], stats[1][0] + stats[1][1] + stats[1][2]);
    printf(1, "Priority 3:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", avg[2][0], avg[2][1], avg[2][2], avg[2][0] + avg[2][1] + avg[2][2]);
	printf(1, "Total ready time: %d\nTotal running time: %d\nTotal sleeping time: %d\nTotal turnaround time: %d\n\n\n", stats[2][0], stats[2][1], stats[2][2], stats[2][0] + stats[2][1] + stats[2][2]);

    exit();
}