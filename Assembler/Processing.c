#include <stdio.h>
#include <ctype.h>
#include<conio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#define NOOFINSTRUCTIONS 4

#define ERROR            0
#define OKAY             1
#define NOT_DECIDED      2

#define NOOFOPERANDS     2
#define opCol			 1

static FILE* HexFile;
static u8 EndOfFile = 0;
static u8 Instructions[][5][50] = {
    { "LDI", "1110BBBBAAAABBBB", "2","Fixed","d,K" },
    { "LDS", "1001000ddddd0000kkkkkkkkkkkkkkkk", "2","fixed","d,k" },
    { "NOP", "0000000000000000", "0","fixed" },
    { "NOP", "0000000001000000", "0","fixed" },
};
/*******************************************opens file for reading purpose,returns a pointer to the file****************************/

FILE* ReadOpen(u8 fileName[])
{	
	FILE *fp;
	fp = fopen(fileName, "r");
	if (fp == NULL)
    {
	   printf("file doesn't exist");
    }
    
	return fp;
}
/*******************************************read the file opened from ReadOpen function,returns the line read****************************/
u8* ReadOn(FILE *fp)
{
	static u8 line[100];
	u8 *Readstatus;
	Readstatus = fgets(line,100,(FILE*)fp);
	if(Readstatus != NULL) 
	return line;
	else return Readstatus;
}

