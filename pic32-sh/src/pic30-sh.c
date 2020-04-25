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
/*  pic30-sh.c: dsPIC30 utility shell.                                  */
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
#include "linker.h"

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

#define MALLOC_MEMCPY_INC(VAL1,VAL2) \
  { \
    mpa_argv[VAL1] = calloc(strlen(new_argv[VAL2]) + 1,1); \
    memcpy(mpa_argv[VAL1],new_argv[VAL2],strlen(new_argv[VAL2])+1); \
    VAL1++; \
  }


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
#define OMFENVIRON  "XC16_OMF"      /* Environment variable for OMF     */
#define OMFENVIRON2 "PIC30_OMF"     /* Subordinate environment variable */
#define OMFCMDOPT   "-omf="         /* Command-line option for OMF      */
#define OMFCOFF     "coff"
#define OMFELF      "elf"
#define OMFDEFAULT  OMFELF
#define OMFMAXLEN   4               /* Longest OMF name                 */
#define version2(X) #X
#define version(X) version2(X)
#define SAVEGLD     "--save-gld"
#define NOCPP       "--no-cpp"
#define AUXRESET    "--auxreset"
#define AUXTEXT     "--auxtext"
#define MPA         "-mpa"

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    V A R I A B L E    D E F I N I T I O N S               */
/*----------------------------------------------------------------------*/
static  char    lszTitle[] = "Microchip Language Tool Shell";
static  char    lszVersion[] = version(MCHP_VERSION)
                           " (Build date: " __DATE__ ")";
static int gcc_tool;
static int ld_tool;
static int as_tool;
static char gccCmd[8192];
static int save_gld=0;
static struct libpath {
  char *path;
  struct libpath *next;
} *path;
static int bVersion = FALSE;
static int no_cpp=0;
static int auxreset=0;
static int auxtext=0;
static char *at_file_found = 0;
static int saving_temp_files = FALSE;

static unsigned char is_mpa = 0;
static unsigned char only_preprocess = 0;
static unsigned char is_cc1 = 0;
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    F U N C T I O N    P R O T O T Y P E S                 */
/*----------------------------------------------------------------------*/
static char *   getOMF(int *argc, char *argv[]);
static char *   getEXE(char *pszThis, char *pszOMF);
static int      execute(char *pszEXE, int argc, char *argv[]);
static int      isCOFF(char *pszPutativeOMF);
static int      isELF(char *pszPutativeOMF);
static int      isOMF(char *pszPutativeOMF, char *pszOMF);
static int      expandargv(int *argc, char **argv[]);
/*----------------------------------------------------------------------*/

#ifdef DEBUG_MINGW
void loop_forever() {
  while(1);
}
#endif

/************************************************************************/
/*  main() -- pa-sh console entry point.                                */
/************************************************************************/

int main(int argc, char *argv[])
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
    pszDebug = getenv("PIC30_DEBUG");
    if (pszDebug && strchr(pszDebug,'P')) bVersion = TRUE;
    if (pszDebug && strchr(pszDebug,'S')) saving_temp_files = TRUE;

    pszSuffix = strrchr(argv[0], '-');
    if (strncmp(pszSuffix,"-ld",3) == 0) {
      ld_tool = 1;
    } else if (strncmp(pszSuffix,"-as",3) == 0) {
      as_tool = 1;
    } else if (strncmp(pszSuffix,"-gcc",3) == 0) {
      gcc_tool = 1;
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

    return((rc >> 8) & 255);
}

static char *locatefile(char *n, char *separator) {
  struct libpath *p;
  static char file[FILENAME_MAX];
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
      sprintf(file,"\"%s/%s\"", p->path,n);
      return file;
    }
  }
  if (bVersion) fprintf(stderr,"Could not find: %s\n", n);
  sprintf(file,"\"%s\"", n);
  return file;
}

enum pi_kind {
  pi_error,
  pi_family,
  pi_name
};

#define processor_family(X) processor_info((X),pi_family)
#define processor_name(X) processor_info((X),pi_name)

