/*
 *   Resource generator, Copyright 2006 Microchip Technology Inc
 *   Updated by CyberCastle, for remove obsolete code.
 */
static const char *version = "0.1 (alpha)";

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "grammar.h"
#include "../xc32/xc32_flag_definitions.h"

extern FILE *yyin;
extern int yydebug;

int lineno=1;
int errcnt;
char *filename;
int display_warnings;
int split_res = 0;
unsigned int total_alloc = 0;
int pic30_family_mask = FAMILY_MASK;

void write_data_item(struct resource_data *d, int width, FILE *f) {
  unsigned char buffer[sizeof(int)];
  char *s;
  char zero = 0;
  int len;

  switch (d->kind) {
    case rik_null:    while (width >= 1) {
                        fwrite(&zero, 1, 1, f);
                        width--;
                      }
                      break;

    case rik_char:    fwrite(&d->v.c, sizeof(char), 1, f);
                      if (width) {
                        width--;
                        while (width >= 1) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_int:     buffer[0] = (d->v.i & 0xFF);
                      buffer[1] = ((d->v.i >> 8) & 0xFF);
                      buffer[2] = ((d->v.i >> 16) & 0xFF);
                      buffer[3] = ((d->v.i >> 24) & 0xFF);
                      fwrite(buffer, 4, 1, f);
                      if (width) {
                        width -= 4;
                        while (width >= 1) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_version: buffer[0] = (d->v.version.major & 0xFF);
                      buffer[1] = ((d->v.version.major >> 8) & 0xFF);
                      buffer[2] = (d->v.version.minor & 0xFF);
                      buffer[3] = ((d->v.version.minor >> 8) & 0xFF);
                      fwrite(buffer, 4, 1, f);
                      if (width) {
                        width -= 4;
                        while (width > 0) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_string: 
 s = strdup(d->v.s);
                      resource_pack_string(s);
                      len = strlen(s)+1;
                      fwrite(s, len, 1, f);
                      if (width) {
                        width -= len;
                        while (width > 0) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      free(s);
                      break;
  }
}

int indent_fprintf(FILE *stream, char *format, ...) {
  static int indent_level=0;
  static char buffer[] = "%12345s";
  int indent_to = 0;
  va_list ap;
  char *c = format;
  char last_c;
  int result = 0;

  va_start(ap,format);
  last_c = 0;
  for (c = format; *c; c++) {
    switch (*c) {
      case '<': indent_to += 2;
                break;
      case '/': if (last_c == '<') {
                  indent_to = 0; 
                  indent_level -= 2;
                }
                else if (c[1] == '>') indent_to -= 2;
                break;
      default:  break;
    }
    last_c  = *c;
  }
  sprintf(buffer,"%%%ds", indent_level);
  fprintf(stream, buffer, "");
  result = vfprintf(stream, format, ap);
  va_end(ap);
  indent_level += indent_to;
  if (indent_level < 0) indent_level = 0;
  if (indent_level > 40) indent_level = 40;
  return result;
}

typedef struct tag_device_dict {
	char name[20];
  unsigned int  value;
  unsigned char family;
  struct tag_device_dict * next;
} device_dict;

int main(int argc, char **argv){

  int counter = 0;
  int input_arg;
  char *cmd_buffer;
  char *buffer;
  int buffer_size = 256;
  char tempfile[256];
  char *c;
  FILE *f;
  FILE *resource;
  FILE *xml;
  FILE *device_file;
  FILE *deviceSupportXML;
  struct resource_data d;
  struct record *r;
  int i,j;
  char *space=" ";
  int pic30_family_mask = FAMILY_MASK;
  char *build_version="no version specified";
  struct tm *tm_current;
  struct timeval current_time;
  char timep[64];

  fprintf(stderr,"Resource generation tool (Version %s)\n\n", version);

  gettimeofday(&current_time,NULL);
  tm_current = localtime(&current_time.tv_sec);
  strftime(timep, sizeof(timep), "%Y-%m-%d", tm_current);

  buffer = calloc(1, buffer_size);
  i = 0;
  for (input_arg = 1; input_arg < argc-1; input_arg++) {
    if (*argv[input_arg] == '+') {
       switch (argv[input_arg][1]) {
         case 'd': yydebug=1;
                   break;
         case 'f': pic30_family_mask = strtol(argv[++input_arg],0,0);
                   break;
         case 'w': display_warnings=1;
                   break;
         case 's': split_res=1;
                   break;
         case 'v': build_version=argv[++input_arg];
                   break;
         default:  fprintf(stderr,"Ignoring rg option %s\n", argv[input_arg]);
                   break;
       }
    } else if (*argv[input_arg] == '-') {
       /* option to the pre-processor */
       while (i + strlen(argv[input_arg]) + 1 >= buffer_size) {
         char *new_buffer;

         buffer_size += 256;
         new_buffer = realloc(buffer, buffer_size);
         if (new_buffer != buffer) free(buffer);
         buffer = new_buffer;
       }
       strcat(buffer, argv[input_arg]);
       strcat(buffer, space);
       i += strlen(argv[input_arg]) + 1;
    } else break;
  }

  sprintf(tempfile,argv[input_arg]);
  for (c = tempfile; (*c) && (*c != '.'); c++);
  if (*c == '.') {
    c[1] = 'i';
    c[2] = 0;
  } else {
    strcat(tempfile,".i");
  }

  fprintf(stderr,"All error lines refer to file: %s\n", tempfile);
  cmd_buffer = malloc(strlen(buffer) + 
                      strlen(argv[input_arg]) + 
                      strlen(tempfile) + 20);
  sprintf(cmd_buffer,"gcc %s -x c -E %s -o %s", buffer, argv[input_arg], 
                                                tempfile);
  fprintf(stderr,"Executing... %s\n", cmd_buffer);

  f = popen(cmd_buffer,"r");
  pclose(f);

  yyin = fopen(tempfile, "r");

  if (yyin == 0) {
     fprintf(stderr,"\nExiting due to pre-processing errors\n");
     exit(1);
  }

  filename = tempfile;
  yyparse();

  if (errcnt) {
    fprintf(stderr,"\nTotal allocation: %d\n",total_alloc);
    fprintf(stderr,"\nERROR: Exiting due to errors\n");
    exit(1);
  }

  if (split_res==1) {
    int device_counter = 0;
    for (r = records; r; r = r->next) {

      /* Check if the record is a device */
      if ( (strncmp(r->fields[0]->v.s,"32",2)  == 0)   ||
           (strncmp(r->fields[0]->v.s,"MGC",3)  == 0)  ||
           (strncmp(r->fields[0]->v.s,"MEC",3) == 0)   ||
           (strncmp(r->fields[0]->v.s,"MTCH63",6) == 0)   ||
           (strncmp(r->fields[0]->v.s,"USB49",5) == 0) ||
           (strncmp(r->fields[0]->v.s,"USB7",4) == 0) ||
           (strncmp(r->fields[0]->v.s,"IPS",3) == 0)) {

        struct resource_data *last = 0;
        /*Create the device file*/
        char name[40];
        strcpy(name,r->fields[0]->v.s);
        strcat(name,".info");
        device_file = fopen(name,"w");
  
        if (!device_file) {
          fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
          exit(1);
        }
  
        d.kind = rik_string;
        d.v.s = rib.tool_chain;
        write_data_item(&d, 0, device_file);

        d.kind = rik_version;
        d.v.version.major = rib.version.major;
        d.v.version.minor = rib.version.minor;
        write_data_item(&d, 0, device_file);

        d.kind = rik_char;
        d.v.c = rib.resource_version_increment;
        write_data_item(&d, 0, device_file);

        d.kind = rik_int;
        d.v.i = rib.field_count;
        write_data_item(&d, 0, device_file);
  
        for (i = 0; i < rib.field_count; i++) {
          d.v.i = rib.field_sizes[i];
          write_data_item(&d, 0, device_file);
        }
  
        d.v.i = 0xFFFFFFFF;
        write_data_item(&d,0,device_file);
       
        for (i = 0; i < rib.field_count; i++) {
          if (!last) {
            write_data_item(r->fields[i], rib.field_sizes[i], device_file);
            if (r->fields[i]->kind == rik_null) last = r->fields[i];
          } else {
            write_data_item(last, rib.field_sizes[i], device_file);
          }
        }
        fclose(device_file);
      }
    }
  }

  else {

    fprintf(stderr, "\nWriting resource file %s...\n", argv[input_arg+1]);
    resource = fopen(argv[input_arg+1],"wb");
  
    if (!resource) {
      fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
      exit(1);
    }
  
    if (argv[input_arg+2]) {
      fprintf(stderr, "\nWriting xml file %s...\n", argv[input_arg+2]);
      xml = fopen(argv[input_arg+2],"wb");
  
      if (!xml) {
        fprintf(stderr,"Could not open file %s\n", argv[input_arg+2]);
        exit(1);
      }
    } else xml = 0;

    /* we should make this an option ... */
    if (argv[input_arg+3]) {
      fprintf(stderr, "\nWriting device support xml file %s...\n", 
              argv[input_arg+3]);
      deviceSupportXML = fopen(argv[input_arg+3],"wb");
  
      if (!deviceSupportXML) {
        fprintf(stderr,"Could not open file %s\n", argv[input_arg+2]);
        exit(1);
      }
    } else deviceSupportXML = 0;

    d.kind = rik_string;
    d.v.s = rib.tool_chain;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_version;
    d.v.version.major = rib.version.major;
    d.v.version.minor = rib.version.minor;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_char;
    d.v.c = rib.resource_version_increment;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_int;
    d.v.i = rib.field_count;
    write_data_item(&d, 0, resource);
  
    for (i = 0; i < rib.field_count; i++) {
      d.v.i = rib.field_sizes[i];
      write_data_item(&d, 0, resource);
    }
  
    d.v.i = 0xFFFFFFFF;
    write_data_item(&d,0,resource);
  
    for (r = records; r; r = r->next) {
      struct resource_data *last = 0;
  
      for (i = 0; i < rib.field_count; i++) {
        if (!last) {
          write_data_item(r->fields[i], rib.field_sizes[i], resource);
          if (r->fields[i]->kind == rik_null) last = r->fields[i];
        } else {
          write_data_item(last, rib.field_sizes[i], resource);
        }
      }
    }
  
  
    fclose(resource);
    
    if (xml) {
      int families[] = {P32MX, P32MZ, P32MM, P32PR, P32WK, P32MK, PUSB49, PUSB7,  0};
      char *family_names[] = { "PIC32MX","PIC32MZ","PIC32MM","PIC32PR","PIC32WK","PIC32MK","USB49", "USB7" };

      int f;

      fprintf(xml,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                  "\n");
      indent_fprintf(xml,"<lti:LanguageToolSuite\n");
      indent_fprintf(xml,"lti:uuid=\"{ec0473c6-f276-4a29-9b4a-41f899a460a2}\""
                         "\n");
      indent_fprintf(xml,
                    "lti:name=\"MPLAB XC for PIC32MX/PIC32MZ\"\n");
      indent_fprintf(xml,"lti:version=\"%d.%d\"\n",
                     rib.version.major, rib.version.minor);
      indent_fprintf(xml,"xmlns:lti=\"http://www.microchip.com/lti\"\n");
      indent_fprintf(xml,
                     "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
      indent_fprintf(xml,"xsi:schemaLocation=\""
                     "http://www.microchip.com/lti lti/LanguageToolSuite.xsd\">"
                     "\n");
      fprintf(xml,"\n");

      indent_fprintf(xml,"<lti:BaseDir lti:dir=\"..\"/>\n\n");

      for (f = 0; families[f]; f++) {
        int header_dumped = 0;

        if ((families[f] & pic30_family_mask) == 0) continue;
        for (r = records; r; r = r->next) {
          struct resource_data *last = 0;

          if ((r->fields[1]->kind == rik_int) &&
              (r->fields[1]->v.i & families[f])) {
            /* device for the family we are currently dumping */
            if (!header_dumped) {
              header_dumped++;
              indent_fprintf(xml,"<lti:DeviceFamily lti:family=\"%s\">\n\n",
                             family_names[f]);
              indent_fprintf(xml,"<lti:IncludeDir lti:dir=\"pic32mx/include\"/>\n");
              fprintf(xml,"\n");
            }
            indent_fprintf(xml,"<lti:Device lti:device=\"%s\"/>\n",
                           r->fields[0]->v.s);
          }
        }
        if (header_dumped) {
          indent_fprintf(xml,"</lti:DeviceFamily>\n\n");
        }
      }
      indent_fprintf(xml,"</lti:LanguageToolSuite>\n");
      fclose(xml);
    }

    if (deviceSupportXML) {
      const char *header = 
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<mp:deviceSupport xmlns:mp=\"http://crownking/mplab\"\n"
        "         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
        "         xsi:schemaLocation=\"http://crownking/mplab%20mplab/deviceSupport\"\n";

      const char *end_header = ">\n\n";
      const char *footer = "</mp:deviceSupport>";
      int families[] = {
        P32MX | P32MZ | P32MM | P32PR | P32WK | P32MK | PUSB49 | PUSB7, 
        0
      };

      char *family_names[] = { "32x",
                               "32x",
                             };
      int f;
      fprintf(deviceSupportXML, "%s", header);
      fprintf(deviceSupportXML, "         mp:version=\"%s\"\n", build_version);
      fprintf(deviceSupportXML, "         mp:builddate=\"%s\"\n",timep);
      fprintf(deviceSupportXML, "%s", end_header);
      for (f = 0; families[f]; f++) {
        int header_dumped = 0;

        if ((families[f] & pic30_family_mask) == 0) continue;
        for (r = records; r; r = r->next) {
          struct resource_data *last = 0;

          if ((r->fields[1]->kind == rik_int) &&
              ((r->fields[1]->v.i & RECORD_TYPE_MASK) == IS_DEVICE_ID) &&
              (r->fields[1]->v.i & families[f])) {
            /* device for the family we are currently dumping */
            if (!header_dumped) {
              header_dumped++;
              indent_fprintf(deviceSupportXML,
                             "<mp:family mp:family=\"%s\">\n\n",
                             family_names[f]);
              fprintf(deviceSupportXML,"\n");
            }
            indent_fprintf(deviceSupportXML,
                           "<mp:device mp:name=\"%s%s\"/>\n",
                           r->fields[0]->v.s[0] == '3' ? "PIC" : "",
                           r->fields[0]->v.s);
          }
        }
        if (header_dumped) {
          indent_fprintf(deviceSupportXML,"</mp:family>\n\n");
        }
      }
      fprintf(deviceSupportXML, "%s", footer);
      fclose(deviceSupportXML);
    }
  }

  fprintf(stderr,"Total allocation: %d\n",total_alloc);
  return EXIT_SUCCESS;
}
                
