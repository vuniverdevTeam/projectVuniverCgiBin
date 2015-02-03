#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mysql/mysql.h"
#pragma comment(lib,"libmysql.lib")
#define FORM_SIZE 1

using namespace std;

MYSQL *connection, mysql;
MYSQL_ROW row, FSrow;
MYSQL_RES *res, *FSres;

struct info
	{
		char* name;
		char* value;
	};	

class Response
{
protected:
	info arr[FORM_SIZE];
	char query[200];
public:
	void readPost();
	void answer();
	void Transcript();

};

void Response :: Transcript()
{
	int len = strlen(arr[0].value);
	arr[0].value[len-1] = '\0';
}

void Response ::  answer()
{
	sprintf(query,"SELECT * FROM user_marks WHERE user_id = %s",arr[0].value);
	mysql_query(connection,query);
	res = mysql_store_result(connection);
	row = mysql_fetch_row(res);
	for(short i = 1; i<9; i++)
	{
		cout << row[i] << '~';
	}
}

void Response :: readPost()
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


int main()
{
	mysql_init(&mysql);
	connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
	mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
	cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
	Response obj;
	obj.readPost();
	obj.Transcript();
	obj.answer();
	return 0;
}