#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf("float <a> <b>");
    exit(1);
  }
  
  float a=(float)(*argv[1]-'0');
  float b=(float)(*argv[2]-'0');
  float q=a/b;
  printf("%d.",(int)q);
  q=q-(int)q;
  for (int i=0;i<7;i++){
	q=q*10;
	printf("%d",(int)q);
	q=q-(int)q;
  }

  exit(0);
}
