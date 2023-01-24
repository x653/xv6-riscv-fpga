#define NPROC         8  // maximum number of processes
#define NCPU          4  // maximum number of CPUs
#define NOFILE        8  // open files per process
#define NFILE        16  // open files per system
#define NINODE        8  // maximum number of active i-nodes
#define NDEV          8  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       16  // max exec arguments
#define MAXOPBLOCKS  16  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       80000  // size of file system in blocks
#define MAXPATH      64   // maximum file path name
