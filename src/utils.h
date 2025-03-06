#pragma once

#ifndef _WIN32
#define stricmp strcasecmp
#endif

extern void reverse(char* s);
extern char* __itoa(int n, char* s);
extern char* trimbuf(char *str);
extern unsigned long host2ip(const char *hostname);
