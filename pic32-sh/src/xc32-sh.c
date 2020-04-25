/*
 * Copyright (c) 2015, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 * 
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above 
 *         copyright notice, this list of conditions and the following 
 *         disclaimer in the documentation and/or other materials provided 
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written 
 *         permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/************************************************************************/
/*  xc32-sh.c: xc32 utility shell.                                      */
/************************************************************************/

/*----------------------------------------------------------------------*/
/*  I N C L U D E    F I L E S                                          */
/*----------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "xc32-linker.h"

#include <unistd.h>

#define map_to "bin/"
#if defined(MINGW) && !defined(DEBUG)
#include <windows.h>
#define stat _stat
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#if defined(MINGW)
#define PRINT_EXECUTING(ARGV) \
  if (bVersion) { \
    int i; \
    int width=0; \
    width += fprintf(stderr,"*** Executing: "); \
    for (i = 0; ARGV[i]; i++) { \
      width += fprintf(stderr, "%s ", ARGV[i]); \
      if (width>65) width = fprintf(stderr,"\n   "); } \
    fprintf(stderr,"\n"); }
#else
#define PRINT_EXECUTING(ARGV) \
  if (bVersion) { \
    int i; \
    fprintf(stderr, "*** Executing: "); \
    for (i = 0; ARGV[i]; i++) { \
      fprintf(stderr, "%s ", ARGV[i]); } \
    fprintf(stderr, "\n"); }
#endif

#if defined(MINGW)
#define DOESHAVEEXE .exe
#define SLASHTYPE "\\"
#else
#define DOESHAVEEXE 
#define SLASHTYPE "/"
#endif
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  C O N S T A N T S    A N D    L I M I T S                           */
/*----------------------------------------------------------------------*/
#define FALSE       0
#define TRUE        (!FALSE)
#define OMFENVIRON  "XC32_OMF"      /* Environment variable for OMF     */
#define OMFENVIRON2 "PIC32_OMF"     /* Subordinate environment variable */
#define OMFCMDOPT   "-omf="         /* Command-line option for OMF      */
#define OMFELF      "pic32"
#define OMFDEFAULT  OMFELF
#define OMFMAXLEN   4               /* Longest OMF name                 */
#define DEBUG_VAL   "XC32_DEBUG"
#define version2(X) #X
#define version(X) version2(X)
#define SAVEGLD     "--save-gld"
#define NOCPP       "--no-cpp"

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    V A R I A B L E    D E F I N I T I O N S               */
/*----------------------------------------------------------------------*/
static  char    lszTitle[] = "Microchip Language Tool Shell";
static  char    lszVersion[] = version(MCHP_VERSION)
                           " (Build date: " __DATE__ ")";
static int ld_tool;
static char gccCmd[8192];
static int save_gld=0;
static struct libpath {
  char *path;
  struct libpath *next;
} *path;
static int bVersion = FALSE;
static int no_cpp=0;
static unsigned char only_preprocess = 0;
static char *at_file_found = 0;

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    F U N C T I O N    P R O T O T Y P E S                 */
/*----------------------------------------------------------------------*/
static char *   getOMF(int *argc, char *argv[]);
static char *   getEXE(char *pszThis, char *pszOMF);
static int      execute(char *pszEXE, int argc, char *argv[]);
static int      isELF(char *pszPutativeOMF);
static int      isOMF(char *pszPutativeOMF, char *pszOMF);
static int      expandargv(int *argc, char **argv[]);
/*----------------------------------------------------------------------*/

#ifdef DEBUG_MINGW
void loop_forever() {
  while(1);
}
#endif

