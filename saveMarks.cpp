#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mysql/mysql.h"
#pragma comment(lib,"libmysql.lib")
#define FORM_SIZE 12

using namespace std;


MYSQL *connection, mysql;
MYSQL_ROW row, FSrow;
MYSQL_RES *res, *FSres;

struct info
	{
		char* name;
		char* value;
	};	

class Marks
{
protected:
	info arr[FORM_SIZE];
	char query[300];
	int count;
public:
	void readPost();
	void toBase();
	void Transcript();
};


void Marks :: readPost()
{
	char buff[200];
  short i, j = 0;
  long content_length=atol(getenv("CONTENT_LENGTH"));
  if(content_length > 0 && content_length < 1001)
    fread(buff,content_length,1,stdin);
  else return;
  buff[content_length] = 0;
  for(char* ptr=buff; *ptr != 0; ptr++){
    for(i = 0; ptr[i] != 0; i++)
      if(ptr[i] == '='){
        ptr[i] = 0;
        arr[j].name = new char[i+1];
        strcpy(arr[j].name, ptr);
        ptr += i+1;
        break;
      } //write name
      for(i = 0; true; i++)
        if(ptr[i] == '&' || ptr[i] == 0){
          ptr[i] = 0;
          arr[j].value = new char[i+1];
          strcpy(arr[j++].value, ptr);
          ptr += i;
          break;
        }//write value
        if(j == FORM_SIZE)return;
  }
}

void Marks :: Transcript()
{
	int len = strlen(arr[0].value);
	arr[0].value[len-1] = '\0';
}

void Marks :: toBase()
{
	sprintf(query,
			"UPDATE user_marks SET certificate = '%s', mark_1 = '%s', subject_2 = '%s', mark_2 = '%s',subject_3 = '%s', mark_3 = '%s', subject_4 = '%s',mark_4 = '%s' WHERE user_id = %s", arr[8].value, arr[1].value, arr[2].value, arr[3].value, arr[4].value, arr[5].value, arr[6].value, arr[7].value,arr[0].value);
	mysql_query(connection, query);
}


int main()
{
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
   Marks obj;
   obj.readPost();
   obj.Transcript();
   obj.toBase();
  return 0;
}