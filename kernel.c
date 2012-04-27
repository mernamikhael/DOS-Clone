/*Cam Cook*/
void printString(char*);
void readString(char*);
void readSector(char*,int);
int mod(int,int);
int div(int,int);
void handleInterrupt21(int,int,int,int);
void readFile(char*,char*);
int stringCompare(char*,char*,int);
void debug4(char*,char*,char*,char*);
void debug3(char*,char*,char*);
void debug2(char*,char*);
void debug(char*);
void executeProgram(char*);
void terminate();
void writeSector(char*,int);
void deleteFile(char*);
void writeFile(char*,char*,int);
int isChar(char*);
void printDir();
void copyFile(char*,char*);
void handleTimerInterrupt(int,int);
void killProcess(int);
int active[8];
int pointers[8];
int currentProcess;
int main()
{
        char shellArr [6];
	int i;
	makeInterrupt21();
	currentProcess =0;
	for(i=0;i<8;i++)
	{
		active[i]=0;
		pointers[i]= 0xff00;
	}
	makeTimerInterrupt();
        enableInterrupts();
	shellArr[0]='s';
        shellArr[1]='h';
        shellArr[2]='e';
        shellArr[3]='l';
        shellArr[4]='l';
	shellArr[5]='\0';
	interrupt(0x21,4,shellArr,0x2000,0);
	while(1);			/*hang up*/
	return 0;
}

void printString(char* toPrint)
{
	int i;
	for(i=0;toPrint[i] != '\0';i++)
	{
		interrupt(0x10, 0xe*256+toPrint[i], 0, 0, 0);
	}
}

void readString(char* line1)
{
	int line_ptr=0;
	char returned;
        char printchar[2];
	while(returned != 0xd)
	{
		returned =interrupt(0x16,0,0,0,0);
		if(returned == 0xd)
		{
			break;
		}
		if(returned==0x8)
		{
			line_ptr--;
			line1[line_ptr]=' ';

		}
		line1[line_ptr]=returned;
                printchar[0]=returned;
		printchar[1]='\0';
		/*printString(printchar);*/
		interrupt(0x10, 0xe*256+returned, 0, 0, 0);
		line_ptr++;
	}
        line1[line_ptr]='\n';
	interrupt(0x10, 0xe*256+line1[line_ptr], 0, 0, 0);
        line_ptr++;
	line1[line_ptr]='\r';
	interrupt(0x10,0xe*256+line1[line_ptr],0,0,0);
	line_ptr++;
	line1[line_ptr]='\0';
}


void readSector(char* buffer, int sector)
{
	int al;
	int ah;
	int bl;
	int bh;
	int cl;
	int ch;
	int dl;
	int dh;
	int ax;
	int bx;
	int cx;
	int dx;
	ah =2;/*read*/
	al =1;/*number of sectors to read*/
	dl =0;/*device number, floppy 0*/
	cl = mod(sector,18)+1; /*relative sector number*/
	dh = mod(div(sector,18),2);/*head number*/
	ch = div(sector,36);/*track*/
	ax= ah*256+al;
	bx= buffer; /*address where data should be stored*/
	cx= ch*256+cl;
	dx= dh*256+dl;
	interrupt(0x13,ax,bx,cx,dx);
	/*printString(buffer);*/
}

int mod(int a,int b)
{
	while(a>=b)
	{
		a=a-b;
	}
	return a;
}

