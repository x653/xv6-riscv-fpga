#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

int
main()
{
	int p1[2];
	int p2[2];
	pipe(p1);
	pipe(p2);
	char byte=0;
	int n;
    write(p1[1],&byte,1);
    int pid = fork();
    if(pid == 0){
      //child
      close(p2[0]);
	  close(p1[1]);
      while((n=read(p1[0],&byte,1))>0){
		write(p2[1],&byte,1);
	  }
    }
    else{
      //parent
	int t=uptime();
	int pings=0;
      close(p1[0]);
      close(p2[1]);
      while((n=read(p2[0],&byte,1))>0){
	    pings++;
		write(p1[1],&byte,1);
	    if(uptime()-t==1){
			close(p1[1]);
			printf("pings %d\n",pings);
	  		break;
		}
      }
    }
	exit(0);
}
