#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int __gnu_setenv (const char *name, const char *value, int overwrite)
{
   if (!overwrite && getenv(name))
      return 0;
      
   char *tmp = malloc(strlen(name)+strlen(value)+2);
   strcpy(tmp, name);
   strcat(tmp, "=");
   strcat(tmp, value);
   return putenv(tmp);
   /* The string pointed to by string becomes part of
      the environment, so altering the string changes the environment.
      so we cannot free it. */
}

int __gnu_unsetenv (const char *name) /* Taken from GNULIB, slightly modified */
{
  size_t len;
  char **ep;

  if (name == NULL || *name == '\0' || strchr (name, '=') != NULL)
    {
      errno = EINVAL;
      return -1;
    }

  len = strlen (name);

  ep = environ;
  while (*ep != NULL)
    if (!strncmp (*ep, name, len) && (*ep)[len] == '=')
      {
	/* Found it.  Remove this pointer by moving later ones back.  */
	char **dp = ep;

	do
	  dp[0] = dp[1];
	while (*dp++);
	/* Continue the loop in case NAME appears again.  */
      }
    else
      ++ep;

  return 0;
}