void
unbackslashify (char *s)
{
  char *save_s, *s2;
  save_s = s;
  while ((s = strchr (s, '\\')) != NULL) {
    *s = '/';
  }

  /* Remove double slashes */
  s = s2 =  save_s;
  while ((s = strchr(s, '/')) != NULL) {
    if (*(s+1) == '/') {
      strcpy(s, (s+1));
    }
    s++;
  }
  return;
}
/************************************************************************/
/*  main() -- pa-sh console entry point.                                */
/************************************************************************/
int 
main(int argc, char *argv[])
{
    int rc;
    char *pszOMF;
    char *pszEXE;
    char *pszSuffix;
    char *pszDebug;

#ifdef DEBUG_MINGW
    fprintf(stderr,"my pid is: %d\n", getpid());
    loop_forever();
#endif

    if (expandargv(&argc, &argv)) {
      fprintf(stderr,"Fatal shell error\n"); 
      return 1;
    }
    /* override debug setting */
    pszDebug = getenv(DEBUG_VAL);
    if ((pszDebug != NULL)  && strchr(pszDebug,'P')) bVersion = TRUE;

    pszSuffix = strrchr(argv[0], '-');
    if (pszSuffix != NULL)
    {
      if (strncmp(pszSuffix,"-ld",3) == 0) {
        ld_tool = 1;
      }
    }
    pszOMF = getOMF(&argc, argv);
#if defined(MINGW)
    {
      static char szCopyString[MAX_PATH];
      if (GetModuleFileName(NULL, szCopyString, sizeof(szCopyString)) > 0 ) {
        pszEXE = getEXE(szCopyString, pszOMF);
      } else {
        pszEXE = getEXE(argv[0], pszOMF);
      }
    }
#else
    pszEXE = getEXE(argv[0], pszOMF);
#endif
    rc = execute(pszEXE, argc-1, argv+1);

    return((rc >> (unsigned)8) & 255);
}

static char *locatefile(char *n, char *separator) {
  struct libpath *p;
  static char file[1000];
  struct stat fs;

  if (stat(n,&fs) == 0) {
    if (bVersion) fprintf(stderr,"Located: ./%s\n", n);
    sprintf(file,"\"%s\"", n);
    return file;
  }
  for (p = path; p; p=p->next) {
    sprintf(file,"%s%s%s", p->path,separator,n);
    if (stat(file, &fs) == 0) {
      // found it
      if (bVersion) fprintf(stderr,"Located: %s%s%s\n", p->path,separator,n);
      sprintf(file,"\"%s%s%s\"", p->path,separator,n);
      return file;
    }
  }
  if (bVersion) fprintf(stderr,"Could not find: %s\n", n);
  sprintf(file,"\"%s\"", n);
  return file;
}

static void write_args_to_file(char *file, char **argv) {
  FILE *f;
  int c;

  f = fopen(file, "w");
  if (f == 0) {
    fprintf(stderr,"Cannot open file %s for writing\n", f);
    exit(1);
  }

  for (c = 1; argv[c] != 0; c++) {
    if (argv[c][0] == '"') {
      int j;

      fprintf(f," ");
      for (j = 0; argv[c][j]; j++) fprintf(f,"%c",argv[c][j]);
    } else fprintf(f, " %s", argv[c]);
  }
  fprintf(f,"\n");
  fclose(f);
}

#define JOIN2(a,b) a##b
#define JOIN(a,b) JOIN2(a,b)
#define new_path(to,value) \
  p = (struct libpath *) malloc(sizeof(struct libpath)); \
  p->next = 0; \
  if (JOIN(s_,to) == 0) JOIN(s_,to) = p; \
  else JOIN(l_,to)->next = p; \
  JOIN(l_,to) = p; \
  p->path = strdup(value);

#ifdef MINGW
int EXECUTE(char *cmd, char **argv) {
  int rc;

  if (at_file_found) {
    char *real_args[3];

    write_args_to_file(at_file_found, argv);
    real_args[0] = malloc(strlen(cmd)+3);
    if (cmd[0] != '"') {
      sprintf(real_args[0],"\"%s\"", cmd);
    } else { 
      sprintf(real_args[0],"%s", cmd);
    }
    real_args[1] = malloc(strlen(at_file_found)+2);
    real_args[2] = 0;

    sprintf(real_args[1], "@%s", at_file_found);
    rc = _spawnv(_P_WAIT, cmd, real_args);
    DeleteFile(at_file_found);

  } else {
    rc = _spawnv(_P_WAIT, cmd, argv);
  }

  return rc;
}
#else
#define EXECUTE(CMD,ARGS) SYSTEM_WITH_ARGV(ARGS)


