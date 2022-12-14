// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"  
#include "specialheader.h"
int starte;

int commands[17][130]; 
int st=0,fn=0,commandtracker=0;
static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons; 
#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;
 
static void addcommand(int *input) 
{  
  if((fn+1)%17!=st) 
  { fn=(fn+1)%17;
    int i=0; 
    while(input[i]!='\0') 
    { 
      commands[fn][i]=input[i]; 
      i++;
    } 
    commands[fn][i]='\0';
  } 
  else 
  { 
    st=(st+1)%17;  
    fn=(fn+1)%17;
    int i=0; 
    while(input[i]!='\0') 
    { 
      commands[fn][i]=input[i]; 
      i++;
    } 
    commands[fn][i]='\0';

  } 
commandtracker=fn;

} 
void setbuffer(char *buff) 
{ 
  buff[0]='a'; 
  buff[1]= '\0';
} 
int retrievehistory(char *buffer,int id) 
{ 
  if(id<0||id>15) 
  return 2; 
  if(((fn-st+17)%17)<id) 
  return 1; 
  int index=(st+id+1)%17;  
  int i=0;
  for(i=0;commands[index][i]!='\0';++i) 
  buffer[i]=commands[index][i];  
  buffer[i]='\0';
  return 0;
}
static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
int globptr=0;
static void
cgaputc(int c,int d)
{
  int pos;
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);
int flag=0;
  if(c == '\n')
    {  
     if(crt[pos-pos%80]==('$'|0x700)) 
     {
      int start=pos-pos%80+2;    
      int length=0; 
       int commandbuff[130];  
      while(crt[start]!=(' '|0x700)) 
      { 
        commandbuff[start-(pos-pos%80+2)]=(crt[start]&(127)); 
        start++;  
        length++;} 
        commandbuff[start-(pos-pos%80+2)]='\0'; 
      addcommand(commandbuff);  
     /*int i=0;    
     starte=input.w;
    */  /*printint(starte,10,0);  
      printint(10,10,0);
      printint(input.e,10,0);
 
 */   /*for(i=0;commandbuff[i]!='\0';++i) 
      { 
        input.buf[(i+starte)%INPUT_BUF]=commandbuff[i];
      } 
      input.buf[(i+starte)%INPUT_BUF]='\n';  
      input.e=(i+starte+1)%INPUT_BUF;
  */
    // pos=pos-pos%80+9; 
     
     }
     
      pos += 80 - pos%80;}
  else if(c == BACKSPACE){ 
    //printint(pos,10,0);
    if(pos > 0) --pos;
  }  
  //left arrow 
  else if (c == 228) 
  { 
    if((pos%80)>2) {--pos;} 
    flag=1;
  }  
  //right arrow
  else if (c == 229) 
  { 
    pos++; 
    flag=1;
  }  
  //up arrow
  else if ( c == 226 )  
  { 
  
    
         
  if(commandtracker!=st) 
  {
    
     
    int startpos = pos- pos%80+2;  
    
   
   
    if(pos!=startpos) 
    { 
      for(int j=startpos;crt[j]!=(' '|0X700);++j) 
      crt[j]=(' '|0X700); 
      pos=startpos;  
      outb(CRTPORT, 14);
      outb(CRTPORT+1, (pos)>>8);
      outb(CRTPORT, 15);
      outb(CRTPORT+1, pos);  
      
    }  


   int len=0;
   for(int j=0;commands[commandtracker][j]!='\0';++j) 
   {input.buf[input.e++ % INPUT_BUF] = commands[commandtracker][j];
     consputc(commands[commandtracker][j]);   
   len++;
   }   
  pos=pos+len; 
   
 //input.buf[input.e++ % INPUT_BUF] = '\n';
   commandtracker=(commandtracker-1+17)%17; 
  //printint(pos,10,0); 
  

    
  } 

   
    
    
  } 
  else if( c == 227 ) 
  {  
    if( commandtracker !=fn) 
    {
    commandtracker=(commandtracker+1)%17; 

    int startpos = pos- pos%80+2;  
    
   
   
    if(pos!=startpos) 
    { 
      for(int j=startpos;crt[j]!=(' '|0X700);++j) 
      crt[j]=(' '|0X700); 
      pos=startpos;  
      outb(CRTPORT, 14);
      outb(CRTPORT+1, (pos)>>8);
      outb(CRTPORT, 15);
      outb(CRTPORT+1, pos);  
      
    }  


   int len=0;
   for(int j=0;commands[commandtracker][j]!='\0';++j) 
   {consputc(commands[commandtracker][j]);   
   len++;
   }   
  pos=pos+len; 
    }
 
   

  }
  
   else
    {   int findpos=pos; 
    while(findpos<pos+129&&crt[findpos]!=(' '|0x0700)) 
    findpos++; 

      if(pos==findpos||findpos==pos+129)
      crt[pos++] = (c&0xff) | 0x0700; 
      else  
      { 
        for(int i=findpos;i>=pos;--i) 
        crt[i+1]=crt[i]; 
        crt[pos++]=(c&0xff) | 0x0700; 
        flag=1;
      }

    }  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, (pos)>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);  

  if(flag!=1)
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    {  
      if(c<128)
      
      uartputc(c);  
      else 
      {uartputc('\t'); }
       
      }
  cgaputc(c,1);
}



#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;
int flag=0;
  acquire(&cons.lock); 
  //starte=-10; 
  
  while((c = getc()) >= 0){ 
   // printint(flag,10,1);
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){  
     //   printint(flag,10,1);
        if(flag==0) 
        { flag++;
          starte=input.e;
        }   
        flag++;
       // printint(flag,10,1);
       // printint(input.e,10,0);
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c); 
        //printint(flag,10,1);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

