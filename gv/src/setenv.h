#ifndef setenv_h
#define setenv_h

#define setenv __gnu_setenv
#define unsetenv __gnu_unsetenv

int __gnu_setenv (const char *name, const char *value, int overwrite);
int __gnu_unsetenv (const char *name);

#endif