int SYSTEM_WITH_ARGV(char **ARGV) {
  char *print_argv; 
  int counter = 0; 
  unsigned long size = 0; 
  int rc;
  if (at_file_found) {
    write_args_to_file(at_file_found, ARGV);
    print_argv = (char *)malloc(strlen((ARGV)[0])+strlen(at_file_found)+3);
    sprintf(print_argv,"%s @%s", (ARGV)[0], at_file_found);
  } else {
  for (counter = 0; (ARGV)[counter]; counter++)
    size += strlen((ARGV)[counter])+1; 
  print_argv = (char *)calloc((size_t)size+1,1); 
  if (print_argv == 0) {
    fprintf(stderr,"Could not allocate %ld bytes\n", (long int)size);
    exit(1);
  }
  
  counter = 0;
  while ((ARGV)[counter]) { 
      print_argv = strcat(print_argv, (ARGV)[counter]); 
      print_argv = strcat(print_argv, " "); 
      counter++; 
	  } 
  }
  rc = system(print_argv); 
  free(print_argv); 
  if (at_file_found) {
    char *rm;
    rm=malloc(8192);
    sprintf(rm,"rm -f \"%s\"", at_file_found);
    if (save_gld == 0) system(rm);
    free(rm);
  }
  return rc;
}
#endif

#if 0
#ifdef MINGW
#define EXECUTE(CMD,ARGS) \
          rc = _spawnv(_P_WAIT, CMD, ARGS)
#else
#define EXECUTE(CMD,ARGS) \
          rc = SYSTEM_WITH_ARGV(ARGS)
#endif
#endif
  
/************************************************************************/
/*  execute() -- Execute the OMF-specific tool.                         */
/************************************************************************/
#if defined(MINGW) && !defined(DEBUG)
#include <process.h>
#endif

