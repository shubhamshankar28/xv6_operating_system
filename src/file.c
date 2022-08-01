//
// File descriptors
//


#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

struct devsw devsw[NDEV];
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;
int sys_draw(void *p,uint n) 
{ 
/*
FILE *fp =fopen("art3" , "r");

if(fp==NULL)
{
    printf("wrong");
    return 0;
}
char c[1200];
int count1=0;
char ch;
while((ch=getc(fp))!=EOF)
{
    c[count1]=ch;count1++;

}
c[count1]='\0';
count1++;
fclose(fp);  
*/
//char c[1200] = "%         #   @ %  (   #         /\n @        #      #     #       ..  \n   /@@@@                 #@@@@ \n";  
char c[2000]= "	            	\t\t,ood8888booo,\n\
                              ,od8           8bo,\n\
                           ,od                   bo,\n\
                         ,d8                       8b,\n\
                        ,o                           o,    ,a8b\n\
                       ,8                             8,,od8  8\n\
                       8'                             d8'     8b\n\
                       8                           d8'ba     aP'\n\
                       Y,                       o8'         aP'\n\
                        Y8,                      YaaaP'    ba\n\
                         Y8o                   Y8'         88\n\
                          `Y8               ,8\"           `P\n\
                            Y8o        ,d8P'              ba\n\
                       ooood8888888P\"\"\"'                  P'\n\
                    ,od                                  8\n\
                 ,dP     o88o                           o'\n\
                ,dP          8                          8\n\
               ,d'   oo       8                       ,8\n\
               $    d$\"8      8           Y    Y  o   8\n\
              d    d  d8    od  \"\"boooooooob   d\"\" 8   8\n\
              $    8  d   ood' ,   8        b  8   '8  b\n\
              $   $  8  8     d  d8        `b  d    '8  b\n\
               $  $ 8   b    Y  d8          8 ,P     '8  b\n\
               `$$  Yb  b     8b 8b         8 8,      '8  o,\n\
                    `Y  b      8o  $$o      d  b        b   $o\n\
                     8   '$     8$,,$\"      $   $o      '$o$$\n\
                      $o$$P\"                 $$o$\n\n"; 
int count1=0; 
while(c[count1]!='\0') 
count1++; 
count1++; 

if(n>=count1) 
{ 
for(int i=0;i<count1;++i) 
*(char *)(p+i)=c[i];  
return count1; 
} 
return -1;
} 
 



void
fileinit(void)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  if(ff.type == FD_PIPE)
    pipeclose(ff.pipe, ff.writable);
  else if(ff.type == FD_INODE){
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == FD_INODE){
    ilock(f->ip);
    stati(f->ip, st);
    iunlock(f->ip);
    return 0;
  }
  return -1;
}

// Read from file f.
int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip);
    if((r = readi(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  panic("fileread");
}

//PAGEBREAK!
// Write to file f.
int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    // this really belongs lower down, since writei()
    // might be writing a device like the console.
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
      int n1 = n - i;
      if(n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      if(r < 0)
        break;
      if(r != n1)
        panic("short filewrite");
      i += r;
    }
    return i == n ? n : -1;
  }
  panic("filewrite");
}

