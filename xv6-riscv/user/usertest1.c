#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ  (MAXOPBLOCKS+2)*BSIZE

char buf[BUFSZ];
char name[3];

void dirtest(char *s)
{
  printf("mkdir test\n");

  if(mkdir("dir0") < 0){
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  if(chdir("dir0") < 0){
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  if(chdir("..") < 0){
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  if(unlink("dir0") < 0){
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
  printf("%s: mkdir test ok\n");
}

// directory that uses indirect blocks
void
bigdir(char *s)
{
  enum { N = 500 };
  int i, fd;
  char name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  if(fd < 0){
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  for(i = 0; i < N; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(link("bd", name) != 0){
      printf("%s: bigdir link failed\n", s);
      exit(1);
    }
  }

  unlink("bd");
  for(i = 0; i < N; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(unlink(name) != 0){
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

void
subdir(char *s)
{
  int fd, cc;

  unlink("ff");
  if(mkdir("dd") != 0){
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "ff", 2);
  close(fd);

  if(unlink("dd") >= 0){
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  if(mkdir("/dd/dd") != 0){
    printf("subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  if(fd < 0){
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = read(fd, buf, sizeof(buf));
  if(cc != 2 || buf[0] != 'f'){
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  if(link("dd/dd/ff", "dd/dd/ffff") != 0){
    printf("link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  if(unlink("dd/dd/ff") != 0){
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  if(chdir("dd") != 0){
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  if(chdir("dd/../../dd") != 0){
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  if(chdir("dd/../../../dd") != 0){
    printf("chdir dd/../../dd failed\n", s);
    exit(1);
  }
  if(chdir("./..") != 0){
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  if(fd < 0){
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  if(read(fd, buf, sizeof(buf)) != 2){
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  if(open("dd", O_CREATE) >= 0){
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  if(open("dd", O_RDWR) >= 0){
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  if(open("dd", O_WRONLY) >= 0){
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  if(link("dd/ff/ff", "dd/dd/xx") == 0){
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  if(link("dd/xx/ff", "dd/dd/xx") == 0){
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  if(link("dd/ff", "dd/dd/ffff") == 0){
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  if(mkdir("dd/ff/ff") == 0){
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  if(mkdir("dd/xx/ff") == 0){
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  if(mkdir("dd/dd/ffff") == 0){
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  if(unlink("dd/xx/ff") == 0){
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  if(unlink("dd/ff/ff") == 0){
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  if(chdir("dd/ff") == 0){
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  if(chdir("dd/xx") == 0){
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  if(unlink("dd/dd/ffff") != 0){
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  if(unlink("dd/ff") != 0){
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  if(unlink("dd") == 0){
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  if(unlink("dd/dd") < 0){
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  if(unlink("dd") < 0){
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}
// run each test in its own process. run returns 1 if child's exit()
// indicates success.
int
run(void f(char *), char *s) {
  int pid;
  int xstatus;
  
  printf("test %s: ", s);
  if((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  if(pid == 0) {
    f(s);
    exit(0);
  } else {
    wait(&xstatus);
    if(xstatus != 0) 
      printf("FAILED\n", s);
    else
      printf("OK\n", s);
    return xstatus == 0;
  }
}


int
main(int argc, char *argv[])
{
  char *n = 0;
  if(argc > 1) {
    n = argv[1];
  }
  
  struct test {
    void (*f)(char *);
    char *s;
  } tests[] = {
    {dirtest, "dirtest"},
    {subdir, "subdir"}, // slow
    { 0, 0},
  };
    
  printf("usertest1 starting\n");

  if(open("usertest1.ran", 0) >= 0){
    printf("already ran user tests -- rebuild fs.img (rm fs.img; make fs.img)\n");
    exit(1);
  }
  close(open("usertest1.ran", O_CREATE));

  int fail = 0;
  for (struct test *t = tests; t->s != 0; t++) {
    if((n == 0) || strcmp(t->s, n) == 0) {
      if(!run(t->f, t->s))
        fail = 1;
    }
  }
  if(!fail)
    printf("ALL TESTS PASSED\n");
  else
    printf("SOME TESTS FAILED\n");
  exit(1);   // not reached.
}
