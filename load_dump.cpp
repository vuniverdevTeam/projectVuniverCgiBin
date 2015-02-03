#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

char buff[200];
char* name;
char* value;

void read()
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

int main()
{
	read();
	cout << "Content-type: text/html\r\n\r\n";
	char* require = new char[100];
	require = "mysql vuniver -uvuniver -h185.25.117.161 -p5S1CQhvO <";
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
	cout << "База була оновлена"; 
}
