#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

char** loadBuff;

int loadFromList()
{
	
	int count;
	ifstream fout("list.bin", ios_base :: binary);
	if(!fout.is_open())
	{
		cout << "Cannot open the file" << endl;
	}
	fout.read((char*)&count,sizeof(int));
	if(count == 0)
	{
		return 0;
	}
	loadBuff = new char*[count];
	int len;
	for(int i = 0; i<count; i++)
	{
		fout.read((char*)&len,sizeof(int));
		loadBuff[i] = new char[len];
		fout.read(loadBuff[i],len);
	}
	fout.close();
	return count;
}

bool isSql(char* str)
{
	int i = strlen(str)-4;
	if(str[i] == '.' && str[i+1] == 's' && str[i+2] == 'q' && str[i+3] == 'l')
	{
		return true;
	}
	return false;
}

int main()
{	
	cout << "Content-type: text/html\r\n\r\n";
	int count = loadFromList();
	ifstream html("../project ISM/dump.html");
	char ch = '1';
	while(!html.eof())
	{
		if(ch == '~')
		{
			for(int i = 0; i<count; i++)
			{	
				if(isSql(loadBuff[i]))
				{
					cout << "<option>" << loadBuff[i] << "<\option>";
				}
			}
		}
		html.read(&ch,sizeof(char));
		cout << ch;
	}
}