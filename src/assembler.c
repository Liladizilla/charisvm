#include "cpu.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 512u
#define LABEL_SIZE 32u
#define MAX_LABELS 256u

typedef struct { char name[LABEL_SIZE]; unsigned address; } Label;
static Label labels[MAX_LABELS];
static size_t label_count;

static void trim(char *s)
{
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1u])) s[--n] = '\0';
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1u);
}
static void upper(char *s) { for (; *s; s++) *s = (char)toupper((unsigned char)*s); }
static int reg_id(const char *s)
{
    if (s[0] == 'R' && s[1] >= '0' && s[1] <= '3' && s[2] == '\0') return s[1] - '0';
    return -1;
}
static int find_label(const char *name)
{
    for (size_t i=0; i<label_count; i++) if (strcmp(labels[i].name,name)==0) return (int)labels[i].address;
    return -1;
}
static int number_or_label(const char *s, unsigned *out)
{
    char *end = NULL;
    errno = 0;
    unsigned long v = strtoul(s, &end, 0);
    if (end != s && *end == '\0' && errno == 0 && v <= 255ul) { *out=(unsigned)v; return 1; }
    int addr = find_label(s);
    if (addr >= 0) { *out=(unsigned)addr; return 1; }
    return 0;
}
static int encode(char *line, uint8_t *out, size_t *n)
{
    char *comment = strchr(line, '#'); if (comment) *comment='\0';
    trim(line); if (!*line) { *n=0; return 1; }
    char *colon=strchr(line, ':');
    if (colon) { *colon='\0'; trim(line); if (*line) { int a=find_label(line); if(a<0) return 0; } memmove(line,colon+1,strlen(colon+1)+1u); trim(line); if(!*line){*n=0;return 1;} }
    char *args=strchr(line,' '); if(!args) args=strchr(line,'\t');
    if(args){*args++='\0';trim(args);} else args=(char *)"";
    upper(line);
    char *a=NULL,*b=NULL;
    if(*args){ a=args; char *comma=strchr(args,','); if(comma){*comma='\0'; b=comma+1; trim(a);trim(b);} }
    unsigned x=0,y=0;
    if(!strcmp(line,"HLT")){out[0]=OP_HLT;*n=1;return !*args;}
    if(!strcmp(line,"LDI")||!strcmp(line,"MOV")||!strcmp(line,"ADD")||!strcmp(line,"SUB")||!strcmp(line,"LOAD")||!strcmp(line,"STORE")){
        if (!a || !b) return 0;
        upper(a);
        upper(b);
        int r = reg_id(a);
        if(r<0)return 0;
        if(!strcmp(line,"LDI")){if(!number_or_label(b,&y))return 0;out[0]=OP_LDI;out[1]=(uint8_t)r;out[2]=(uint8_t)y;}
        else if(!strcmp(line,"MOV")||!strcmp(line,"ADD")||!strcmp(line,"SUB")){int s=reg_id(b);if(s<0)return 0;out[0]=!strcmp(line,"MOV")?OP_MOV:!strcmp(line,"ADD")?OP_ADD:OP_SUB;out[1]=(uint8_t)r;out[2]=(uint8_t)s;}
        else {if(!number_or_label(b,&y))return 0;out[0]=!strcmp(line,"LOAD")?OP_LOAD:OP_STORE;out[1]=(uint8_t)r;out[2]=(uint8_t)y;}
        *n=3;return 1;
    }
    if(!strcmp(line,"OUT")){if(!*args)return 0;upper(args);int r=reg_id(args);if(r<0)return 0;out[0]=OP_OUT;out[1]=(uint8_t)r;*n=2;return 1;}
    if(!strcmp(line,"JMP")||!strcmp(line,"JZ")){upper(args);if(!*args||!number_or_label(args,&x))return 0;out[0]=!strcmp(line,"JMP")?OP_JMP:OP_JZ;out[1]=(uint8_t)x;*n=2;return 1;}
    return 0;
}
static int read_lines(const char *path, char lines[][LINE_SIZE], size_t *count)
{
    FILE *f=fopen(path,"r"); if(!f){perror(path);return 0;}
    *count=0; while(fgets(lines[*count],LINE_SIZE,f)){if(++*count>=256u){fprintf(stderr,"charisasm: too many lines (max 255)\n");fclose(f);return 0;}}
    int ok=!ferror(f); fclose(f); return ok;
}
int main(int argc,char **argv)
{
    char lines[256][LINE_SIZE]; size_t count=0;
    if(argc!=3){fprintf(stderr,"Usage: %s input.asm output.hex\n",argv[0]);return EXIT_FAILURE;}
    if(!read_lines(argv[1],lines,&count))return EXIT_FAILURE;
    /* Pass one: collect label addresses and instruction sizes. */
    unsigned pc=0;
    for(size_t i=0;i<count;i++){
        char copy[LINE_SIZE];snprintf(copy,sizeof(copy),"%s",lines[i]);
        char *comment=strchr(copy,'#');if(comment)*comment='\0';trim(copy);if(!*copy)continue;
        char *colon=strchr(copy,':');
        if(colon){*colon='\0';trim(copy);upper(copy);if(!*copy||strlen(copy)>=LABEL_SIZE||label_count>=MAX_LABELS||find_label(copy)>=0){fprintf(stderr,"charisasm:%zu: invalid or duplicate label\n",i+1u);return EXIT_FAILURE;}strcpy(labels[label_count].name,copy);labels[label_count++].address=pc;memmove(copy,colon+1,strlen(colon+1)+1u);trim(copy);if(!*copy)continue;}
        char *sp=strpbrk(copy," \t");if(sp)*sp='\0';upper(copy);
        unsigned size=!strcmp(copy,"HLT")?1u:(!strcmp(copy,"OUT")||!strcmp(copy,"JMP")||!strcmp(copy,"JZ"))?2u:3u;
        if(pc+size>CPU_RAM_SIZE){fprintf(stderr,"charisasm:%zu: program exceeds 256 bytes\n",i+1u);return EXIT_FAILURE;}pc+=size;
    }
    FILE *out=fopen(argv[2],"w");if(!out){perror(argv[2]);return EXIT_FAILURE;}
    pc=0;
    for(size_t i=0;i<count;i++){
        char copy[LINE_SIZE];snprintf(copy,sizeof(copy),"%s",lines[i]);uint8_t bytes[3];size_t n=0;
        if(!encode(copy,bytes,&n)){fprintf(stderr,"charisasm:%zu: invalid instruction or operand\n",i+1u);fclose(out);remove(argv[2]);return EXIT_FAILURE;}
        for(size_t j=0;j<n;j++){fprintf(out,"%02X%s",bytes[j],j+1u==n?"":" ");pc++;}
        if(n)fputc('\n',out);
    }
    if(fclose(out)!=0){perror(argv[2]);remove(argv[2]);return EXIT_FAILURE;}
    printf("Assembled %u byte(s) into %s\n",pc,argv[2]);return EXIT_SUCCESS;
}
