###############################################
../user.h
###############################################
1a2
> struct rtcdate;
12c13
< int exec(const char*, char**);
---
> int exec(char*, char**);
40,41d40
< int memcmp(const void *, const void *, uint);
< void *memcpy(void *, const void *, uint);