/*******************************************opens file for writing purpose,returns a pointer to the file****************************/
FILE* WriteOpen(u8 fileName[])
{	
   FILE *hex;
   hex = fopen(fileName, "w");
   if (hex == NULL)
   {
	   printf("file doesn't exist");
   }
	
  return hex;
}
/*******************************************write in the file opened from WriteOpen function******************************************/
void WriteOn(FILE *fp,u8* Opcode)
{
	//fprintf(fp, "This is testing for fprintf...\n");
	fputs( Opcode,fp );
}
/****************************************************closes any file********************************************/
void RWClose(FILE *fp)
{
	fclose(fp);
}
/*******************************************missing types****************************/
void errorTypes(u8 *error,u8 type,u8 InvalidExpression,u8 *inst)
{ u8 count = 0;
	u8 one[] = "missing expression or extra coloumn\n";
	u8 two[] = "missing ' \n";
	u8 three[] = "invalid expression";
	u8 four[] = "Invalid instruction";
	u8 instr =0;
	if (type == 1)
	{
		for(count = 0; count<40;count++)
			error[count] = one[count];
	}
	if (type == 2)
	{
		for(count = 0; count<20;count++)
			error[count] = two[count];
	}
	if (type == 3)
	{
		for(count = 0; count<18;count++)
			error[count] = three[count];
		error[count] = ' ';
		error[count+1] = ':';
		error[count+2] = ' ';
		error[count+3] = InvalidExpression;
		printf("%s\n",error);
	}
	if(type == 4)
	{
		for(count = 0; count<19;count++)
			error[count] = four[count];
		error[count] = ' ';
		error[count+1] = ':';
		error[count+2] = ' ';
		instr = 0;
		for(count = 22; count<28;count++)
		{error[count] = inst[instr];instr++;}
			
	}
	
}
u8 ClacCheckSum(u32 * Data,u8 counts, u8 * DataLenght)
{
	u8 CS = 0;
	u16 HighNibble;
	u16 LowNibble;u8 numofBytes;
	u8 i = 0;
	for(i = 0;i<counts;i++)
	{
		HighNibble = (u16)(((*(Data+i))&0XFFFF0000)>>16);
		LowNibble =  (u16)((*(Data+i))&0X0000FFFF);
		CS = (u8)(CS+HighNibble+LowNibble);
	}
	for(i = 0;i<counts;i++)
		numofBytes = (*(DataLenght+i)) + numofBytes;
	CS = (u8)(100-(CS + numofBytes));
	return CS;
}
void AccumelateLine(u32 Opcode,u8 opCodeLengthInBytes)
{	
	static u8 countOpcode = 0;
	static  u32 Data[16];
	static u8 DataLenght[16];
	static u16 Address = 0;
	u8 Byte1;
	u8 Byte2;
	u8 Byte3;
	u8 Byte4;
	u8 CheckSum;
	static u8 counts = 0;
	u8 i;
	//printf("first %d\n",countOpcode);
	if(EndOfFile != 1)
	{	Byte1 = Opcode & 0x000000FF;
		Byte2 = ((Opcode & 0x0000FF00)>>8);
		Byte3 = ((Opcode & 0x00FF0000)>>16);
		Byte4 = ((Opcode & 0xFF000000)>>24);
		if(opCodeLengthInBytes == 2)
		Opcode = Byte2 |(Byte1<<8);
		else
			Opcode = ((Byte1<<24) | (Byte2<<16) | (Byte3<<8) | (Byte4));
		Data[counts] = Opcode;
		DataLenght[counts] = opCodeLengthInBytes;
		//countOpcode++;//if not end of file do not increment the count
	}
	if(countOpcode == 16 || EndOfFile == 1)
	{
		fprintf(HexFile,":%.2X%0.4X00",countOpcode,Address);//prints counter then address then 00
		for(i = 0;i<counts;i++)
		{	if(DataLenght[i] == 2)
			fprintf(HexFile,"%0.4X",Data[i]);//prints data
			else
				fprintf(HexFile,"%0.8X",Data[i]);
		}
		CheckSum = ClacCheckSum(Data,(counts+1),DataLenght);
		fprintf(HexFile,"%X\n",CheckSum); // prints check sum
		
		Address = Address +  16;
		countOpcode =0;
	}
	//printf("AccumelateLine");
	countOpcode = countOpcode + opCodeLengthInBytes;counts++;
}
/*******************************************function that generates OP CODE****************************/
u32 generateOpcode(u16 Operand1,u16 Operand2,u8 instNo,u8* opCodeLengthInBytes)
{	s16 count = 0;
	u8 opcodeLength;
	u32 opCodeDec   = 0;
	u8 A = 0;
	u8 B = 0;
	/* printf("%d\n",Operand1);
	printf("%d\n",Operand2);
	printf("%d\n",instNo); */
	//calculating opCode lenght
	while(*((Instructions[instNo][opCol])+count) != '\0'){count++;}*opCodeLengthInBytes = (u8)(count/8);count--;opcodeLength = count;//printf("%d\n",count);
	
		while(count >= 0)
		{ 	//printf("%c",*((Instructions[instNo][1])+count));
			switch(*((Instructions[instNo][opCol])+count))
			{	
				case '0': ClrBit(opCodeDec,opcodeLength-count);break;
				case '1': SetBit(opCodeDec,opcodeLength-count);break;
				case 'A': 
						switch(GetBit(Operand1,A))
						{
							case 0: ClrBit(opCodeDec,opcodeLength-count);A++;break;
							case 1: SetBit(opCodeDec,opcodeLength-count);A++;break;
							default: break;
						}
						break;
				case 'B': 
						switch(GetBit(Operand2,B))
						{
							case 0: ClrBit(opCodeDec,opcodeLength-count);B++;break;
							case 1: SetBit(opCodeDec,opcodeLength-count);B++;break;
						}
						break;
				default:break;	
			}
			//printf("%X\n",opCodeDec);
			printf("%c\n",*((Instructions[instNo][opCol])+count));
		count--;
		}
	printf("%X\n",opCodeDec)		;
	return opCodeDec;
}
/*******************************************function converts string to number****************************/
u16 toHex(const char * s) {
 u16 result = 0;
 u16 c ;
 if (*s == '0' && 'x' == *(s+1)) { s+=2;
  while (*s) {
   result = result << 4;
   if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
   else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
   else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
   else break;
   ++s;
  }
 }
 return result;
}
/*******************************************evaluate argument function****************************/
u8 evaluateArg(u8 *instr, u8 Lindex,u8 instNo)
{	u8 InstrStatus = NOT_DECIDED;
	u8 NoofOperands = 0;
	u8 error[100] = "";
	u8 ins[] = "";
	char Operand1 [10] = ""; u16 op1;
	char Operand2 [10] = ""; u16 op2;
	u8 ret;u16 count;
	u32 Opcode;
	u8 opCodeLengthInBytes = 0;
	u8 FlagComment = 0;
	//calculate number of operands
	//printf("start");
	while(instr[Lindex] != '\n' && instr[Lindex] != '\0'&& FlagComment == 0)
	{
		switch(instr[Lindex])
		{
			case ' ':
					 while(instr[Lindex] == ' '){Lindex++;}
					 break;
			case ';':FlagComment = 1;
					 break;
			case ',':
					 Lindex++;
					 InstrStatus = ERROR;errorTypes(error,1,instr[Lindex],ins);
					 break;
					 
			case '\'':
					  Lindex = Lindex + 2;//printf("%c\n",instr[Lindex]);
					  if(instr[Lindex] != '\'')//if the argument is not 'single_character between single quotation' then generate an error eg: 'a' or 'b'
					  {InstrStatus = ERROR;errorTypes(error,2,instr[Lindex],ins);}/* argument is 'some_symbol' */
					  else
					  {	  Lindex--;//printf("%c",instr[Lindex]);
						  if(isalpha(instr[Lindex]) || isdigit(instr[Lindex])) 
						  {
							  NoofOperands++;
							  Lindex = Lindex + 2;
						  }
						  else
						  {InstrStatus = ERROR;errorTypes(error,3,(instr[Lindex]),ins);}
					  }
					  break;
					  
			default: 
					if(isdigit(instr[Lindex]) || instr[Lindex] == '$')
					{//MUST CHECK IF NUMBER FOR CORNER CASES
						count  = 0;
						while(instr[Lindex] != ',' && instr[Lindex] != '\n' && instr[Lindex] != ' '&&instr[Lindex] != ';' &&instr[Lindex] !='\0')
						{	
							if(NoofOperands == 0){Operand1[count] = instr[Lindex];}
							else if(NoofOperands == 1){Operand2[count] = instr[Lindex];}
							Lindex++;
							count++;//printf("d\n");
						}
						NoofOperands++;
						InstrStatus = OKAY;
					}
					else{InstrStatus = ERROR;errorTypes(error,3,(instr[Lindex]),ins);Lindex++;}
					break;
		}
		
	}
	//printf("check");
	ret = stricmp(Instructions[instNo][3],"Fixed");
	if (InstrStatus == ERROR)
	{
		printf("%s\n",error);exit(0);
	}
	else if(NoofOperands < atoi(Instructions[instNo][2]) && ret == 0)//check if number of operands not enough
	{
		printf("no of arg not enough");exit(0);
	}
	else if(NoofOperands > atoi(Instructions[instNo][2]))
	{
		printf("too many arguments");exit(0);
	}
	//check for error in operands
	op1 = toHex(Operand1);
	op2 = toHex(Operand2);
	if (NoofOperands == 1){op2 = op1;}
	/*********************NO ERRORS**********************/
/* 	printf("opcode 1 is: %s and its value is: %d\n",Operand1,op1);
	printf("opcode 2 is: %s and its value is: %d\n",Operand2,op2);
	printf("%d\n",instNo); */
	Opcode = generateOpcode(op1,op2,instNo,&opCodeLengthInBytes);
	AccumelateLine(Opcode,opCodeLengthInBytes);
}
/*******************************************evaluate instruction function****************************/

