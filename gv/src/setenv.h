#ifndef _SETENV_H_
#define _SETENV_H_

extern void setenv (
#if NeedFunctionPrototypes
    char*, /* name    */
    char*, /* value   */
    int    /* overwrite */
#endif
);

extern void unsetenv (
#if NeedFunctionPrototypes
    char*  /* name */
#endif
);

#endif /* _SETENV_H_ */

