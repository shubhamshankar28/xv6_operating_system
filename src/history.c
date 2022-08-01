#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" 
char *argv[] = { "sl", 0 };
int main(void) 
{  
//printf(1, "testdraw:\n");
char buff[129]="3"; 
int count1=0;
while(history(buff,count1)==0) 
{ 
    printf(2,"%s\n",buff); 
    count1++;
} 
exit();
}