u8 evaluateInstr(u8 *instr)
{	u8 LineIndex = 0;
	u8 WordIndex = 0;
	u8 InstrStatus = NOT_DECIDED;
	u8 countInst;
	u8 ret;
	u8 error[100] = "ff";
    static u8 Word[50] = "";/*5 words, 10 u8s each*/
	/* if(!isalpha(instr[0]))
	{EndOfFile = 1;AccumelateLine(LineIndex,LineIndex);return;}
	else */
    while (instr[LineIndex] != '\n')
    { 
		switch(instr[LineIndex])
		{
			case ';': InstrStatus = OKAY;/*returns if comment found*/
					break;
			case ' ':countInst = 0;ret = 1; 
					
					 while(countInst < NOOFINSTRUCTIONS)
					{	
						ret = stricmp(Word, Instructions[countInst][0]);
						if (ret == 0)
						{
							InstrStatus = OKAY; 
							break;/*breaks if instr found*/
						}
						countInst++;	
					}
					
					if(ret != 0)
					{
						InstrStatus = ERROR;errorTypes(error,4,countInst,Word);
					}
					else
					{
						//WordIndex++;
						LineIndex++;
					}
					break;
			default:
					while(isalpha(instr[LineIndex]))//if alphabet found put itin instruction
					{
					Word[WordIndex] = instr[LineIndex];
					WordIndex++;
					LineIndex++;
					}
					break;
		}
		if(InstrStatus == ERROR || InstrStatus == OKAY)
			break;
    }

	if(InstrStatus == ERROR)//////////////////////////////////////////////TBR/////////////////////////////////////////////////////////////
	{
		printf("%s\n",error);return InstrStatus;
	}
	/***************remove all spaces***********/
	else
	{
		while(instr[LineIndex]==' ')
			LineIndex++;
		evaluateArg(instr,LineIndex,countInst);
	}
	
}
void main(int argc,char *argv[]) 
{	u8 *Line = 'c';
	u8 *FirstLine = ":020000020000FC\n";
	u8 *LastLine = ":00000001FF\n";
	FILE* AssemblyFile  = ReadOpen(argv[1]);
	HexFile = WriteOpen("Hex.txt");
	WriteOn(HexFile,FirstLine);
	
	while(Line !=NULL)
	{
		Line = ReadOn(AssemblyFile);/***reads and puts either line or null in u8* Line***///printf("%s",Line);
		if(Line == NULL){EndOfFile = 1;AccumelateLine(0,0);}/***if line == NULL the var EndOfFile is set o one for the function AccumelateLine to write the opcode in Hex.txt***/
		else if(Line[0] != ';')/***if line is not a comment***/
		{
			evaluateInstr(Line);
		}
	}
	WriteOn(HexFile,LastLine);
	RWClose(AssemblyFile);
	RWClose(HexFile);
	getch();
	  
}