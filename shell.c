/*Cam Cook*/
int stringCompare(char*, char*,int);
int isChar(char*);
int main()
{
        char commandName[80];
	char line[80];
        char buffer[13312];
	char arg1[7];
	char arg2[7];
	int i;
	int j;
	while(1)
	{
		interrupt(0x21,0,"SHELL>\0",0,0);
		interrupt(0x21,1,commandName,0,0);

		if(stringCompare(commandName,"type",4))
		{
			interrupt(0x21, 3, commandName+5, buffer, 0);
			interrupt(0x21,0,buffer,0,0);
		}
		else if (stringCompare(commandName,"execute",7))
		{
			interrupt(0x21,4,commandName+8,0x2000,0);
		}
		else if (stringCompare(commandName,"delete",6))
		{
			interrupt(0x21,7,commandName+7,0,0);
		}
		else if (stringCompare(commandName,"dir",3))
		{
			interrupt(0x21,11,0,0,0);
		}
		else if (stringCompare(commandName,"copy",4))
		{
			/*create args*/
			for(i=0;i<6;i++)
			{
				arg1[i]=commandName[i+5];
				arg2[i]=commandName[i+12];
			}
			arg1[6]='\0';
			arg2[6]='\0';
			interrupt(0x21,9,arg1,arg2,0);
			/*interrupt(0x21,9,"messag\0","messa1\0",0);
			/*interrupt(0x21,9,arg1,arg2,0);*/
		}
		else if (stringCompare(commandName,"create",5))
		{
			j=0;
                        interrupt(0x21,1,line,0,0);
			/*line[0]='s';*/
			while(isChar(line[0]))
			{
				
				interrupt(0x21,0,line,0,0);
				for(i=0;i<80 && isChar(line[i]);i++)
				{
					buffer[i+j]=line[i];
				}
				buffer[i]='\r';
				buffer[i+1]='\n';
				j=i+2;
				 interrupt(0x21,1,line,0,0);
				interrupt(0x21,1,line,0,0);
			}
			interrupt(0x21,0,buffer,0,0);
		}
		else if (stringCompare(commandName,"kill",4))
		{
			interrupt(0x21,11,commandName+6-'0',0,0);
		}
		else
		{
			interrupt(0x21,0,"Bad Command!\r\n\0",0,0);
		}
	}
	return 1;
}
int stringCompare(char* indexed, char* fileName,int length)
{
	int i;
	for(i=0;i<length;i++)
	{
		if(indexed[i]!=fileName[i])
		{
			return 0;
		}
	}
	return 1;
}
int isChar(char* test)
{
        if((test >= 32) && (test <= 126))
        {
                return 1;
        }
        return 0;
}