int div (int a, int b)
{
	int quotient =0;
	while((quotient*b) <= a)
	{
		quotient=quotient+1;
	}
	return quotient-1;
}
void readFile(char* fileName, char* buffer)
{
	/*
	1. Load the directory sector into a 512 byte character array using readSector
	2. Go through the directory trying to match the file name.  If you do not find it, return.
	3. Using the sector numbers in the directory, load the file, sector by sector, into the buffer array.
	You should add 512 to the buffer address every time you call readSector
	4. Return
	*/
	char directory[512];
	char indexedFile[7];
	int fileStart;
	int fileEnd;
	int i;
	int j;
	int k;
	int l;
	int counter;
	int notFound=1;
	char temp[2];

	readSector(directory,2);
	for(i=0;i<16;i++)
	{
		fileStart =i*32;
		fileEnd = fileStart+6;
		k=0;
		for(j=fileStart;j<fileEnd;j++)
		{
			indexedFile[k]=directory[j];
			k++;
		}
		indexedFile[6]='\0';
		/*debug(directory);*/
		/*debug(indexedFile);*/
		if(stringCompare(indexedFile,fileName,6) /*indexedFile,fileName,6)*/)
		{
			notFound =0;
			/*get sectors*/
			for(l=fileEnd;directory[l] !=0x0;l++)
			{
				readSector(buffer,directory[l]);
				buffer= buffer +512;
			}
		}
	}
	if(notFound)
	{
		buffer[0]='N';
		buffer[1]='O';
		buffer[2]='T';
		buffer[3]=' ';
		buffer[4]='F';
		buffer[5]='O';
		buffer[6]='U';
		buffer[7]='N';
		buffer[8]='D';
		buffer[9]='\r';
		buffer[10]='\n';
		buffer[11]='\0';
	}
}
int stringCompare(char* indexed, char* fileName,int length)
{
	int i;
	int isSame=0;
	for(i=0;i<length;i++)
	{
		if(indexed[i]==fileName[i])
		{
			isSame=1;
		}
		else
		{
			isSame=0;
			break;
		}
	}
	return isSame;
}
void debug4(char* first, char* second, char* third, char* fourth)
{
	printString("FIRST:\0");
	printString(first);
	printString("\r\n\0");
	if(second[0]!='\0')
	{
		printString("SECOND:\0");
        	printString(second);
	        printString("\r\n\0");

	}
	if(third[0] != '\0')
	{
		printString("THIRD:\0");
	        printString(third);
	        printString("\r\n\0");

	}
	if(fourth[0] != '\0')
	{
		printString("FOURTH:\0");
	        printString(fourth);
	        printString("\r\n\0");

	}
}
void debug3(char* first,char* second, char* third)
{
	debug4(first,second,third,"\0");
}
void debug2(char* first, char* second)
{
	debug3(first,second,"\0");
}
void debug(char* first)
{
	debug2(first,"\0");
}

void executeProgram(char* name)
{
	/*
		1.Load program into buffer
		2. Transfer program into the bottom of the segment
		3. Set up segment registers
		4. Jumping to the program
	*/
	char buffer[13312];
	int i;
	int segment =0;
	setKernelDataSegment();
	readFile(name,buffer);
	i=0;
        while(i<8)
        {

                if(active[i] ==0)
                {
                        segment =i;
                        segment++;
                        segment++;
                        segment = segment *0x1000;
                        active[i]=1;
			currentProcess=i;
                        break;
                }
                i++;
        }
	restoreDataSegment();
	for(i=0;i<13312;i=i+1)
	{
		putInMemory(segment, i, buffer[i]);
	}

	initializeProgram(segment);
}
void terminate()
{
	setKernelDataSegment();
	/*char shellArr [6];
	shellArr[0]='s';
	shellArr[1]='h';
	shellArr[2]='e';
	shellArr[3]='l';
	shellArr[4]='l';
	shellArr[5]='\0';
	interrupt(0x21,4,shellArr,0x2000,0);*/
	active[currentProcess]=0;
	while(1);
}
void writeSector(char* buffer,int sector)
{
	int al;
        int ah;
        int bl;
        int bh;
        int cl;
        int ch;
        int dl;
        int dh;
        int ax;
        int bx;
        int cx;
        int dx;
        ah =3;/*read*/
        al =1;/*number of sectors to read*/
        dl =0;/*device number, floppy 0*/
        cl = mod(sector,18)+1; /*relative sector number*/
        dh = mod(div(sector,18),2);/*head number*/
        ch = div(sector,36);/*track*/
        ax= ah*256+al;
        bx= buffer; /*address where data should be stored*/
        cx= ch*256+cl;
        dx= dh*256+dl;
        interrupt(0x13,ax,bx,cx,dx);
}
void deleteFile(char* fileName)
{
	char directory[512];
	char diskmap[512];
	char indexedFile[7];
	int fileStart;
	int fileEnd;
	int i;
	int j;
	int k;
	int l;
	int counter;
	int notFound=1;
	char temp[2];
	readSector(directory,2);
	readSector(diskmap,1);

	for(i=0;i<16;i++)
	{
		fileStart =i*32;
		fileEnd = fileStart+6;
		k=0;
		for(j=fileStart;j<fileEnd;j++)
		{
			indexedFile[k]=directory[j];
			k++;
		}
		indexedFile[6]='\0';

		if(stringCompare(indexedFile,fileName,6))
		{
			directory[fileStart]=0x0;
			for(l=fileEnd;directory[l] !=0x0;l++)
			{
				int diskMapToChange = directory[l];
				diskmap[diskMapToChange] = 0x00;
			}
		}
	}
	writeSector(directory,2);
	writeSector(diskmap,1);
}
int isChar(char* test)
{
	if((test >= 32) && (test <= 126))
	{
		return 1;
	}
	return 0;
}
void writeFile(char* fileName,char* buffer, int numOfSectors)
{
	char diskmap[512];
	char directory[512];
	char holder[512];
	int i;
	int k;
	int l;
	int m;
	int p;
	int sectorsAssigned = 0;
	int n=0;
	int fileStart;
	int fileEnd;
	/*interrupt(0x10,0xe*256+buffer[0],0,0,0);*/

	readSector(diskmap,1);
	readSector(directory,2);

	k=0;
	for(i=0;i<16;i++)
	{
		fileStart=i*32;
		fileEnd = fileStart+6;
		if(directory[fileStart] == 0x00)
		{
			/*empty dir found. assigning it a name*/
			for(k=0;k<6;k++)
			{
				if(isChar(fileName[k]))
				{
					directory[fileStart+k]=fileName[k];
				}
				else
				{
					directory[fileStart+k]=0x00;
				}
			}
			/*find free sectors*/
				/*go through diskmap, find a free sector*/
				p=fileEnd;
				for(l=0;l<512 && sectorsAssigned <numOfSectors;l++)
				{
					if(diskmap[l]!= 0xFF)
					{
						n=0;
						diskmap[l] = 0xFF;
						/*write the nth 512 to the sector*/
						for(m=sectorsAssigned*512;m<512+(sectorsAssigned*512);m++)
						{
							holder[n]=buffer[m];
							n++;
						}
						sectorsAssigned++;
						/*interrupt(0x10,0xe*256+holder[0],0,0,0);*/
						/*interrupt(0x10,0xe*256+'3',0,0,0);*/
						writeSector(holder,l);
			                        m=0;
						directory[p]=l;
						p++;
					}
				}
		break;
		}
	}
	writeSector(directory,2);
	writeSector(diskmap,1);
}
void printDir()
{
	/*
	goes through the file system and prints 
	*/
	int i;
	int k;
	int j;
	int filestart;
	int fileend;
	char indexedFile[9];
	char directory[512];

	readSector(directory,2);

	for(i=0;i<16;i++)
	{
		filestart=i*32;
		fileend=filestart+6;
		k=0;
		for(j=1;j<9;j++)
		{
			indexedFile[j]=' ';
		}
		for(filestart;filestart <fileend; filestart++)
		{
			if(directory[filestart]!= 0x00)
			{
				indexedFile[k]=directory[filestart];
			}
			k++;
		}
		indexedFile[6]='\r';
		indexedFile[7]='\n';
		indexedFile[8]='\0';
		if(isChar(indexedFile[0]))
		{
			printString(indexedFile);
		}
		indexedFile[0]=0x00;
	}
}

