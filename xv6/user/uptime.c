#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  
  if(argc > 1){
    printf("usage: %s\n",argv[0]);
    exit(1);
  }
  int t=uptime()/600;
  int m1=t%10;
  t=t/10;
  int m2=t%6;
  t=t/6;
  int h=t%24;
  t=t/24;
  printf("up %d days, %d:%d%d\n",t,h,m2,m1);
  
  exit(0);

}
