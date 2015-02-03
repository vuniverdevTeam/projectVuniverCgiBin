#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mysql/mysql.h"
#pragma comment(lib,"libmysql.lib")
#define FORM_SIZE 3
#define YEARS 10
using namespace std;



MYSQL *connection, mysql;
MYSQL_ROW row, FSrow;
MYSQL_RES *res, *FSres;

struct info
	{
		char* name;
		char* value;
	};	

class Favourite
{
protected:
	info arr[FORM_SIZE];
public:
	void read();
	void insertToDb();
	void Transcript();
	void readPost();
};

void Favourite :: readPost()
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

void Favourite :: Transcript()
{
	int len = strlen(arr[2].value);
	arr[2].value[len-1] = '\0';
}

void Favourite :: read()
{
  short i, j = 0;
  char buff[300];
  strcpy(buff, getenv("QUERY_STRING"));
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
        if(ptr[i] == '&') {
          ptr[i] = 0;
          arr[j].value = new char[i+1];
          strcpy(arr[j++].value, ptr);
          ptr += i;
          break;
        }
        else if(ptr[i] == 0) {
          arr[j].value = new char[i+1];
          strcpy(arr[j++].value, ptr);
          ptr += i;
          return;
        }//write value
        if(j == FORM_SIZE)return;
  }
}

void Favourite :: insertToDb()
{
	char str[200];
	sprintf(str,"SELECT s_id FROM specialities WHERE s_name =  '%s'",arr[1].value);
	mysql_query(connection, str);
	res = mysql_store_result(connection);
	row = mysql_fetch_row(res);
	int s_id = atoi(row[0]);
	mysql_free_result(res);
	sprintf(str,"SELECT f_id FROM faculties WHERE f_fullname = '%s'", arr[0].value);
	mysql_query(connection, str);
	res = mysql_store_result(connection);
	row = mysql_fetch_row(res);
	int f_id = atoi(row[0]);
	sprintf(str,"INSERT INTO user_facult_spec (user_id, s_id, f_id) VALUES('%s','%d','%d')",arr[2].value,s_id, f_id);
	mysql_query(connection,str);
}

int main()
{
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
   Favourite obj;
   obj.readPost();
   obj.Transcript();
   obj.insertToDb();
  return 0;
}
