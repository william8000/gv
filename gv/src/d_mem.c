/*
**
** d_mem.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "paths.h"
#include INC_X11(Intrinsic.h) 

#include "d_mem.h"

#define GV_MEMDEBUG_OUT stderr

typedef struct _mem {
   int number;
   unsigned long adress;
} adress;

#define MAX_ADR 2000
static adress ad[MAX_ADR];
static int num = 0;

#define MEMDEBUG_MESSAGE(ppp)                                   \
   fprintf (                                                    \
      GV_MEMDEBUG_OUT,                                          \
      "%10s | %4s | %4s | %4s | %4s | %4s | %18s %4d\n",         \
      ppp            ,                                          \
      mem_num        ,                                          \
      mem_matchnum   ,                                          \
      call_type      ,                                          \
      mem_routine    ,                                          \
      mem_size       ,                                          \
      call_module    ,                                          \
      call_line                                                 \
   )

#define USE_mem_AND_call_VARIABLES \
   char mem_inptr[20];             \
   char mem_outptr[20];            \
   char mem_num[10];               \
   char mem_matchnum[10];          \
   char mem_size[10];              \
   char *mem_routine;              \
   char *call_type;                \
   char call_module[40];           \
   int  call_line

#define FORMAT_call_module(fff)			\
   {						\
     char *pos = strrchr((fff),']');		\
     if (!pos) pos = strrchr((fff),':');	\
     if (!pos) pos = (fff);			\
     else pos++;				\
     strcpy(call_module,pos);                   \
     pos = strrchr(call_module,'.');            \
     if (pos) *pos = '\0';                      \
   }

#define FORMAT_mem_size(sss)			\
   {						\
     if (sss<0) mem_size[0]='\0';		\
     else sprintf(mem_size,"%4d",(sss));	\
   }

#define FORMAT_mem_inptr(ppp)			\
   {						\
     sprintf(mem_inptr,"- %8lX",(long unsigned int)(ppp));\
   }

#define FORMAT_mem_outptr(ppp)			\
   {						\
     sprintf(mem_outptr,"+ %8lX",(long unsigned int)(ppp));\
   }

#define FORMAT_mem_num				\
   {						\
     sprintf(mem_num,"%4d",num);		\
   }

#define FORMAT_mem_matchnum(nnn)		\
   {						\
     if (nnn<0) mem_matchnum[0]='\0';		\
     else sprintf(mem_matchnum,"%4d",nnn);	\
   }

#define INIT_STACK                                                              \
   if (num==0) {                                                                \
      while (num<MAX_ADR) { ad[num].number=-1; num++; }                         \
      num=0;                                                                    \
      ad[1].number=-2;                                                          \
      fprintf(GV_MEMDEBUG_OUT,							\
         "    Adress |  Num | Corr | Type | Func | Size |               File Line\n");\
   }

#define ADD_TO_STACK(aaa)                                                         \
   {                                                                              \
     int i=0;                                                                     \
     while (i<MAX_ADR && ad[i].number >=0) ++i;                                   \
     if (i==MAX_ADR)                                                              \
        fprintf(GV_MEMDEBUG_OUT,"D_MEM.C: Warning, stack too small.\n");        \
     else {                                                                       \
        if (ad[i].number == -2) {                                                 \
           if (i==MAX_ADR-1)                                                      \
              fprintf(GV_MEMDEBUG_OUT,"D_MEM.C: Warning, stack exhausted.\n");  \
           else                                                                   \
              ad[i+1].number = -2;                                                \
        }                                                                         \
        ad[i].number = num;                                                       \
        ad[i].adress = (unsigned long) aaa;                                       \
     }                                                                            \
   }

#define INCREMENT_STACK ++num

#define GET_mem_matchnum(ppp)                                             \
   {                                                                      \
     int i=0;                                                             \
     mem_matchnum[0] = '\0';                                              \
     while (ad[i].number > -2 && i<MAX_ADR) {                             \
         if (ad[i].number > -1 && ad[i].adress == (unsigned long) ppp) {  \
            FORMAT_mem_matchnum(ad[i].number)				  \
            ad[i].number = -1;                                            \
            break;                                                        \
         }                                                                \
         ++i;                                                             \
     }                                                                    \
   }


void *d_malloc(size,line,file,caller)
   size_t size;
   int  line;
   char *file;
   char *caller;
{
   char *outptr;
   USE_mem_AND_call_VARIABLES;

   INIT_STACK;

   outptr = malloc(size);
   
   mem_inptr[0]='\0';
   FORMAT_mem_outptr(outptr);
   FORMAT_mem_num;
          mem_matchnum[0] = '\0'; 
   FORMAT_mem_size(size);
          mem_routine  = "mall";
          call_type = caller;
   FORMAT_call_module(file);
          call_line = line;
   MEMDEBUG_MESSAGE(mem_outptr);

   ADD_TO_STACK(outptr);
   INCREMENT_STACK;
   return outptr;
}

void *d_calloc(count,size,line,file,caller)
   size_t count;
   size_t size;
   int  line;
   char *file;
   char *caller;
{
   void *outptr;
   USE_mem_AND_call_VARIABLES;

   INIT_STACK;

   outptr = calloc(count,size);
   
   mem_inptr[0]='\0';
   FORMAT_mem_outptr(outptr);
   FORMAT_mem_num;
          mem_matchnum[0] = '\0'; 
   FORMAT_mem_size(size);
          mem_routine  = "call";
          call_type = caller;
   FORMAT_call_module(file);
          call_line = line;
   MEMDEBUG_MESSAGE(mem_outptr);

   ADD_TO_STACK(outptr);
   INCREMENT_STACK;
   return outptr;
}

void *d_realloc(inptr,size,line,file,caller)
   void   *inptr;
   size_t size;
   int line;
   char *file;
   char *caller;
{
   void *outptr;
   USE_mem_AND_call_VARIABLES;

   INIT_STACK;

   FORMAT_mem_inptr(inptr);
   FORMAT_mem_num;
      GET_mem_matchnum(inptr);
   FORMAT_mem_size(-1);
          mem_routine  = "real";
          call_type = caller;
   FORMAT_call_module(file);
          call_line = line;
   MEMDEBUG_MESSAGE(mem_inptr);

   outptr = realloc(inptr,size);

   FORMAT_mem_size(size);
   FORMAT_mem_outptr(outptr);
   MEMDEBUG_MESSAGE(mem_outptr);

   ADD_TO_STACK(outptr);
   INCREMENT_STACK;
   return outptr;
}

void d_free(inptr,line,file,caller)
   void *inptr;
   int  line;
   char *file;
   char *caller;
{
   USE_mem_AND_call_VARIABLES;

   INIT_STACK;

   mem_outptr[0]='\0';
   FORMAT_mem_inptr(inptr);
   FORMAT_mem_num;
      GET_mem_matchnum(inptr);
   FORMAT_mem_size(-1);
          mem_routine  = "free";
          call_type = caller;
   FORMAT_call_module(file);
          call_line = line;
   MEMDEBUG_MESSAGE(mem_inptr);

   free(inptr);

   INCREMENT_STACK;
   return;
}

void d_cfree(inptr,line,file,caller)
   void *inptr;
   int  line;
   char *file;
   char *caller;
{
   USE_mem_AND_call_VARIABLES;

   INIT_STACK;

   mem_outptr[0]='\0';
   FORMAT_mem_inptr(inptr);
   FORMAT_mem_num;
      GET_mem_matchnum(inptr);
   FORMAT_mem_size(-1);
          mem_routine  = "cfre";
          call_type = caller;
   FORMAT_call_module(file);
          call_line = line;
   MEMDEBUG_MESSAGE(mem_inptr);

   free(inptr);

   INCREMENT_STACK;
   return;
}

void d_MemDump()
{
   int i=0;
   int j=0;

   if (!num) { 
      fprintf(GV_MEMDEBUG_OUT,"MemDebug:   no call registered.\n");
      return;
   }
   fprintf(GV_MEMDEBUG_OUT,"MemDebug:   Unresolved calls:\n");
   while (i<MAX_ADR && ad[i].number>-2) {
      if (ad[i].number>=0)
        fprintf(GV_MEMDEBUG_OUT,"? %8lX | %7d\n",(unsigned long)(ad[i].adress),ad[i].number);
      if (ad[i].number==-1) ++j;
      ++i;
   }
   if (i==j) fprintf(GV_MEMDEBUG_OUT,"  None\n");
   fprintf(GV_MEMDEBUG_OUT,"MemDebug:   %d out of %d stack positions are unused.\n",j,i);
   if (i==j) fprintf(GV_MEMDEBUG_OUT,"MemDebug:   Stack is CLEAN !\n");
}
