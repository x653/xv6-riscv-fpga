###############################################
../cat.c
###############################################
14c14
<       fprintf(2, "cat: write error\n");
---
>       printf("cat: write error\n");
19c19
<     fprintf(2, "cat: read error\n");
---
>     printf("cat: read error\n");
31c31
<     exit(0);
---
>     exit(1);
36c36
<       fprintf(2, "cat: cannot open %s\n", argv[i]);
---
>       printf("cat: cannot open %s\n", argv[i]);

###############################################
../echo.c
###############################################

###############################################
../forktest.c
###############################################
8c8
< #define N  1000
---
> #define N  5

###############################################
../grep.c
###############################################
65,66c65
< // The Practice of Programming, Chapter 9, or
< // https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
---
> // The Practice of Programming, Chapter 9.

###############################################
../grind.c
###############################################

###############################################
../init.c
###############################################
5,8d4
< #include "kernel/spinlock.h"
< #include "kernel/sleeplock.h"
< #include "kernel/fs.h"
< #include "kernel/file.h"
20c16
<     mknod("console", CONSOLE, 0);
---
>     mknod("console", 1, 1);
38,51c34,35
< 
<     for(;;){
<       // this call to wait() returns if the shell exits,
<       // or if a parentless process exits.
<       wpid = wait((int *) 0);
<       if(wpid == pid){
<         // the shell exited; restart it.
<         break;
<       } else if(wpid < 0){
<         printf("init: wait returned an error\n");
<         exit(1);
<       } else {
<         // it was a parentless process; do nothing.
<       }
---
>     while((wpid=wait(0)) >= 0 && wpid != pid){
>       //printf("zombie!\n");

###############################################
../kill.c
###############################################

###############################################
../ln.c
###############################################

###############################################
../ls.c
###############################################
45d44
<   case T_DEVICE:

###############################################
../mkdir.c
###############################################

###############################################
../printf.c
###############################################
42c42
< printptr(int fd, uint32 x) {
---
> printptr(int fd, uint64 x) {
46,47c46,47
<   for (i = 0; i < (sizeof(uint32) * 2); i++, x <<= 4)
<     putc(fd, digits[x >> (sizeof(uint32) * 8 - 4)]);
---
>   for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
>     putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
70c70
<         printint(fd, va_arg(ap, uint32), 10, 0);
---
>         printint(fd, va_arg(ap, uint64), 10, 0);
74c74
<         printptr(fd, va_arg(ap, uint32));
---
>         printptr(fd, va_arg(ap, uint64));

###############################################
../rm.c
###############################################

###############################################
../sh.c
###############################################
55d54
< void runcmd(struct cmd*) __attribute__((noreturn));
137c136
<   write(2, "$ ", 2);
---
>   fprintf(2, "$ ");
390c389
<       cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE|O_TRUNC, 1);
---
>       cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);

###############################################
../stressfs.c
###############################################

###############################################
../ulib.c
###############################################
6,16d5
< //
< // wrapper so that it's OK if main() does not call exit().
< //
< void
< _main()
< {
<   extern int main();
<   main();
<   exit(0);
< }
< 
117,125c106,107
<   if (src > dst) {
<     while(n-- > 0)
<       *dst++ = *src++;
<   } else {
<     dst += n;
<     src += n;
<     while(n-- > 0)
<       *--dst = *--src;
<   }
---
>   while(n-- > 0)
>     *dst++ = *src++;
127,146d108
< }
< 
< int
< memcmp(const void *s1, const void *s2, uint n)
< {
<   const char *p1 = s1, *p2 = s2;
<   while (n-- > 0) {
<     if (*p1 != *p2) {
<       return *p1 - *p2;
<     }
<     p1++;
<     p2++;
<   }
<   return 0;
< }
< 
< void *
< memcpy(void *dst, const void *src, uint n)
< {
<   return memmove(dst, src, n);

###############################################
../umalloc.c
###############################################

###############################################
../usertests.c
###############################################
20c20
< #define BUFSZ  ((MAXOPBLOCKS+2)*BSIZE)
---
> #define BUFSZ  (MAXOPBLOCKS+2)*BSIZE
23,421c23
< 
< //
< // Section with tests that run fairly quickly.  Use -q if you want to
< // run just those.  With -q usertests also runs the ones that take a
< // fair of time.
< //
< 
< // what if you pass ridiculous pointers to system calls
< // that read user memory with copyin?
< void
< copyin(char *s)
< {
<   uint32 addrs[] = { 0x80000000LL, 0xffffffffffffffff };
< 
<   for(int ai = 0; ai < 2; ai++){
<     uint32 addr = addrs[ai];
<     
<     int fd = open("copyin1", O_CREATE|O_WRONLY);
<     if(fd < 0){
<       printf("open(copyin1) failed\n");
<       exit(1);
<     }
<     int n = write(fd, (void*)addr, 8192);
<     if(n >= 0){
<       printf("write(fd, %p, 8192) returned %d, not -1\n", addr, n);
<       exit(1);
<     }
<     close(fd);
<     unlink("copyin1");
<     
<     n = write(1, (char*)addr, 8192);
<     if(n > 0){
<       printf("write(1, %p, 8192) returned %d, not -1 or 0\n", addr, n);
<       exit(1);
<     }
<     
<     int fds[2];
<     if(pipe(fds) < 0){
<       printf("pipe() failed\n");
<       exit(1);
<     }
<     n = write(fds[1], (char*)addr, 8192);
<     if(n > 0){
<       printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", addr, n);
<       exit(1);
<     }
<     close(fds[0]);
<     close(fds[1]);
<   }
< }
< 
< // what if you pass ridiculous pointers to system calls
< // that write user memory with copyout?
< void
< copyout(char *s)
< {
<   uint32 addrs[] = { 0x80000000LL, 0xffffffffffffffff };
< 
<   for(int ai = 0; ai < 2; ai++){
<     uint32 addr = addrs[ai];
< 
<     int fd = open("README", 0);
<     if(fd < 0){
<       printf("open(README) failed\n");
<       exit(1);
<     }
<     int n = read(fd, (void*)addr, 8192);
<     if(n > 0){
<       printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", addr, n);
<       exit(1);
<     }
<     close(fd);
< 
<     int fds[2];
<     if(pipe(fds) < 0){
<       printf("pipe() failed\n");
<       exit(1);
<     }
<     n = write(fds[1], "x", 1);
<     if(n != 1){
<       printf("pipe write failed\n");
<       exit(1);
<     }
<     n = read(fds[0], (void*)addr, 8192);
<     if(n > 0){
<       printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", addr, n);
<       exit(1);
<     }
<     close(fds[0]);
<     close(fds[1]);
<   }
< }
< 
< // what if you pass ridiculous string pointers to system calls?
< void
< copyinstr1(char *s)
< {
<   uint32 addrs[] = { 0x80000000LL, 0xffffffffffffffff };
< 
<   for(int ai = 0; ai < 2; ai++){
<     uint32 addr = addrs[ai];
< 
<     int fd = open((char *)addr, O_CREATE|O_WRONLY);
<     if(fd >= 0){
<       printf("open(%p) returned %d, not -1\n", addr, fd);
<       exit(1);
<     }
<   }
< }
< 
< // what if a string system call argument is exactly the size
< // of the kernel buffer it is copied into, so that the null
< // would fall just beyond the end of the kernel buffer?
< void
< copyinstr2(char *s)
< {
<   char b[MAXPATH+1];
< 
<   for(int i = 0; i < MAXPATH; i++)
<     b[i] = 'x';
<   b[MAXPATH] = '\0';
<   
<   int ret = unlink(b);
<   if(ret != -1){
<     printf("unlink(%s) returned %d, not -1\n", b, ret);
<     exit(1);
<   }
< 
<   int fd = open(b, O_CREATE | O_WRONLY);
<   if(fd != -1){
<     printf("open(%s) returned %d, not -1\n", b, fd);
<     exit(1);
<   }
< 
<   ret = link(b, b);
<   if(ret != -1){
<     printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
<     exit(1);
<   }
< 
<   char *args[] = { "xx", 0 };
<   ret = exec(b, args);
<   if(ret != -1){
<     printf("exec(%s) returned %d, not -1\n", b, fd);
<     exit(1);
<   }
< 
<   int pid = fork();
<   if(pid < 0){
<     printf("fork failed\n");
<     exit(1);
<   }
<   if(pid == 0){
<     static char big[PGSIZE+1];
<     for(int i = 0; i < PGSIZE; i++)
<       big[i] = 'x';
<     big[PGSIZE] = '\0';
<     char *args2[] = { big, big, big, 0 };
<     ret = exec("echo", args2);
<     if(ret != -1){
<       printf("exec(echo, BIG) returned %d, not -1\n", fd);
<       exit(1);
<     }
<     exit(747); // OK
<   }
< 
<   int st = 0;
<   wait(&st);
<   if(st != 747){
<     printf("exec(echo, BIG) succeeded, should have failed\n");
<     exit(1);
<   }
< }
< 
< // what if a string argument crosses over the end of last user page?
< void
< copyinstr3(char *s)
< {
<   sbrk(8192);
<   uint32 top = (uint32) sbrk(0);
<   if((top % PGSIZE) != 0){
<     sbrk(PGSIZE - (top % PGSIZE));
<   }
<   top = (uint32) sbrk(0);
<   if(top % PGSIZE){
<     printf("oops\n");
<     exit(1);
<   }
< 
<   char *b = (char *) (top - 1);
<   *b = 'x';
< 
<   int ret = unlink(b);
<   if(ret != -1){
<     printf("unlink(%s) returned %d, not -1\n", b, ret);
<     exit(1);
<   }
< 
<   int fd = open(b, O_CREATE | O_WRONLY);
<   if(fd != -1){
<     printf("open(%s) returned %d, not -1\n", b, fd);
<     exit(1);
<   }
< 
<   ret = link(b, b);
<   if(ret != -1){
<     printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
<     exit(1);
<   }
< 
<   char *args[] = { "xx", 0 };
<   ret = exec(b, args);
<   if(ret != -1){
<     printf("exec(%s) returned %d, not -1\n", b, fd);
<     exit(1);
<   }
< }
< 
< // See if the kernel refuses to read/write user memory that the
< // application doesn't have anymore, because it returned it.
< void
< rwsbrk()
< {
<   int fd, n;
<   
<   uint32 a = (uint32) sbrk(8192);
< 
<   if(a == 0xffffffffffffffffLL) {
<     printf("sbrk(rwsbrk) failed\n");
<     exit(1);
<   }
<   
<   if ((uint32) sbrk(-8192) ==  0xffffffffffffffffLL) {
<     printf("sbrk(rwsbrk) shrink failed\n");
<     exit(1);
<   }
< 
<   fd = open("rwsbrk", O_CREATE|O_WRONLY);
<   if(fd < 0){
<     printf("open(rwsbrk) failed\n");
<     exit(1);
<   }
<   n = write(fd, (void*)(a+4096), 1024);
<   if(n >= 0){
<     printf("write(fd, %p, 1024) returned %d, not -1\n", a+4096, n);
<     exit(1);
<   }
<   close(fd);
<   unlink("rwsbrk");
< 
<   fd = open("README", O_RDONLY);
<   if(fd < 0){
<     printf("open(rwsbrk) failed\n");
<     exit(1);
<   }
<   n = read(fd, (void*)(a+4096), 10);
<   if(n >= 0){
<     printf("read(fd, %p, 10) returned %d, not -1\n", a+4096, n);
<     exit(1);
<   }
<   close(fd);
<   
<   exit(0);
< }
< 
< // test O_TRUNC.
< void
< truncate1(char *s)
< {
<   char buf[32];
<   
<   unlink("truncfile");
<   int fd1 = open("truncfile", O_CREATE|O_WRONLY|O_TRUNC);
<   write(fd1, "abcd", 4);
<   close(fd1);
< 
<   int fd2 = open("truncfile", O_RDONLY);
<   int n = read(fd2, buf, sizeof(buf));
<   if(n != 4){
<     printf("%s: read %d bytes, wanted 4\n", s, n);
<     exit(1);
<   }
< 
<   fd1 = open("truncfile", O_WRONLY|O_TRUNC);
< 
<   int fd3 = open("truncfile", O_RDONLY);
<   n = read(fd3, buf, sizeof(buf));
<   if(n != 0){
<     printf("aaa fd3=%d\n", fd3);
<     printf("%s: read %d bytes, wanted 0\n", s, n);
<     exit(1);
<   }
< 
<   n = read(fd2, buf, sizeof(buf));
<   if(n != 0){
<     printf("bbb fd2=%d\n", fd2);
<     printf("%s: read %d bytes, wanted 0\n", s, n);
<     exit(1);
<   }
<   
<   write(fd1, "abcdef", 6);
< 
<   n = read(fd3, buf, sizeof(buf));
<   if(n != 6){
<     printf("%s: read %d bytes, wanted 6\n", s, n);
<     exit(1);
<   }
< 
<   n = read(fd2, buf, sizeof(buf));
<   if(n != 2){
<     printf("%s: read %d bytes, wanted 2\n", s, n);
<     exit(1);
<   }
< 
<   unlink("truncfile");
< 
<   close(fd1);
<   close(fd2);
<   close(fd3);
< }
< 
< // write to an open FD whose file has just been truncated.
< // this causes a write at an offset beyond the end of the file.
< // such writes fail on xv6 (unlike POSIX) but at least
< // they don't crash.
< void
< truncate2(char *s)
< {
<   unlink("truncfile");
< 
<   int fd1 = open("truncfile", O_CREATE|O_TRUNC|O_WRONLY);
<   write(fd1, "abcd", 4);
< 
<   int fd2 = open("truncfile", O_TRUNC|O_WRONLY);
< 
<   int n = write(fd1, "x", 1);
<   if(n != -1){
<     printf("%s: write returned %d, expected -1\n", s, n);
<     exit(1);
<   }
< 
<   unlink("truncfile");
<   close(fd1);
<   close(fd2);
< }
< 
< void
< truncate3(char *s)
< {
<   int pid, xstatus;
< 
<   close(open("truncfile", O_CREATE|O_TRUNC|O_WRONLY));
<   
<   pid = fork();
<   if(pid < 0){
<     printf("%s: fork failed\n", s);
<     exit(1);
<   }
< 
<   if(pid == 0){
<     for(int i = 0; i < 100; i++){
<       char buf[32];
<       int fd = open("truncfile", O_WRONLY);
<       if(fd < 0){
<         printf("%s: open failed\n", s);
<         exit(1);
<       }
<       int n = write(fd, "1234567890", 10);
<       if(n != 10){
<         printf("%s: write got %d, expected 10\n", s, n);
<         exit(1);
<       }
<       close(fd);
<       fd = open("truncfile", O_RDONLY);
<       read(fd, buf, sizeof(buf));
<       close(fd);
<     }
<     exit(0);
<   }
< 
<   for(int i = 0; i < 150; i++){
<     int fd = open("truncfile", O_CREATE|O_WRONLY|O_TRUNC);
<     if(fd < 0){
<       printf("%s: open failed\n", s);
<       exit(1);
<     }
<     int n = write(fd, "xxx", 3);
<     if(n != 3){
<       printf("%s: write got %d, expected 3\n", s, n);
<       exit(1);
<     }
<     close(fd);
<   }
< 
<   wait(&xstatus);
<   unlink("truncfile");
<   exit(xstatus);
< }
<   
---
> char name[3];
551c153
<       printf("%s: error: write aa %d new file failed\n", s, i);
---
>       printf("%s: error: write aa %d new file failed\n", i);
555c157
<       printf("%s: error: write bb %d new file failed\n", s, i);
---
>       printf("%s: error: write bb %d new file failed\n", i);
592c194
<       printf("%s: error: write big file failed\n", s, i);
---
>       printf("%s: error: write big file failed\n", i);
610c212
<         printf("%s: read only %d blocks from big", s, n);
---
>         printf("%s: read only %d blocks from big", n);
615c217
<       printf("%s: read failed %d\n", s, i);
---
>       printf("%s: read failed %d\n", i);
619c221
<       printf("%s: read content of block %d is %d\n", s,
---
>       printf("%s: read content of block %d is %d\n",
639d240
<   char name[3];
656a258,259
>   printf("mkdir test\n");
> 
675a279
>   printf("%s: mkdir test ok\n");
788,817d391
< 
< // test if child is killed (status = -1)
< void
< killstatus(char *s)
< {
<   int xst;
<   
<   for(int i = 0; i < 100; i++){
<     int pid1 = fork();
<     if(pid1 < 0){
<       printf("%s: fork failed\n", s);
<       exit(1);
<     }
<     if(pid1 == 0){
<       while(1) {
<         getpid();
<       }
<       exit(0);
<     }
<     sleep(1);
<     kill(pid1);
<     wait(&xst);
<     if(xst != -1) {
<        printf("%s: status should be -1\n", s);
<        exit(1);
<     }
<   }
<   exit(0);
< }
< 
827c401
<     printf("%s: fork failed", s);
---
>     printf("%s: fork failed");
852c426
<       printf("%s: preempt write error", s);
---
>       printf("%s: preempt write error");
860c434
<     printf("%s: preempt read error", s);
---
>     printf("%s: preempt read error");
1035c609
<   for(int i = 0; i < 800; i++){
---
>   for(int i = 0; i < 1; i++){
1080,1084d653
<     if(xstatus == -1){
<       // probably page fault, so might be lazy lab,
<       // so OK.
<       exit(0);
<     }
1381c950
<     printf("%s: link non-existent succeeded! oops\n", s);
---
>     printf("%s: link non-existant succeeded! oops\n", s);
1472,1473d1040
<       close(open(file, 0));
<       close(open(file, 0));
1479,1480d1045
<       unlink(file);
<       unlink(file);
1520a1086,1125
> // directory that uses indirect blocks
> void
> bigdir(char *s)
> {
>   enum { N = 500 };
>   int i, fd;
>   char name[10];
> 
>   unlink("bd");
> 
>   fd = open("bd", O_CREATE);
>   if(fd < 0){
>     printf("%s: bigdir create failed\n", s);
>     exit(1);
>   }
>   close(fd);
> 
>   for(i = 0; i < N; i++){
>     name[0] = 'x';
>     name[1] = '0' + (i / 64);
>     name[2] = '0' + (i % 64);
>     name[3] = '\0';
>     if(link("bd", name) != 0){
>       printf("%s: bigdir link failed\n", s);
>       exit(1);
>     }
>   }
> 
>   unlink("bd");
>   for(i = 0; i < N; i++){
>     name[0] = 'x';
>     name[1] = '0' + (i / 64);
>     name[2] = '0' + (i % 64);
>     name[3] = '\0';
>     if(unlink(name) != 0){
>       printf("%s: bigdir unlink failed", s);
>       exit(1);
>     }
>   }
> }
1710c1315,1316
<     if(fd < 0){
---
>     printf("bigwrite: fd=%d sz=%d\n",fd,sz);
> 	if(fd < 0){
1727d1332
< 
1734,1735c1339,1340
<   unlink("bigfile.dat");
<   fd = open("bigfile.dat", O_CREATE | O_RDWR);
---
>   unlink("bigfile");
>   fd = open("bigfile", O_CREATE | O_RDWR);
1749c1354
<   fd = open("bigfile.dat", 0);
---
>   fd = open("bigfile", 0);
1778c1383
<   unlink("bigfile.dat");
---
>   unlink("bigfile");
1817,1824d1421
< 
<   // clean up
<   unlink("123456789012345/12345678901234");
<   unlink("12345678901234/12345678901234");
<   unlink("12345678901234/12345678901234/12345678901234");
<   unlink("123456789012345/123456789012345/123456789012345");
<   unlink("12345678901234/123456789012345");
<   unlink("12345678901234");
1919,1920c1516
< // test that iput() is called at the end of _namei().
< // also tests empty file names.
---
> // test that iput() is called at the end of _namei()
1947,1952d1542
<   // clean up
<   for(i = 0; i < NINODE + 1; i++){
<     chdir("..");
<     unlink("irefd");
<   }
< 
2004,2028c1594,1596
<   pid = fork();
<   if(pid < 0){
<     printf("fork failed in sbrkbasic\n");
<     exit(1);
<   }
<   if(pid == 0){
<     a = sbrk(TOOMUCH);
<     if(a == (char*)0xffffffffffffffffL){
<       // it's OK if this fails.
<       exit(0);
<     }
<     
<     for(b = a; b < a+TOOMUCH; b += 4096){
<       *b = 99;
<     }
<     
<     // we should not get here! either sbrk(TOOMUCH)
<     // should have failed, or (with lazy allocation)
<     // a pagefault should have killed this process.
<     exit(1);
<   }
< 
<   wait(&xstatus);
<   if(xstatus == 1){
<     printf("%s: too much memory allocated!\n", s);
---
>   a = sbrk(TOOMUCH);
>   if(a != (char*)0xffffffffffffffffL){
>     printf("%s: sbrk(<toomuch>) returned %p\n", a);
2037c1605
<       printf("%s: sbrk test failed %d %x %x\n", s, i, a, b);
---
>       printf("%s: sbrk test failed %d %x %x\n", i, a, b);
2077,2082d1644
< 
<   // touch each page to make sure it exists.
<   char *eee = sbrk(0);
<   for(char *pp = a; pp < eee; pp += 4096)
<     *pp = 1;
< 
2095c1657
<     printf("%s: sbrk deallocation produced wrong address, a %x c %x\n", s, a, c);
---
>     printf("%s: sbrk deallocation produced wrong address, a %x c %x\n", a, c);
2103c1665
<     printf("%s: sbrk re-allocation failed, a %x c %x\n", s, a, c);
---
>     printf("%s: sbrk re-allocation failed, a %x c %x\n", a, c);
2115c1677
<     printf("%s: sbrk downsize failed, a %x c %x\n", s, a, c);
---
>     printf("%s: sbrk downsize failed, a %x c %x\n", a, c);
2134,2158c1696
<       printf("%s: oops could read %x = %x\n", s, a, *a);
<       exit(1);
<     }
<     int xstatus;
<     wait(&xstatus);
<     if(xstatus != -1)  // did kernel kill child?
<       exit(1);
<   }
< }
< 
< // user code should not be able to write to addresses above MAXVA.
< void
< MAXVAplus(char *s)
< {
<   volatile uint32 a = MAXVA;
<   for( ; a != 0; a <<= 1){
<     int pid;
<     pid = fork();
<     if(pid < 0){
<       printf("%s: fork failed\n", s);
<       exit(1);
<     }
<     if(pid == 0){
<       *(char*)a = 99;
<       printf("%s: oops wrote %x\n", s, a);
---
>       printf("%s: oops could read %x = %x\n", a, *a);
2218,2220c1756
<     // allocate a lot of memory.
<     // this should produce a page fault,
<     // and thus not complete.
---
>     // allocate a lot of memory
2227,2229c1763
<     // print n so the compiler doesn't optimize away
<     // the for loop.
<     printf("%s: allocate a lot of memory succeeded %d\n", s, n);
---
>     printf("%s: allocate a lot of memory succeeded %d\n", n);
2233c1767
<   if(xstatus != -1 && xstatus != 2)
---
>   if(xstatus != -1)
2282c1816
< // does uninitialized data start out zero?
---
> // does unintialized data start out zero?
2351c1885
<     printf("writing %s\n", name);
---
>     printf("%s: writing %s\n", name);
2354c1888
<       printf("open %s failed\n", name);
---
>       printf("%s: open %s failed\n", name);
2365c1899
<     printf("wrote %d bytes\n", total);
---
>     printf("%s: wrote %d bytes\n", total);
2397a1932,1939
> unsigned long randstate = 1;
> unsigned int
> rand()
> {
>   randstate = randstate * 1664525 + 1013904223;
>   return randstate;
> }
> 
2411,2434c1953
<     printf("%s: stacktest: read below stack %p\n", s, *sp);
<     exit(1);
<   } else if(pid < 0){
<     printf("%s: fork failed\n", s);
<     exit(1);
<   }
<   wait(&xstatus);
<   if(xstatus == -1)  // kernel killed child?
<     exit(0);
<   else
<     exit(xstatus);
< }
< 
< // check that writes to text segment fault
< void
< textwrite(char *s)
< {
<   int pid;
<   int xstatus;
<   
<   pid = fork();
<   if(pid == 0) {
<     volatile int *addr = (int *) 0;
<     *addr = 10;
---
>     printf("%s: stacktest: read below stack %p\n", *sp);
2450d1968
< void *big = (void*) 0xeaeb0b5b00002f5e;
2456,2457c1974,1976
<   exec(big, argv);
<   pipe(big);
---
>   exec((char*)0xeaeb0b5b00002f5e, argv);
> 
>   pipe((int*)0xeaeb0b5b00002f5e);
2520,2740d2038
< // if process size was somewhat more than a page boundary, and then
< // shrunk to be somewhat less than that page boundary, can the kernel
< // still copyin() from addresses in the last page?
< void
< sbrklast(char *s)
< {
<   uint32 top = (uint32) sbrk(0);
<   if((top % 4096) != 0)
<     sbrk(4096 - (top % 4096));
<   sbrk(4096);
<   sbrk(10);
<   sbrk(-20);
<   top = (uint32) sbrk(0);
<   char *p = (char *) (top - 64);
<   p[0] = 'x';
<   p[1] = '\0';
<   int fd = open(p, O_RDWR|O_CREATE);
<   write(fd, p, 1);
<   close(fd);
<   fd = open(p, O_RDWR);
<   p[0] = '\0';
<   read(fd, p, 1);
<   if(p[0] != 'x')
<     exit(1);
< }
< 
< 
< // does sbrk handle signed int32 wrap-around with
< // negative arguments?
< void
< sbrk8000(char *s)
< {
<   sbrk(0x80000004);
<   volatile char *top = sbrk(0);
<   *(top-1) = *(top-1) + 1;
< }
< 
< 
< 
< // regression test. test whether exec() leaks memory if one of the
< // arguments is invalid. the test passes if the kernel doesn't panic.
< void
< badarg(char *s)
< {
<   for(int i = 0; i < 50000; i++){
<     char *argv[2];
<     argv[0] = (char*)0xffffffff;
<     argv[1] = 0;
<     exec("echo", argv);
<   }
<   
<   exit(0);
< }
< 
< struct test {
<   void (*f)(char *);
<   char *s;
< } quicktests[] = {
<   {copyin, "copyin"},
<   {copyout, "copyout"},
<   {copyinstr1, "copyinstr1"},
<   {copyinstr2, "copyinstr2"},
<   {copyinstr3, "copyinstr3"},
<   {rwsbrk, "rwsbrk" },
<   {truncate1, "truncate1"},
<   {truncate2, "truncate2"},
<   {truncate3, "truncate3"},
<   {openiputtest, "openiput"},
<   {exitiputtest, "exitiput"},
<   {iputtest, "iput"},
<   {opentest, "opentest"},
<   {writetest, "writetest"},
<   {writebig, "writebig"},
<   {createtest, "createtest"},
<   {dirtest, "dirtest"},
<   {exectest, "exectest"},
<   {pipe1, "pipe1"},
<   {killstatus, "killstatus"},
<   {preempt, "preempt"},
<   {exitwait, "exitwait"},
<   {reparent, "reparent" },
<   {twochildren, "twochildren"},
<   {forkfork, "forkfork"},
<   {forkforkfork, "forkforkfork"},
<   {reparent2, "reparent2"},
<   {mem, "mem"},
<   {sharedfd, "sharedfd"},
<   {fourfiles, "fourfiles"},
<   {createdelete, "createdelete"},
<   {unlinkread, "unlinkread"},
<   {linktest, "linktest"},
<   {concreate, "concreate"},
<   {linkunlink, "linkunlink"},
<   {subdir, "subdir"},
<   {bigwrite, "bigwrite"},
<   {bigfile, "bigfile"},
<   {fourteen, "fourteen"},
<   {rmdot, "rmdot"},
<   {dirfile, "dirfile"},
<   {iref, "iref"},
<   {forktest, "forktest"},
<   {sbrkbasic, "sbrkbasic"},
<   {sbrkmuch, "sbrkmuch"},
<   {kernmem, "kernmem"},
<   {MAXVAplus, "MAXVAplus"},
<   {sbrkfail, "sbrkfail"},
<   {sbrkarg, "sbrkarg"},
<   {validatetest, "validatetest"},
<   {bsstest, "bsstest"},
<   {bigargtest, "bigargtest"},
<   {argptest, "argptest"},
<   {stacktest, "stacktest"},
<   {textwrite, "textwrite"},
<   {pgbug, "pgbug" },
<   {sbrkbugs, "sbrkbugs" },
<   {sbrklast, "sbrklast"},
<   {sbrk8000, "sbrk8000"},
<   {badarg, "badarg" },
< 
<   { 0, 0},
< };
< 
< //
< // Section with tests that take a fair bit of time
< //
< 
< // directory that uses indirect blocks
< void
< bigdir(char *s)
< {
<   enum { N = 500 };
<   int i, fd;
<   char name[10];
< 
<   unlink("bd");
< 
<   fd = open("bd", O_CREATE);
<   if(fd < 0){
<     printf("%s: bigdir create failed\n", s);
<     exit(1);
<   }
<   close(fd);
< 
<   for(i = 0; i < N; i++){
<     name[0] = 'x';
<     name[1] = '0' + (i / 64);
<     name[2] = '0' + (i % 64);
<     name[3] = '\0';
<     if(link("bd", name) != 0){
<       printf("%s: bigdir link(bd, %s) failed\n", s, name);
<       exit(1);
<     }
<   }
< 
<   unlink("bd");
<   for(i = 0; i < N; i++){
<     name[0] = 'x';
<     name[1] = '0' + (i / 64);
<     name[2] = '0' + (i % 64);
<     name[3] = '\0';
<     if(unlink(name) != 0){
<       printf("%s: bigdir unlink failed", s);
<       exit(1);
<     }
<   }
< }
< 
< // concurrent writes to try to provoke deadlock in the virtio disk
< // driver.
< void
< manywrites(char *s)
< {
<   int nchildren = 4;
<   int howmany = 30; // increase to look for deadlock
<   
<   for(int ci = 0; ci < nchildren; ci++){
<     int pid = fork();
<     if(pid < 0){
<       printf("fork failed\n");
<       exit(1);
<     }
< 
<     if(pid == 0){
<       char name[3];
<       name[0] = 'b';
<       name[1] = 'a' + ci;
<       name[2] = '\0';
<       unlink(name);
<       
<       for(int iters = 0; iters < howmany; iters++){
<         for(int i = 0; i < ci+1; i++){
<           int fd = open(name, O_CREATE | O_RDWR);
<           if(fd < 0){
<             printf("%s: cannot create %s\n", s, name);
<             exit(1);
<           }
<           int sz = sizeof(buf);
<           int cc = write(fd, buf, sz);
<           if(cc != sz){
<             printf("%s: write(%d) ret %d\n", s, sz, cc);
<             exit(1);
<           }
<           close(fd);
<         }
<         unlink(name);
<       }
< 
<       unlink(name);
<       exit(0);
<     }
<   }
< 
<   for(int ci = 0; ci < nchildren; ci++){
<     int st = 0;
<     wait(&st);
<     if(st != 0)
<       exit(st);
<   }
<   exit(0);
< }
< 
2778,2780c2076,2077
< // test the exec() code that cleans up if it runs out
< // of memory. it's really a test that such a condition
< // doesn't cause a panic.
---
> // regression test. test whether exec() leaks memory if one of the
> // arguments is invalid. the test passes if the kernel doesn't panic.
2782c2079
< execout(char *s)
---
> badarg(char *s)
2784,2809c2081,2085
<   for(int avail = 0; avail < 15; avail++){
<     int pid = fork();
<     if(pid < 0){
<       printf("fork failed\n");
<       exit(1);
<     } else if(pid == 0){
<       // allocate all of memory.
<       while(1){
<         uint32 a = (uint32) sbrk(4096);
<         if(a == 0xffffffffffffffffLL)
<           break;
<         *(char*)(a + 4096 - 1) = 1;
<       }
< 
<       // free a few pages, in order to let exec() make some
<       // progress.
<       for(int i = 0; i < avail; i++)
<         sbrk(-4096);
<       
<       close(1);
<       char *args[] = { "echo", "x", 0 };
<       exec("echo", args);
<       exit(0);
<     } else {
<       wait((int*)0);
<     }
---
>   for(int i = 0; i < 50000; i++){
>     char *argv[2];
>     argv[0] = (char*)0xffffffff;
>     argv[1] = 0;
>     exec("echo", argv);
2811,2822d2086
< 
<   exit(0);
< }
< 
< // can the kernel tolerate running out of disk space?
< void
< diskfull(char *s)
< {
<   int fi;
<   int done = 0;
< 
<   unlink("diskfulldir");
2824,2924c2088
<   for(fi = 0; done == 0; fi++){
<     char name[32];
<     name[0] = 'b';
<     name[1] = 'i';
<     name[2] = 'g';
<     name[3] = '0' + fi;
<     name[4] = '\0';
<     unlink(name);
<     int fd = open(name, O_CREATE|O_RDWR|O_TRUNC);
<     if(fd < 0){
<       // oops, ran out of inodes before running out of blocks.
<       printf("%s: could not create file %s\n", s, name);
<       done = 1;
<       break;
<     }
<     for(int i = 0; i < MAXFILE; i++){
<       char buf[BSIZE];
<       if(write(fd, buf, BSIZE) != BSIZE){
<         done = 1;
<         close(fd);
<         break;
<       }
<     }
<     close(fd);
<   }
< 
<   // now that there are no free blocks, test that dirlink()
<   // merely fails (doesn't panic) if it can't extend
<   // directory content. one of these file creations
<   // is expected to fail.
<   int nzz = 128;
<   for(int i = 0; i < nzz; i++){
<     char name[32];
<     name[0] = 'z';
<     name[1] = 'z';
<     name[2] = '0' + (i / 32);
<     name[3] = '0' + (i % 32);
<     name[4] = '\0';
<     unlink(name);
<     int fd = open(name, O_CREATE|O_RDWR|O_TRUNC);
<     if(fd < 0)
<       break;
<     close(fd);
<   }
< 
<   // this mkdir() is expected to fail.
<   if(mkdir("diskfulldir") == 0)
<     printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n");
< 
<   unlink("diskfulldir");
< 
<   for(int i = 0; i < nzz; i++){
<     char name[32];
<     name[0] = 'z';
<     name[1] = 'z';
<     name[2] = '0' + (i / 32);
<     name[3] = '0' + (i % 32);
<     name[4] = '\0';
<     unlink(name);
<   }
< 
<   for(int i = 0; i < fi; i++){
<     char name[32];
<     name[0] = 'b';
<     name[1] = 'i';
<     name[2] = 'g';
<     name[3] = '0' + i;
<     name[4] = '\0';
<     unlink(name);
<   }
< }
< 
< void
< outofinodes(char *s)
< {
<   int nzz = 32*32;
<   for(int i = 0; i < nzz; i++){
<     char name[32];
<     name[0] = 'z';
<     name[1] = 'z';
<     name[2] = '0' + (i / 32);
<     name[3] = '0' + (i % 32);
<     name[4] = '\0';
<     unlink(name);
<     int fd = open(name, O_CREATE|O_RDWR|O_TRUNC);
<     if(fd < 0){
<       // failure is eventually expected.
<       break;
<     }
<     close(fd);
<   }
< 
<   for(int i = 0; i < nzz; i++){
<     char name[32];
<     name[0] = 'z';
<     name[1] = 'z';
<     name[2] = '0' + (i / 32);
<     name[3] = '0' + (i % 32);
<     name[4] = '\0';
<     unlink(name);
<   }
---
>   exit(0);
2927,2941d2090
< struct test slowtests[] = {
<   {bigdir, "bigdir"},
<   {manywrites, "manywrites"},
<   {badwrite, "badwrite" },
<   {execout, "execout"},
<   {diskfull, "diskfull"},
<   {outofinodes, "outofinodes"},
<     
<   { 0, 0},
< };
< 
< //
< // drive tests
< //
< 
2948c2097
< 
---
>   
2960c2109
<       printf("FAILED\n");
---
>       printf("FAILED\n", s);
2962c2111
<       printf("OK\n");
---
>       printf("OK\n", s);
2968,2988c2117
< runtests(struct test *tests, char *justone) {
<   for (struct test *t = tests; t->s != 0; t++) {
<     if((justone == 0) || strcmp(t->s, justone) == 0) {
<       if(!run(t->f, t->s)){
<         printf("SOME TESTS FAILED\n");
<         return 1;
<       }
<     }
<   }
<   return 0;
< }
< 
< 
< //
< // use sbrk() to count how many free physical memory pages there are.
< // touches the pages to force allocation.
< // because out of memory with lazy allocation results in the process
< // taking a fault and being killed, fork and report back.
< //
< int
< countfree()
---
> main(int argc, char *argv[])
2990,3005c2119,2176
<   int fds[2];
< 
<   if(pipe(fds) < 0){
<     printf("pipe() failed in countfree()\n");
<     exit(1);
<   }
<   
<   int pid = fork();
< 
<   if(pid < 0){
<     printf("fork failed in countfree()\n");
<     exit(1);
<   }
< 
<   if(pid == 0){
<     close(fds[0]);
---
>   char *n = 0;
>   if(argc > 1) {
>     n = argv[1];
>   }
>   
>   struct test {
>     void (*f)(char *);
>     char *s;
>   } tests[] = {
>     //{reparent2, "reparent2"},
>     {pgbug, "pgbug" },
>   //  {sbrkbugs, "sbrkbugs" },
>     // {badwrite, "badwrite" },
>   //  {badarg, "badarg" },
>   //  {reparent, "reparent" },
>   //  {twochildren, "twochildren"},
>     //{forkfork, "forkfork"},
>     //{forkforkfork, "forkforkfork"},
>     {argptest, "argptest"},
>     //{createdelete, "createdelete"},
>     //{linkunlink, "linkunlink"},
>     {linktest, "linktest"},
>     {unlinkread, "unlinkread"},
>     //{concreate, "concreate"},
>     {subdir, "subdir"},
> //    {fourfiles, "fourfiles"},
>  //   {sharedfd, "sharedfd"},
> //    {exectest, "exectest"},
> //    {bigargtest, "bigargtest"},
>     {bigwrite, "bigwrite"},
>     {bsstest, "bsstest"},
>   //  {sbrkbasic, "sbrkbasic"},
>     {sbrkmuch, "sbrkmuch"},
> //    {kernmem, "kernmem"},
>  //   {sbrkfail, "sbrkfail"},
>     {sbrkarg, "sbrkarg"},
>     {validatetest, "validatetest"},
>   //  {stacktest, "stacktest"},
>     {opentest, "opentest"},
>     {writetest, "writetest"},
>     {writebig, "writebig"},
>     {createtest, "createtest"},
>     //{openiputtest, "openiput"},
>     //{exitiputtest, "exitiput"},
>     {iputtest, "iput"},
>     //{mem, "mem"},
>     //{pipe1, "pipe1"},
>     //{preempt, "preempt"},
>     //{exitwait, "exitwait"},
>     {rmdot, "rmdot"},
>     {fourteen, "fourteen"},
>     {bigfile, "bigfile"},
>     {dirfile, "dirfile"},
>     {iref, "iref"},
>     //{forktest, "forktest"},
>     {bigdir, "bigdir"}, // slow
>     { 0, 0},
>   };
3007,3011c2178
<     while(1){
<       uint32 a = (uint32) sbrk(4096);
<       if(a == 0xffffffffffffffff){
<         break;
<       }
---
>   printf("usertests starting\n");
3013,3038c2180,2182
<       // modify the memory to make sure it's really allocated.
<       *(char *)(a + 4096 - 1) = 1;
< 
<       // report back one more page.
<       if(write(fds[1], "x", 1) != 1){
<         printf("write() failed in countfree()\n");
<         exit(1);
<       }
<     }
< 
<     exit(0);
<   }
< 
<   close(fds[1]);
< 
<   int n = 0;
<   while(1){
<     char c;
<     int cc = read(fds[0], &c, 1);
<     if(cc < 0){
<       printf("read() failed in countfree()\n");
<       exit(1);
<     }
<     if(cc == 0)
<       break;
<     n += 1;
---
>   if(open("usertests.ran", 0) >= 0){
>     printf("already ran user tests -- rebuild fs.img (rm fs.img; make fs.img)\n");
>     exit(1);
3039a2184
>   close(open("usertests.ran", O_CREATE));
3041,3071c2186,2190
<   close(fds[0]);
<   wait((int*)0);
<   
<   return n;
< }
< 
< int
< drivetests(int quick, int continuous, char *justone) {
<   do {
<     printf("usertests starting\n");
<     int free0 = countfree();
<     int free1 = 0;
<     if (runtests(quicktests, justone)) {
<       if(continuous != 2) {
<         return 1;
<       }
<     }
<     if(!quick) {
<       if (justone == 0)
<         printf("usertests slow tests starting\n");
<       if (runtests(slowtests, justone)) {
<         if(continuous != 2) {
<           return 1;
<         }
<       }
<     }
<     if((free1 = countfree()) < free0) {
<       printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
<       if(continuous != 2) {
<         return 1;
<       }
---
>   int fail = 0;
>   for (struct test *t = tests; t->s != 0; t++) {
>     if((n == 0) || strcmp(t->s, n) == 0) {
>       if(!run(t->f, t->s))
>         fail = 1;
3073,3094d2191
<   } while(continuous);
<   return 0;
< }
< 
< int
< main(int argc, char *argv[])
< {
<   int continuous = 0;
<   int quick = 0;
<   char *justone = 0;
< 
<   if(argc == 2 && strcmp(argv[1], "-q") == 0){
<     quick = 1;
<   } else if(argc == 2 && strcmp(argv[1], "-c") == 0){
<     continuous = 1;
<   } else if(argc == 2 && strcmp(argv[1], "-C") == 0){
<     continuous = 2;
<   } else if(argc == 2 && argv[1][0] != '-'){
<     justone = argv[1];
<   } else if(argc > 1){
<     printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
<     exit(1);
3096,3100c2193,2197
<   if (drivetests(quick, continuous, justone)) {
<     exit(1);
<   }
<   printf("ALL TESTS PASSED\n");
<   exit(0);
---
>   if(!fail)
>     printf("ALL TESTS PASSED\n");
>   else
>     printf("SOME TESTS FAILED\n");
>   exit(1);   // not reached.

###############################################
../wc.c
###############################################

###############################################
../zombie.c
###############################################
12c12
<     sleep(5);  // Let child exit before parent.
---
>     sleep(50);  // Let child exit before parent.

