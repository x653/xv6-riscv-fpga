#define NPROC         8  // maximum number of processes
#define NCPU          1  // maximum number of CPUs
#define NOFILE        8  // open files per process
#define NFILE         8  // open files per system
#define NINODE        8  // maximum number of active i-nodes
#define NDEV          8  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       16  // max exec arguments
#define MAXOPBLOCKS  16  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS)  // size of disk block cache
#define FSSIZE       2000  // size of file system in blocks
#define MAXPATH      32   // maximum file path name
