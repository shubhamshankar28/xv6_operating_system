#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" 
char *argv[] = { "sl", 0 };
int main(void) 
{  
//printf(1, "testdraw:\n");
char buff[2000]="345"; 
 
int y=draw(buff,2000); 
//printf(2,"%d\n" , y);  
if(y>=0)
printf(2,"%s\n" , buff); 
else 
printf(2,"%d\n" , y);
 exit();
return 0;
}