static char *processor_info(char *processor, enum pi_kind kind) {
  static char *converted_processor = 0;
  static char name[64] = { 0 };
  static char family[64] = { 0 };
  char *c;
  
  if (converted_processor != processor) {
    for (c = processor; *c; c++) *c = toupper(*c);
    converted_processor = processor;
    family[0]=0;
  }
  if ((family[0] == 0) && (processor)) {
    if (strncmp(processor,"30F", 3) == 0) {
      sprintf(family,"-D__dsPIC30F__=1");
    } else if (strncmp(processor,"33F", 3) == 0) {
      sprintf(family,"-D__dsPIC33F__=1");
    } else if (strncmp(processor,"24F", 3) == 0) {
      if (processor[3] ==  'J')
        sprintf(family,"-D__PIC24F__=1");
      else
        sprintf(family,"-D__PIC24FK__=1");
    } else if (strncmp(processor,"24H", 3) == 0) {
      sprintf(family,"-D__PIC24H__=1");
    }
  }
  if (kind == pi_family) {
    if (family[0]) return family;
    else return "-DUNKNOWN_FAMILY=1";
  } else if (kind == pi_name) {
    if (family[0] == 0) return "-DUNKNOWN_PROCESSOR=1";
    if (family[4] == 'd') {
      sprintf(name, "-D__dsPIC%s__=1", processor);
    } else {
      sprintf(name, "-D__PIC%s__=1", processor);
    }
    return name;
  } else return NULL;
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
    }
    else 
      sprintf(real_args[0],"%s", cmd);
    real_args[1] = malloc(strlen(at_file_found)+2);
    real_args[2] = 0;
    sprintf(real_args[1], "@%s", at_file_found);

    char *real_command = malloc(strlen(real_args[0]) +
                                strlen(real_args[1]) + 2);
    sprintf(real_command,"%s %s", real_args[0], real_args[1]);
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset(&si,0,sizeof(si));
    si.cb= sizeof(si);
    if (CreateProcess(NULL,real_command,NULL,NULL,FALSE,NULL,NULL,NULL,
                      &si, &pi) == 0) {
      fprintf(stderr,"Could not execute\n");
    } else {
      WaitForSingleObject( pi.hProcess, INFINITE );
    }
    DeleteFile(at_file_found);
  } else
    rc = _spawnv(_P_WAIT, cmd, argv);
 
  return rc;
}
#else
#define EXECUTE(CMD,ARGS) system_with_argv(ARGS)