void copyFile(char* filename1,char* filename2)
{
	char buffer[13312];
	char directory[512];
	int numOfSectors=0;
	int i;
	int j;
	int k;
	int filestart;
	int fileend;
	char indexedfile[6];
	/*go through the directory and count the number of sectors*/
	/*adjust file size*/
	readSector(directory,2);

	for(i=0;i<16;i++)
	{
		filestart= i*32;
		fileend = filestart+6;
		for(j=0;j<6;j++)
		{
			indexedfile[j]= directory[filestart+j];
		}
		if(stringCompare(indexedfile,filename1,6))
		{
			/*find number of sectors*/
			for(k=fileend;directory[k]!= 0x00;k++)
			{
				numOfSectors++;
			}
		}
	}
	/*interrupt(0x10,0xe*256+numOfSectors+0x30,0,0,0);*/
	readFile(filename1,buffer);
	writeFile(filename2,buffer,numOfSectors);

}
void killProcess(int pid)
{
	active[pid]=0;
}
void handleTimerInterrupt(int segment,int sp)
{
	/*TODO*/
	int i;
	pointers[currentProcess]=sp;
	for(i=currentPorcess+1;i<8;i++)
	{
		if(active[i]==1)
		{
			
		}
	}
	returnFromTimer(segment,sp);
}
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	if(ax==0)
	{
		printString(bx);
	}
	else if (ax==1)
	{
		readString(bx);
	}
	else if (ax ==2)
	{
		readSector(bx,cx);
	}
	else if (ax ==3)
	{
		readFile(bx,cx);
	}
	else if (ax ==4)
	{
		executeProgram(bx);
	}
	else if (ax ==5)
	{
		terminate();
	}
	else if (ax==6)
	{
		writeSector(bx,cx);
	}
	else if (ax==7)
	{
		deleteFile(bx);
	}
	else if (ax ==8)
	{
		writeFile(bx,cx,dx);
	}
	else if (ax ==9)
	{
		copyFile(bx,cx);
	}
	else if (ax ==10)
	{
		killProcess(bx);
	}
	else if (ax ==11)
	{
		printDir();
	}
	else
	{
		printString("Error: No such interrupt call\r\n\0");
	}
}
