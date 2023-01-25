#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc <= 1){
    printf("Hello");
    exit(1);
  }
  printf("Hello %s\n",argv[1]);
  exit(0);
}