static int
execute(char *pszEXE, int argc, char *argv[]) {
    /* use the spawn or system command to invoke the omf-specific tool */

    int i;
    int rc;
	
    struct libpath *s_gld_names = 0;
    struct libpath *l_gld_names = 0;
    struct libpath *s_to_names = 0;
    struct libpath *l_to_names = 0;
    struct libpath *p = 0;
    char **new_argv;
    char **gcc_argv;
    char *new_EXE;
    int shi,gcci;
    char *option;
    struct libpath *last = 0;   
 
    /* Every option may be an -mreserve since we can have more than one area */
    new_argv = malloc(sizeof(char*) * ((argc*2)+4));
    gcc_argv = malloc(sizeof(char*) * (argc+9));
    
    if ((new_argv == NULL) || (gcc_argv == NULL))
      {
        fprintf(stderr, "Out of memory in xc32-sh\n");
        exit(1);
      }
    
    shi = 0;
    gcci = 0;

    for (i = 0; i < argc; i++) {
      if (ld_tool) {
        if (strncmp(argv[i],RESERVE,RESERVE_LEN) == 0) {
          collect_reserve_section_opts(argv[i]);
        }
      }
    }

    /*  Write the sections and add the new -T option to the new command line.
     *  This has to happen after the options are sorted by end address.
     *  This has to happen before the original arguments are copied so that the
     *  new -T options come before a user-specified -T option.
     */
    write_reserve_sections(rs_opt_list, new_argv, &shi);
    
    for (i = 0; i < argc; i++) {
      char new_option[FILENAME_MAX];
      option = argv[i];
      new_option[0]=0;

      if (option[0] == '@') {
        /* Remove backslashes from file paths */
        unbackslashify(option);
      }

      if (strncmp(argv[i],"-E",2) == 0) {
        only_preprocess = 1;
      }

      if (ld_tool) {
        
        if ((strncmp(argv[i],"-D",2) == 0) && (strlen(argv[i])>2)) {
          // -D option passed to an ld_tool, put it somewhere else
          gcc_argv[gcci+1] = malloc(strlen(argv[i]) + 3);
          if (argv[i][0] != '"')
            sprintf(gcc_argv[gcci+1], "\"%s\"", argv[i]);
          else
            sprintf(gcc_argv[gcci+1], "%s", argv[i]);
          gcci++;
          continue;
        } else if ((strncmp(argv[i],"-L",2) == 0) ||
                   (strncmp(argv[i],"--library-path",15) == 0)) {
          p = (struct libpath *) malloc(sizeof(struct libpath));
          if (argv[i][1] == 'L') p->path = &argv[i][2];
          else p->path = &argv[i][15];
          if (*p->path == 0) {
            p->path = argv[i+1];
//            i++;
          }
          p->path = strdup(p->path);
          p->next = 0;
          if (last == 0) {
            path = p;
          } else {
            last->next = p;
          }
          last = p;
        } else if (strncmp(argv[i],"-T",2) == 0) {
          // might be a .gld name
          if (argv[i][2] == 0) {
            new_path(gld_names,argv[i+1]);
            if (ld_tool == 2) {
              new_path(to_names,tmpgldname(s_gld_names->path));
              sprintf(new_option,"-T\"%s\"", p->path);
              option=new_option;
            } else {
              new_argv[shi+1] = malloc(strlen(option)+3);
              if (option[0] != '"')
                sprintf(new_argv[++shi], "\"%s\"", option);
              else
                sprintf(new_argv[++shi], "%s", option);
              option = argv[i+1];
            }
            i++;
          } else if (argv[i][2] == '=') {
            new_path(gld_names,strdup(&argv[i][3]));
            if (ld_tool == 2) {
              new_path(to_names,strdup(tmpgldname(s_gld_names->path)));
#if 0
              /* doesn't look  like MINGW has a difference in this operation.. */
              if (p[0] != '"')
                sprintf(new_option,"-T\"%s\"", p);
              else
                sprintf(new_option,"-T%s", p);
#endif
              if (p->path[0] != '"')
                sprintf(new_option,"-T \"%s\"", p->path);
              else
                sprintf(new_option,"-T %s", p->path);
              option = new_option;
            }
          } else if ((strcmp(&argv[i][2],"bss") != 0) &&
                     (strcmp(&argv[i][2],"text") != 0) &&
                     (strcmp(&argv[i][2],"data") != 0)) {
            // not -Tdata, -Tbss, -Ttext so *must* be a gld file
            new_path(gld_names,strdup(&argv[i][2]));
            if (ld_tool == 2) {
              new_path(to_names,strdup(tmpgldname(s_gld_names->path)));
              if (p->path[0] != '"')
                sprintf(new_option,"-T\"%s\"", p->path);
              else
                sprintf(new_option,"-T%s", p->path);
              option = new_option;
            }
          }
        } else if (strncmp(argv[i],"--script",8) == 0) {
          if (argv[i][8] == 0) {
            new_path(gld_names,strdup(argv[i+1]));
            i++;
          } else {
            new_path(gld_names,strdup(&argv[i][9]));
          }
          if (ld_tool == 2) {
            new_path(to_names,strdup(tmpgldname(s_gld_names->path)));
            if (p->path[0] != '"')
              sprintf(new_option,"-T\"%s\"", p->path);
            else
              sprintf(new_option,"-T%s", p->path);
            option=new_option;
          }
        }
      } else {
        /* We must have code that will recognize this option belongs to 
        * linker and put the correct -Wl in front of the command */
        if (strncmp(argv[i],RESERVE,RESERVE_LEN) == 0) {

        /* Copying the new option with the quotes on either side */
        strcpy(new_option, "\"-mdebugger\"");
        strcpy(new_option, " \"");
        strcat(new_option, argv[i]);
        strcat(new_option, "\"");
        option = new_option;
        }
      }

      /* This code copies the argument from the original argv list to the
       * new argv list */
      if (*argv[i]) {
        size_t bufsize = strlen(option) + 3;
        new_argv[shi+1] = malloc(bufsize); 
        
        if (new_argv[shi+1] == NULL)
        {
          fprintf(stderr, "Out of memory in xc32-sh\n");
          exit(1);
        }

        /* No need to enclose in quotes when writing to response file.
        */
        if (at_file_found) {
          snprintf(new_argv[shi+1], bufsize, "%s", option);
        } else {
          if (new_option[0]) 
             snprintf(new_argv[shi+1], bufsize, "%s", option);
          else snprintf(new_argv[shi+1], bufsize, "\"%s\"", option);
        }
        shi++;
      }
    }
    
    if (ld_tool == 2) {
      struct libpath *f,*t;

      gcc_argv[++gcci] = strdup("-C");
      gcc_argv[++gcci] = strdup("-P");
      gcc_argv[++gcci] = strdup("-E");
      gcc_argv[++gcci] = strdup("-o");
      gcc_argv[++gcci] = 0;
      gcc_argv[gcci+1] = 0;
      gcc_argv[gcci+2] = 0;

      new_EXE = malloc(strlen(gccCmd)+3);
      gcc_argv[0] = new_EXE;
      sprintf(new_EXE,"\"%s\"", gccCmd);
      // this is ld and we have found pic30-gcc
      for (f = s_gld_names, t = s_to_names; f; f=f->next, t=t->next) {
        gcc_argv[gcci] = malloc(strlen(t->path)+3);
        sprintf(gcc_argv[gcci],"\"%s\"",t->path);
        gcc_argv[gcci+1] = locatefile(f->path,SLASHTYPE);

        PRINT_EXECUTING(gcc_argv);

        rc = EXECUTE(gccCmd, gcc_argv);

        free(gcc_argv[gcci]);
      }
      for (f = s_gld_names; f; f=p) {
        p = f->next;
        if (f->path) free(f->path);
        free(f);
      }
	}

    new_EXE = malloc(strlen(pszEXE) + 3);
    new_argv[0] = new_EXE;
    new_argv[shi+1] = 0;
    sprintf(new_EXE,"\"%s\"", pszEXE);
    errno = 0;

    PRINT_EXECUTING(new_argv);
    rc = EXECUTE(pszEXE, new_argv);

    check_and_delete_reserved_sects();

    // this is ld and we have found xc32-gcc
    if (ld_tool == 2) {
      struct libpath *f;

      for (f = s_to_names; f; f=p) {
        p = f->next;
        if (f->path) {

#ifdef MINGW
#ifndef DEBUG
          if (save_gld == 0) DeleteFile(f->path);
#endif
#else
          char *rm;
          rm=malloc(8192);
          sprintf(rm,"rm -f \"%s\"", f->path);
          if (save_gld == 0) system(rm);
#endif
          free(f->path);
        }
        free(f);
      }
    }

    for (i = 1; i < shi; i++) 
      free(new_argv[i]);
    if (new_argv != NULL)
      free(new_argv);
    if (gcc_argv != NULL)
      free(gcc_argv);

    return (rc != 0) * INT_MAX;
}
/************************************************************************/
/*  getEXE() -- Locate the OMF-specific tool executable.                */
/************************************************************************/
static char * 
getEXE(char *pszThis, char *pszOMF)
{
    int original_name = FALSE;
    static char szCommand[FILENAME_MAX+3+sizeof(map_to)];
    int nPathLen = 0;
    int cbPrefix;
    char *pszSuffix;
    int cbToolName;
    struct stat fs;
    szCommand[0] = 0;
    /*
    ** Tool names should have the format xxx
    */
    pszSuffix = strrchr(pszThis, '-');
    if (strncmp(pszSuffix,"-ld",3) == 0) {
      ld_tool = 1;
    }
    if (pszSuffix == NULL)
    {
        fprintf(stderr, "%s: This language tool name is invalid\n", pszThis);
        exit(1);
    }
    cbPrefix = pszSuffix - pszThis;
    cbToolName = cbPrefix+1 + strlen(pszOMF) + strlen(pszSuffix) + 1 + 
                 sizeof(map_to);

#if defined(CHECK_PATH)
#if defined(MINGW)
    /*
    ** In some Windows command shells, no path information
    ** is provided as part of argv[0].
    */
    if (strchr(pszThis, '\\') == NULL)
#else
    if (strchr(pszThis, '/') == NULL)
#endif
    {
        /*
        ** No DOS path information for the program:
        ** search for the executable using the
        ** PATH environment variable.
        */
        char *pSeparator = NULL;
        const char *pszPATH = NULL;
        char *dosPATH = NULL;
        int cbSuffix;
        char szSuffix[FILENAME_MAX+1];
        int dosPATH_alloced = 0;

        /*
        ** Append ".exe" to the executable name, if necessary
        */
        cbSuffix = strlen(pszSuffix);
#if defined(MINGW) 
        if ((cbSuffix+5) < sizeof(szSuffix))
        {
            strcpy(szSuffix, pszSuffix);
            if ((tolower(szSuffix[cbSuffix-1]) != 'e') ||
                (tolower(szSuffix[cbSuffix-2]) != 'x') ||
                (tolower(szSuffix[cbSuffix-3]) != 'e') ||
                (       (szSuffix[cbSuffix-4]) != '.') )
            {
                strcat(szSuffix, ".exe");
                pszSuffix = szSuffix;
                cbToolName += 4;
            }
        }
#endif

        /*
        ** Get the current PATH
        */
        pszPATH = getenv("PATH");
#if defined(MINGW) 
        if (pszPATH) {
          /*
           *  stupid gnu will print (null) to the string if pszPATH==0
           */
          dosPATH = calloc(1, strlen(pszPATH) + 3);
          sprintf(dosPATH,"%s;.",pszPATH);
          dosPATH_alloced = 1;
        } else {
          dosPATH=".";
        }
        pszPATH = dosPATH;
#endif
        do
        {
#if defined(MINGW) 
            pSeparator = strchr(pszPATH, ';');
#else
            pSeparator = strchr(pszPATH, ':');
#endif
            nPathLen = pSeparator ? pSeparator-pszPATH : strlen(pszPATH);
            if ((nPathLen + cbToolName + 1) < sizeof(szCommand))
            {
                strncpy(szCommand, pszPATH, nPathLen);
                szCommand[nPathLen] = '\0';
                if ((nPathLen > 0) && (szCommand[nPathLen-1] != '\\'))
                {
                    strcat(szCommand, SLASHTYPE);
                    nPathLen++;
                }
                strcat(szCommand,map_to);
#if 0
                /* omit product name */
                strncat(szCommand, pszThis, cbPrefix+1);
#endif
                /* At the moment of writing this comment, this should 
                 * should always be true */
                if (original_name == TRUE) {
                  strcat(szCommand, pszSuffix+1);
                }
                else {
                  strcat(szCommand, pszOMF);
                  strcat(szCommand, pszSuffix);
                }
                if (stat(szCommand, &fs) == 0)
                {
                    if (ld_tool && !no_cpp) {
                      // found the tool, do we also have -gcc?
                      strncpy(gccCmd, pszPATH, nPathLen);
                      gccCmd[nPathLen] = '\0';
                      if ((nPathLen > 0) && (gccCmd[nPathLen-1] != '\\'))
                      {
                          strcat(gccCmd, SLASHTYPE);
                          nPathLen++;
                      }
                      strcat(gccCmd,map_to);
#if 0
                      /* omit product name */
                      strncat(gccCmd, pszThis, cbPrefix+1);
#endif
                      strcat(gccCmd, pszOMF);
                      strcat(gccCmd, "-cpp" TOSTRING(DOESHAVEEXE));
                      if (stat(gccCmd, &fs) == 0) ld_tool++;
                    }
                    break;
                }

            }
            pszPATH = pSeparator+1;
            nPathLen = 0;
        } while (pSeparator);
        if (dosPATH_alloced && dosPATH) free(dosPATH);
    }
    else
#endif
    {
        /*
        ** Path information available for the program
        */
        nPathLen = cbToolName;
        if (nPathLen < sizeof(szCommand))
        {   char *c;

            for (c = pszSuffix; c >= pszThis; c--) 
              if ((*c == '/') || (*c == '\\')) {
                c++;
                break;
              }
            if (c > pszThis) {
              strncpy(szCommand, pszThis, c-pszThis);
              szCommand[c-pszThis] = 0;
            } else {
              c = pszThis;
            }
            strcat(szCommand, map_to);
#if 0
            /* omit product name */
            strncat(szCommand, c, cbPrefix-(c-pszThis)+1);
#endif
            if (original_name == TRUE) {
              strcat(szCommand, pszSuffix+1);
            }
            else {
              strcat(szCommand, pszOMF);
              strcat(szCommand, pszSuffix);
            }
            if (ld_tool && !no_cpp) {
              if (c > pszThis) {
                strncpy(gccCmd, pszThis, c-pszThis);
                gccCmd[c-pszThis] = 0;
              } else {
                c = pszThis;
              }
              strcat(gccCmd, map_to);
#if 0
              /* omit product name */
              strncat(gccCommand, c, cbPrefix-(c-pszThis)+1);
#endif
              strcat(gccCmd, pszOMF);
              strcat(gccCmd, "-cpp" TOSTRING(DOESHAVEEXE));
              if (stat(gccCmd, &fs) == 0) ld_tool++;
            }

        }
        else
        {
            nPathLen = 0;
        }
    }
    if (nPathLen == 0)
    {
        fprintf(stderr, 
                "%s: Cannot locate the %s-specific executable\n", 
                pszThis, pszOMF);
        exit(1);
    }
    return(szCommand);
}
/************************************************************************/
/*  getOMF() -- Get the OMF to use for the tool.                        */
/************************************************************************/
static char * 
getOMF(int *argc, char *argv[])
{
    int i;
    char *pszOMF = NULL;
    int bHelp = FALSE;

    i = 0;
    while (i < argc[0])
    {
        int bFound = FALSE;

        if (strcmp(argv[i], "--help") == 0) 
        {
            bVersion = TRUE;
            bHelp = TRUE;
        } 
        else if (strcmp(argv[i], "-v") == 0)
        {
            bVersion = TRUE;
        }
        else if (strncmp(argv[i], OMFCMDOPT, strlen(OMFCMDOPT)) == 0)
        {
            /* ** OMF option specified */
            char *pszPutativeOMF = argv[i]+strlen(OMFCMDOPT);
            char setenvstr[80];

            if (isELF(pszPutativeOMF))
            {
                /* ** ELF */
                pszOMF = OMFELF;
                bFound = TRUE;
            }
            sprintf(setenvstr,OMFENVIRON "=%s", pszOMF);
            putenv(setenvstr);
        } else if (strncmp(argv[i], SAVEGLD, strlen(SAVEGLD)) == 0) {
          if (argv[i][strlen(SAVEGLD)] == '=') {
            save_gld = 1;
            set_gld_save_folder(strdup(argv[i]+strlen(SAVEGLD)+1));
          } else {
            save_gld = 1;
          }
          bFound = TRUE;
        } else if (strncmp(argv[i], NOCPP, strlen(NOCPP)) == 0) {
          no_cpp=1;
          bFound = TRUE;
        }

        if (bFound)
        {
            int j;

            for (j = i+1; j < argc[0]; ++j)
            {
                argv[j-1] = argv[j];
            }
            argv[j-1] = NULL;
            --argc[0];
        }
        else
        {
            i++;
        }
    }
    if (pszOMF == NULL)
    {
        /*
        ** If no OMF command-line option specified,
        ** see if an OMF environment variable is specified.
        */
        char *pszPIC30_OMF = getenv(OMFENVIRON);
        if (pszPIC30_OMF == NULL) {
          pszPIC30_OMF = getenv(OMFENVIRON2);
        }
        if (pszPIC30_OMF != NULL)
        {
            if (isELF(pszPIC30_OMF))
            {
                pszOMF = OMFELF;
            }
        }
    }
    if (pszOMF == NULL)
    {
        /*
        ** If no OMF command-line option specified,
        ** and no OMF environment variable is specified,
        ** use the built-in default.
        */
        pszOMF = OMFDEFAULT;
    }
    if (bVersion)
    {
      printf("%s Version %s.\n", lszTitle, lszVersion);
      printf("Copyright (c) 2012 Microchip Technology Inc. "
               "All rights reserved\n");
    }
    if (bHelp) {
       printf("\n  -omf=elf\t\t"
              "Select elf object module format");
       if (ld_tool) {
         printf("\n  --no-cpp\t\t"
                "Do not pre-process linker scripts\n"
                "           \t\tspecified with -T or --script");
         printf("\n  --save-gld=<root>\t"
                "Save pre-processed linker scripts using <root>");
         printf("\n  -D<macro>[=<value>]\t"
                "This option is passed to the C pre-processor");
         printf("\n  -T<gld-file>"
                "\n  -T <gld-file>"
                "\n  --script <gld-file>"
                "\n  --script=<gld-file>\t"
                 "Identify linker script(s) to be pre-processed\n");
       }
       printf("\n");
    
    }
    return(pszOMF);
}
/************************************************************************/
/*  isELF() -- Determine if a string names the ELF OMF.                 */
/************************************************************************/
static int
isELF(char *pszPutativeOMF)
{
    return(isOMF(pszPutativeOMF, OMFELF));
}
/************************************************************************/
/*  isOMF() -- Determine if a string names an OMF.                      */
/************************************************************************/
static int
isOMF(char *pszPutativeOMF, char *pszOMF)
{
    int i = 0;

    while (
           (pszOMF[i] != 0) && 
           (pszPutativeOMF[i] != 0) &&
           (tolower(pszPutativeOMF[i]) == tolower(pszOMF[i]))
          )
    {
        ++i;
    }
    return(pszPutativeOMF[i] == pszOMF[i]);
}
/*
 * expandargv
 *   copy the argc into a new place if there is an '@'argument
 */