int system_with_argv(char **ARGV) {
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
    print_argv = (char *)calloc(size+1,1); 
    if (print_argv == 0) {
      fprintf(stderr,"Could not allocate %ld bytes\n", size);
      exit(1);
    }
    counter = 0;
    while ((ARGV)[counter]) { 
        print_argv = strcat(print_argv, (ARGV)[counter]); 
        print_argv = strcat(print_argv, " "); 
        counter++; } 
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

  
/************************************************************************/
/*  execute() -- Execute the OMF-specific tool.                         */
/************************************************************************/

#if defined(MINGW) && !defined(DEBUG)
#include <process.h>
#endif

static int execute(char *pszEXE, int argc, char *argv[]) {
    /* use the spawn or system command to invoke the omf-specific tool */

    int i;
    int rc;
    struct libpath *s_gld_names = 0;
    struct libpath *l_gld_names = 0;
    struct libpath *s_to_names = 0;
    struct libpath *l_to_names = 0;
    struct libpath *p = 0;
    char *partition = 0;
    char *processor = 0;
    char **new_argv;
    char **gcc_argv;
    char **mpa_argv;
    char *new_EXE;
    char *mpaEXE = 0;
    int shi,gcci;
    char *option;
    struct libpath *last = 0;
    char *mpa_temp_file = 0;
    
#if 0
    size_t cmdlen;
    char * pszCmd, * ccCmd;
    
    /* Left from linux. Yet to be determined if this is needed for 
     * windows/linux merge */
    int min_size = save_gld ? FILENAME_MAX : 13;
    cmdlen = 0;
    for (i = 0; i < argc; ++i)
    {   
      int l = strlen(argv[i]);
      cmdlen += (l < min_size ? (min_size) : (l)) +3;
    }
    pszCmd = malloc(strlen(pszEXE)+3 + cmdlen + 1);
    ccCmd = malloc(strlen(gccCmd)+3 + cmdlen + 1);
    if ((pszCmd == NULL) || (ccCmd == NULL))
    {
        fprintf(stderr, "Not enough memory to execute language tool\n");
        exit(1);
    }
#endif

    new_argv = malloc(sizeof(char*) * (argc+3));
    gcc_argv = malloc(sizeof(char*) * (argc+10));
    shi = 0;
    gcci = 0;

    /* This routine copies args from the original arglist and 
       alternatively places them in one of the new ones.  We allocate
       at least argc slots for each new one. */
    for (i = 0; i < argc; i++) {
      char new_option[FILENAME_MAX];
      option = argv[i];
      new_option[0]=0;

      if (strncmp(argv[i],"-E",2) == 0) {
        only_preprocess = 1;
      } else if (strncmp(argv[i],"--partition",7) == 0) {
        partition=argv[i+1];
        i++;
        // don't propagate this to the tool
        continue;
      } 

      if (ld_tool) {
        if (strncmp(argv[i],"-D",2) == 0) {
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
            i++;
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
              /* doen't look  like MINGW has a difference in this operation.. */
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
        } else if (strncmp(argv[i],"--processor",11) == 0) {
          if (argv[i][11] == 0) {
            processor=argv[i+1]; 
            i++;
          } else processor=&argv[i][12];
        } else if (strncmp(argv[i],"-p",2) == 0) {
          if (argv[i][2] == 0) {
            processor=argv[i+1];
            i++;
          } else processor=&argv[i][2];
        } 
        /* Here we mark a flag for later if reserved sections are signified */
        else if (strncmp(argv[i],RESERVE,RESERVE_LEN) == 0) {
          char * reserve_opt = get_reserve_section_opt(argv[i]);

          /* Now add the option to the new argv */
          new_argv[shi+1] = reserve_opt;
            shi++;
          continue;
        }
      } else {
        /* Something other then the ld tool */

        /* We must have code that will recognize this option belongs to 
         * linker and put the correct -Wl in front of the command */
        if (strncmp(argv[i],RESERVE,RESERVE_LEN) == 0) {

          /* Copying the new option with the quotes on either side */
          strcpy(new_option, "\"-Wl,");
          strcat(new_option, argv[i]);
          strcat(new_option, "\"");
          option = new_option;
        }
      }

      /* This code copies the argument from the original argv list to the
       * new argv list */
      if (*argv[i]) {
        new_argv[shi+1] = malloc(strlen(option) + 3); 
        if (new_option[0])
          sprintf(new_argv[shi+1], "%s", option);
        else sprintf(new_argv[shi+1], "\"%s\"", option);
        shi++;
      }
    }

    if (processor) processor=strdup(processor);

    if (partition) {
      if ((partition[0] != '1') && (partition[0] != '2')) {
        fprintf(stderr,
                "Warning: Ignoring unknown partition number '%c'\n", 
                partition[0]);
      } else if (as_tool) {
        new_argv[++shi] = strdup("--partition");
        new_argv[++shi] = strdup("--defsym __DUAL_PARTITION=c");
        new_argv[shi][strlen(new_argv[shi])-1] = partition[0];
      } else if (ld_tool) {
        gcc_argv[++gcci] = strdup("-D__DUAL_PARTITION=c");
        gcc_argv[gcci][strlen(gcc_argv[gcci])-1] = partition[0];
        new_argv[++shi] = strdup("--partition");
      } else if (gcc_tool) {
        new_argv[++shi] = strdup("-mpartition=c");
        new_argv[shi][strlen(new_argv[shi])-1] = partition[0];
      }
    }

    /* when ld_tool is 2 not only do we have ld tool,
     * but it also signifies something about cpp tool, which we need to do
     * linker preprocessing */
    if (ld_tool == 2) {
      struct libpath *f,*t;

      if (processor) processor=strdup(processor);
      if (auxtext) gcc_argv[++gcci] = strdup("-DAUXTEXT");
      if (auxreset) gcc_argv[++gcci] = strdup("-DAUXRESET");
      gcc_argv[++gcci] = processor_family(processor);
      gcc_argv[++gcci] = processor_name(processor);
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
        // if (gcc_argv[gcci]) free(gcc_argv[gcci]);
        // if (gcc_argv[gcci+1]) free(gcc_argv[gcci+1]);
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

    /* When running the cc1 tool, we have to find the -o option and 
       change the suffix from .i to .p */
    /* Also we should remove the mpa option */
    if ( (is_cc1 == 1) && (is_mpa == 1) && (only_preprocess == 0) ) {
      int mpa_num = 0;
      mpa_argv = calloc(sizeof(char *),(argc));
      for (i = 0; new_argv[i]; i++) {
        int quote = 0;

        if (new_argv[i][0] == '"') quote=1;

        if (i == 0) { 
          char * find_cc1;
          char * temp_save = 0;

          mpaEXE = strdup(pszEXE);

          // mpa_argv[mpa_num] = malloc(strlen(new_argv[i]) + 1);
          // memcpy(mpa_argv[mpa_num],new_argv[i],strlen(new_argv[i])+1);

          /* finds cc1, saves anything after cc1, changes cc1 to pa
             and adds whatever was after cc1 */
          find_cc1 = strstr(mpaEXE,"-cc1");
          temp_save = strdup(find_cc1);
          memcpy(find_cc1,"-pa",4);
          strcat(find_cc1,temp_save+4);
          free(temp_save);

          mpa_argv[mpa_num] = malloc(strlen(mpaEXE)+3);
          sprintf(mpa_argv[mpa_num], "\"%s\"", mpaEXE);
          mpa_num++;
        }
        else if (strncmp(&new_argv[i][quote], "-v", 
                 strlen(new_argv[i])-(quote*2)) == 0) {
          MALLOC_MEMCPY_INC(mpa_num,i);
        }
#if 0
        else if (strstr(new_argv[i], "-omf=")) {
          MALLOC_MEMCPY_INC(mpa_num,i);
        }
#endif
        else if (strncmp(&new_argv[i][quote], "-mcpu",5) == 0) {
          char * find_slash;
          mpa_argv[mpa_num] = calloc(strlen(new_argv[i]) + 2,1);
          find_slash = strchr(new_argv[i], '-');
          memcpy(mpa_argv[mpa_num], new_argv[i], find_slash-new_argv[i]);
          strcat(mpa_argv[mpa_num], "-");
          strcat(mpa_argv[mpa_num], find_slash);
          mpa_num++;
        }
        else if (strncmp(&new_argv[i][quote], "-o",
                 strlen(new_argv[i])-(quote*2)) == 0) {
          char * output;
 
          /* copy -o */
          MALLOC_MEMCPY_INC(mpa_num,i);
          
          /* copy output file name from cc1 to pa tool */
          MALLOC_MEMCPY_INC(mpa_num,i+1);

          /* create a new output file cc1 command*/
          output = strstr(new_argv[i+1],".s");

          if (output) output[1] = 'p';
          mpa_temp_file = new_argv[i+1];

          /* add the input file to pa tool */
          MALLOC_MEMCPY_INC(mpa_num,i+1);
        } 
        else if (strncmp(&new_argv[i][quote], "-mpa",
                 strlen(new_argv[i])-(quote*2)) == 0) {
          char * pt_equals;
          pt_equals = strchr(new_argv[i], '=');

          /* check to see if there is nesting level */
          if (pt_equals) {
            mpa_argv[mpa_num] = malloc(strlen(new_argv[i+1]) + 1);
            strcpy(mpa_argv[mpa_num], "\"-n");
            strcat(mpa_argv[mpa_num], pt_equals+1);
            mpa_num++;
          }

          /* erase -mpa from cc1 command */
          strcpy(new_argv[i],"\0");
        }
#if 0
         might add this later 
          else if (strstr(new_argv[i], "-Wpa")) {
          char * first, * second;

          /* First comman found should be after -Wpa and into the  
             options portions */
          first = strpbrk(new_argv[i], ",");

          do {
            second = strpbrk(new_argv[i], ",");
            mpa_argv[mpa_num] = malloc(strlen(new_argv[i]) + 1);
            memcpy(mpa_argv[mpa_num], first+1, (second-first));
            first = second;
          }
          while (second);
        }
#endif 
      }
    } 
        
    PRINT_EXECUTING(new_argv);
    rc = EXECUTE(pszEXE, new_argv);

    /* adding code to invoke pa tool after the cc1 tool 
       if cc1 returned an exit status of 0 */
    if ((is_cc1 == 1) && (is_mpa == 1) && (only_preprocess == 0) && (rc == 0)) {
      PRINT_EXECUTING(mpa_argv);
      rc = EXECUTE(mpaEXE, mpa_argv);
      if ((saving_temp_files == 0) && mpa_temp_file)  {
        int val;
        char *f = mpa_temp_file;

        if (mpa_temp_file[0] = '"') {
           f = &mpa_temp_file[1];
           f[strlen(f)-1] = 0;
        }
        val = unlink(f);
        if (val == -1) {
          fprintf(stderr,
                  "Warning: Unable to delete the created temporary file:\n"
                  "\t%s\n", mpa_temp_file);
        }
        if (f != mpa_temp_file) {
          f[strlen(f)] = '"';
        }
      }
    }

    check_and_delete_reserved_sects();

    // this is ld and we have found pic30-gcc
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
#if 0
    /* Can't be here, we don't always allocate memory for gcc_argv[i] */
    for (i = 1; i < gcci; i++) 
      free(gcc_argv[i]);
#endif
    free(new_argv);
    free(gcc_argv);

    return (rc != 0) * INT_MAX;
}

/************************************************************************/
/*  getEXE() -- Locate the OMF-specific tool executable.                */
/************************************************************************/

static char * getEXE(char *pszThis, char *pszOMF)
{
    static char szCommand[FILENAME_MAX+OMFMAXLEN+3+sizeof(map_to)];
    int nPathLen = 0;
    char *pszSuffix;
    int cbPrefix;
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

    if (strncmp(pszSuffix,"-cc1",4) == 0)
    {
      is_cc1 = 1;
    }

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
        char *pSeparator;
        const char *pszPATH;
        char *dosPATH;
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
                strcat(szCommand, pszOMF);
                strcat(szCommand, pszSuffix);
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
        if (dosPATH_alloced) free(dosPATH);
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
            strcat(szCommand, pszOMF);
            strcat(szCommand, pszSuffix);
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

static char * getOMF(int *argc, char *argv[])
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
        else if ((strcmp(argv[i], MPA) == 0) || 
                 (strncmp(argv[i],MPA"=",sizeof(MPA"=")) == 0))
        {
          is_mpa = 1;
        }
        else if (strncmp(argv[i], OMFCMDOPT, strlen(OMFCMDOPT)) == 0)
        {
            /*
            ** OMF option specified
            */
            char *pszPutativeOMF = argv[i]+strlen(OMFCMDOPT);
            char setenvstr[80];

            if (isCOFF(pszPutativeOMF))
            {
                /*
                ** COFF
                */
                pszOMF = OMFCOFF;
                bFound = TRUE;
            }
            else if (isELF(pszPutativeOMF))
            {
                /*
                ** ELF
                */
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
        } else if (strncmp(argv[i], AUXRESET, strlen(AUXRESET)) == 0) {
          auxreset=1;
          bFound = TRUE;
        } else if (strncmp(argv[i], AUXTEXT, strlen(AUXTEXT)) == 0) {
          auxtext=1;
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
            if (isCOFF(pszPIC30_OMF))
            {
                pszOMF = OMFCOFF;
            }
            else if (isELF(pszPIC30_OMF))
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
       printf("\n  -omf={coff|elf}\t"
              "Select coff or elf object module format\n");
       if (ld_tool) {
         printf("\n  --no-cpp\t\t"
                "Do not pre-process linker scripts\n" 
                "           \t\tspecified with -T or --script\n");
         printf("\n  --save-gld=<root>\t"
                "Save pre-processed linker scripts using <root>\n");
         printf("\n  -D<macro>[=<value>]\t"
                "This option is passed to the C pre-processor\n");
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
/*  isCOFF() -- Determine if a string names the COFF OMF.               */
/************************************************************************/

static int isCOFF(char *pszPutativeOMF)
{
    return(isOMF(pszPutativeOMF, OMFCOFF));
}

/************************************************************************/
/*  isELF() -- Determine if a string names the ELF OMF.                 */
/************************************************************************/

static int isELF(char *pszPutativeOMF)
{
    return(isOMF(pszPutativeOMF, OMFELF));
}

/************************************************************************/
/*  isOMF() -- Determine if a string names an OMF.                      */
/************************************************************************/

static int isOMF(char *pszPutativeOMF, char *pszOMF)
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

    buffer = malloc(finfo.st_size);
    if (buffer == 0) {
      fprintf(stderr,"Cannot allocate %d bytes\n", finfo.st_size);
      return 1;
    }

    actual_size = fread(buffer, 1, finfo.st_size, argfile);
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
    
