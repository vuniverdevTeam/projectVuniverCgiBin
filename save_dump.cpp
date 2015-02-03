#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;


char buff[400];
char* name;
char* value;

class Dump
{
protected:
	char buff[200];
	char** loadBuff;
	char* value;
	char* name;
	int count;
public:
	void save();
	void read();
	void addToList();
	void loadFromList();
	bool isSql();
};

bool Dump ::  isSql()
{
	int i = strlen(value)-4;
	if(value[i] == '.' && value[i+1] == 's' && value[i+2] == 'q' && value[i+3] == 'l')
	{
		return true;
	}
	return false;
}

void Dump :: read()
{
  short i, j = 0;
  strcpy(buff, getenv("QUERY_STRING"));
    char* ptr=buff;
    for(i = 0; ptr[i] != 0; i++)
      if(ptr[i] == '='){
        ptr[i] = 0;
        name = new char[i+1];
        strcpy(name, ptr);
        ptr += i+1;
        break;
      } //write name
    for(i = 0; true; i++)
      if(ptr[i] == '&' || ptr[i] == 0){
        ptr[i] = 0;
        value = new char[i+1];
        strcpy(value, ptr);
        ptr += i;
        break;
      }//write value
}

void Dump :: save()
{
	char* require = new char[200];
	require  = (char*)"mysqldump -u vuniver -h 185.25.117.161 -p5S1CQhvO  vuniver facult_spec > ";
	this->read();
	if(isSql())
	{
		char* NEW  = new char[300];
		for(int i = 0; i<strlen(require); i++)
		{
			NEW[i] = require[i];
		}
		for(int i = strlen(NEW), j = 0; j<strlen(value); i++,j++)
		{
			NEW[i] = value[j];
 		}
		system(NEW);	
	}
}

void Dump :: addToList()
{
	this->loadFromList();
	if(count > 100)
	{
		cout << "Сервер переповнений,будь ласка, видаліть непотрібні файли" << endl;
	}
	count++;
	ofstream fin("list.bin", ios_base :: binary);
	if(!fin.is_open())
	{
		cout << "Cannot open the file" << endl;
	}
	fin.write((char*)&count, sizeof(int));
	for(int i = 0; i<count-1; i++)
	{
		int len = strlen(loadBuff[i])+1;
		fin.write((char*)&len,sizeof(int));
		fin.write(loadBuff[i],sizeof(char)*len);
	}
	if(isSql())
	{
		int len = strlen(value)+1;
		fin.write((char*)&len,sizeof(len));
		fin.write(value,sizeof(char)*len);
		fin.close();
	}
	else
	{
		cout << "<br/>Некорректний формат.Файл повинен мати розширення '.sql'" <<endl;
	}
}

void Dump :: loadFromList()
{
	ifstream fout("list.bin", ios_base :: binary);
	if(!fout.is_open())
	{
		cout << "Cannot open the file" << endl;
	}
	fout.read((char*)&count,sizeof(int));
	if(count == 0)
	{
		return;
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
}


int main()
{
	
	Dump table;
	table.save();
	table.addToList();
	cout<<"Location: http://alex.inet-tech.org.ua/project ISM/dump.html\r\n\r\n"<<endl;
	return 0;
}
