int expandargv(int *argc, char **argv[]) {
  int arg;
  char *c;

  for (arg = 0; arg < *argc; arg++) {
    if ((*argv)[arg][0] == '@') break;
  }

  /* '@' found, copy contents of file into argc stream */
  if (arg != *argc) {
    struct expanded_args {
      char *argv;
      struct expanded_args *next;
    } *head, *tail;

    FILE *argfile = 0;
    struct stat finfo;
    char *buffer;
    int new_args;
    int a;
    char **new_argv;
    int new_argc;
    int in_quote = 0;
    int back_quote = 0;
    int actual_size;

    at_file_found = tmpgldname(0);

    if (stat(&(*argv)[arg][1], &finfo)  == -1) {
      fprintf(stderr,"Cannot find response file: %s\n", &(*argv)[arg][1]);
      return 1;
    }
    
    argfile = fopen(&(*argv)[arg][1], "r");
    if (argfile == NULL) {
      fprintf(stderr,"Cannot open response file: %s\n", &(*argv)[arg][1]);
      return 1;
    }

    buffer = (char *) malloc (finfo.st_size * sizeof (char) + 1);
    if (buffer == 0) {
      fprintf(stderr,"Cannot allocate %d bytes\n", finfo.st_size);
      return 1;
    }

    actual_size = fread(buffer, sizeof (char), finfo.st_size, argfile);
    if ((actual_size == 0) || (actual_size > finfo.st_size)) {
      fprintf(stderr,"Error reading file\n");
      return 1;
    }

    buffer[actual_size] = '\0';
    head = 0;
    tail = 0;
    new_args=0;

    /* buffer read - create new arguments from the file; */
    for (a = 0; buffer[a] == ' '; a++);

#define IS_VALID_SPACE(X) \
    ((!in_quote) && \
      (((X) == ' ') || ((X) == '\n') || ((X) == '\t')))

    c = &buffer[a];
    for (;a <= actual_size; a++) {
      if (back_quote == 1) {
        back_quote = 0;
        continue;
      }
      if (buffer[a] == '"') in_quote = !in_quote;
      if (buffer[a] == '\\') back_quote = 1;
      else if ((buffer[a] == 0) || (IS_VALID_SPACE(buffer[a]))) {
        struct expanded_args *new_arg;

        buffer[a] = 0;
        new_arg = malloc(sizeof(struct expanded_args));
        if (new_arg == 0) {
          fprintf(stderr,"Cannot allocate buffer space\n");
          return 1;
	}

        new_args++;
        new_arg->argv = strdup(c);
        new_arg->next = 0;
        if (head == 0) {
          head = new_arg;
          tail = new_arg;
        } else {
          tail->next = new_arg;
          tail = new_arg;
        }
        for (; buffer[a+1] == ' '; a++);
        c = &buffer[a+1];
      }
    }
    
    new_argv = calloc(new_args + *argc - 1, sizeof(char *));
    if (new_argv == 0) {
      fprintf(stderr,"Cannot allocate buffer space\n");
      return 1;
    }
    
    /* copy old and new args into place */
    for (new_argc =0, a = 0; a < *argc; a++) {
      if (a != arg) {
        new_argv[new_argc++] = (*argv)[a];
      } else {
        struct expanded_args *next;

        for (tail = head; tail; tail = next) {
          next = tail->next;
          new_argv[new_argc++] = tail->argv;
          free(tail);
        }
      }
    }
    if (new_argc > new_args + *argc - 1) {
      fprintf(stderr,"Thought about generating %d args, but made %d\n",
              new_args + *argc - 1, new_argc);
    }
   
    *argc = new_argc;
    *argv = new_argv;

    /* almost finished re-scan with the new args looking for further
       response files */
    return expandargv(argc, argv);
  }
  return 0;
}

/*END********************************************************************/
