/*
 * main.c
 *
 *  Created on: Feb 19, 2016
 *      Author: seif
 */

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

#define ATMEGA32   "m32def.inc"
#define INCFILE	   "inc.txt"
#define COMFREE_F  "comnts_free.txt"
#define CAPITAL_F  "caps_file.txt"
#define FINAL_F    "final.txt"
#define READY_F	   "intermedite.txt"
#define HEX_F	   "out.hex"

void inc_files(char const*,char const*);
void cap_file(char const*);
void parse(char const*);
void rm_comments(char const*);
void rm_spaces(char const*);
void directives_replace(char const*);
void directive_replace_help(FILE*,FILE*,char*);
void replaceAllString(char *, const char *, const char *, char*);
void convhex(char const*);
int equcount(FILE* );

int main(int argc, char* argv[])
{
	char bufreb[255];
	int i=0;
	int count=0;
	FILE *fin, *fout, *file;
	
	// check if no input argument or file name
	if(argc < 2 && argc >3)
	{		
		puts("******* AVR assembler*******");
		puts("Arguments: one argument at least expected(file.asm).");
		puts("Options: -o to name the output hex file");
		puts("Usage: asm.exe \"file.asm\" -r \"out.hex\"");
		
	}
	// if there is an input argument (file name)
	else
	{		
		inc_files(argv[1],ATMEGA32);
		cap_file(INCFILE);
		rm_comments(CAPITAL_F);
		rm_spaces(COMFREE_F);
		file = fopen(FINAL_F,"r");
		count = equcount(file);
		for(i = 0; i < count; i++)
		{
		directives_replace(FINAL_F);
		remove(FINAL_F);
		rename("out.txt",FINAL_F);
		}
		//convhex(READY_F);
		/*if(strcmp(argv[2],"-o")==0 && argc == 3)
		{
			rename(HEX_F, argv[3]);
		}
		else
		{
			//file stays at default name
		}*/
			
		
		
		
		
		
	}
	return 0;
}
void inc_files(char const* asm_file,char const* inc_file)
{
	FILE *f1, *f2, *ff;
	char ch;
	f1 = fopen(asm_file,"r");
	f2 = fopen(inc_file,"r");
	if (f1 != NULL && f2 != NULL)
	{
		ff = fopen(INCFILE,"w");
		if (ff != NULL)
		{
			while( ( ch = fgetc(f2) ) != EOF )
			{
				fputc(ch,ff);
			}
			fputc('\n',ff);
			while( ( ch = fgetc(f1) ) != EOF )
				  fputc(ch,ff);
			
			fclose(f1);
			fclose(f2);
			fclose(ff);
		}
		else
		{
			puts("ERROR: couldn't open files");
		}
	}
	else
	{
		puts("ERROR: couldn't open files");
	}
	fclose(f1);
	fclose(f2);
	fclose(ff);
}		

void cap_file(char const* file)
{
	FILE *fs, *fc;
	char c;	
	fs = fopen(file,"r");
	if(fs != NULL)
	{
		fc = fopen(CAPITAL_F, "w");
		if (fc != NULL)
		{
			while((c = fgetc(fs)) != EOF)
			{
				if(c == ',' || c == '\t')
				{
					c = ' ';
				}
				else
				{
					if (c >=97 && c<=122)
						c = c-32;
				}
				fputc(c, fc);
				
			}
		
		} else{printf("couldnt open caps out file");}
	} else{printf("ERROR: Couldn't open inout cap");}
	fclose(fs);
	fclose(fc);
}

void rm_comments(char const* filename)
{
	 FILE* file = fopen(filename, "r"); /* should check the result */
    FILE * hfPtr;
    int flag=0;
	char ch;
    if ( (hfPtr=fopen(COMFREE_F,"w") ) == NULL )
	{
	//printf( "writable File could not be opened\n" );
	}
    
    else {
    
while( ( ch = fgetc(file) ) != EOF )
{
	     
if(ch==';' || ch=='/' ||ch=='#' || ch=='*')
           {
            flag=1;
           }
          else if (ch=='\n') {
            flag=0;
          }
          if (flag==1)
          {
             ch=' ';
          }
       fputc(ch,hfPtr);
	   }  
}
    fclose(file);
    fclose(hfPtr);
 return ;
	/*FILE *fin ,*fout;
	int flag=0;
	char c;
	fin = fopen(file, "r");
	if(fin != NULL)
	{	
		fout = fopen(COMFREE_F,"w");
		if (fout != NULL)
		{
		while((c=fgetc(fin)) != EOF)
		{
			if (c == ';' || c == '*' || c == '/' || c == '#')
			{
				flag = 1;
			}
			
			if (flag == 1 && c != '\n')
			{
				continue;
			}			
			flag = 0;
			fputc(c, fout);
		}
		}
		else{ puts("ERROR");}
	}
	else
	{
		puts("ERROR: couldn't open file");
	}		
	fclose(fin);
	fclose(fout);*/
}

void rm_spaces(char const* file)
{
	FILE* fin, *fout;
	fin = fopen(file,"r");
	char c;
	int flag = 1;
	if (fin != NULL)
	{
		fout  = fopen(FINAL_F, "w");
		if(fout != NULL)
		{
		while( ( c = fgetc(fin) ) != EOF )
		{
			if(c=='\n'&& (flag==0))
			{
				flag=1;
				fputc(c,fout);
			}
			else if ((flag==1) && (c!='\n') && (c!=' '))
			{ 
				fputc(c,fout);
				flag=0;
			}     
			else if(flag==0)
			{
				fputc(c,fout); 
			}
		}
		} else {puts("couldn't open spaces out file");}
	}
	else
	{
		puts("ERROR:couldnt open spaces in file");
	}
	fclose(fin);
	fclose(fout);
}
void directives_replace(char const* filename)
{
	FILE* fin, *fout;
	char buffer[255];
	fin = fopen(filename,"r");
		
		if (fin != NULL)
		{
			fout = fopen("out.txt","a+");
			if (fout != NULL)
			{
				while (fgets(buffer, sizeof(buffer), fin)) 
				{
					if(memcmp(".EQU",buffer,4)==0||memcmp(".DEF",buffer,4)==0)
					{
						//cur_pos = ftell(fin);
						directive_replace_help(fin,fout,buffer);
											
					}
					else if(memcmp(".DEVICE",buffer,7)==0)
					{
						continue;
					}
					else
					{
						fputs(buffer,fout);
					}
					
				}	
			}else{puts("ERROR: couldnt open out file");}
		}else{puts("ERROR: couldnt open source file");}
		
	fclose(fin);
	fclose(fout);
	}

	

	
void directive_replace_help(FILE* in, FILE*out, char*string)
{
	char buffer[255];
	char *tokenArr[4];
	char *token;
	char *search;
	char *p;
	int i = 0;
		
	token = strtok(string, " ");
	while ( token != NULL )
	{
		tokenArr[i] = token;
		i++;
		token = strtok(NULL, " ");
	}
	i = 0;
	
	p = strchr(tokenArr[3], '\n');  
	if (p != NULL) {strcpy(p, "");}

	while (fgets(buffer, sizeof(buffer), in))
	{
	    
		search = strstr(buffer, tokenArr[1]);  //search for token in line
		if (search != NULL)  //if found
		{ 
			char result[250];			
			replaceAllString(buffer,tokenArr[1],tokenArr[3],buffer); //replace in buffer 			
			
		}
		
		fputs(buffer,out);		
	}
}
void replaceAllString(char *buf, const char *orig, const char *replace, char* result)
{
    int olen, rlen;
    char *s, *d;
    char *tmpbuf;

    if (!buf || !*buf || !orig || !*orig || !replace)
        return;

    tmpbuf = malloc(strlen(buf) + 1);
    if (tmpbuf == NULL)
        return;


    olen = strlen(orig);
    rlen = strlen(replace);

    s = buf;
    d = tmpbuf;

    while (*s) {
        if (strncmp(s, orig, olen) == 0) {
            strcpy(d, replace);
            s += olen;
            d += rlen;
        }
        else
            *d++ = *s++;
    }

    *d = '\0';

    strcpy(result, tmpbuf);
    free(tmpbuf);
}

int equcount(FILE* tmp)
{
  int count=0;
  char line[250];
     while (fgets(line, sizeof(line), tmp)) 
    {
      //printf("%s \n",line);
      if (!memcmp(".EQU", line, 4) || !memcmp(".DEF", line, 4))
      {
        count++;
      }
    }
    //printf("%d \n",count);
     fclose(tmp);
return count;
}