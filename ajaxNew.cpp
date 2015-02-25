#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>
#define FORM_SIZE 13
#define YEARS 10
using namespace std;

enum ErrorsSub{
  SUB23 = 1,
  SUB24 = 2,
  SUB34 = 4,
  SUB234 = 8
};
enum ErrorsM{
  MARK1 = 1,
  MARK2 = 2,
  MARK3 = 4,
  MARK4 = 8,
  MARK5 = 16
};

class n_v{
public:
  char *name;
  char * value;
  //  ~n_v(){delete[] name, delete[] value;}
};

class entr{
public:
  n_v arr[FORM_SIZE];
public:
  n_v *operator[](short index){return (this->arr)+index;}
  const char* operator()(char *str){
    for(short i = 0; i < FORM_SIZE; i++)
      if(strcmp(arr[i].name, str) == 0)
        return arr[i].value;
  }
};

MYSQL *connection, mysql;
MYSQL_ROW row;
MYSQL_RES *res;

class Template
{
protected:
  char buff[1001], *ptr;
  ifstream fin;
  entr en;
  short errMark, errSub;
  void Read();
  void ReadPost();
public:
  void Controller(char* file);
  int View();
};

void Template :: Read()
{
  short i, j = 0;
  strcpy(buff, getenv("QUERY_STRING"));

  for(ptr=buff; *ptr != 0; ptr++){
    for(i = 0; ptr[i] != 0; i++)
      if(ptr[i] == '='){
        ptr[i] = 0;
        en.arr[j].name = new char[i+1];
        strcpy(en.arr[j].name, ptr);
        ptr += i+1;
        break;
      } //write name
      for(i = 0; true; i++)
        if(ptr[i] == '&' || ptr[i] == 0){
          ptr[i] = 0;
          en.arr[j].value = new char[i+1];
          strcpy(en.arr[j++].value, ptr);
          ptr += i;
          break;
        }//write value
        if(j == FORM_SIZE)return;
  }
}
void Template :: ReadPost()
{
  short i, j = 0;
  long content_length=atol(getenv("CONTENT_LENGTH"));
  if(content_length > 0 && content_length < 1001)
    fread(buff, content_length, 1, stdin);
  else return;
	buff[content_length] = 0;
  for(ptr=buff; *ptr != 0; ptr++){
    for(i = 0; ptr[i] != 0; i++)
      if(ptr[i] == '='){
        ptr[i] = 0;
        en.arr[j].name = new char[i+1];
        strcpy(en.arr[j].name, ptr);
        ptr += i+1;
        break;
      } //write name
      for(i = 0; true; i++)
        if(ptr[i] == '&' || ptr[i] == 0){
          ptr[i] = 0;
          en.arr[j].value = new char[i+1];
          strcpy(en.arr[j++].value, ptr);
          ptr += i;
          break;
        }//write value
        if(j == FORM_SIZE)return;
  }
}
int Template :: View()
{
  while(!fin.eof())
  {
    fin.getline(buff, 1000);
    for(ptr = buff; *ptr; ptr++)
      if(*ptr == '~'){
        *ptr = 0;
        ptr++;
        cout<<buff;
        return -1;
      }
      cout<<buff<<endl;
  }
  return 0;
}
void Template :: Controller(char* file)
{
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
  int i, exit = 1;
  char condition[20]={0}, table[70]={0}, what[50]={0};
  char str[10001];
  if(strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
  	Read();
  else ReadPost();
        if(strcmp(en.arr[0].name, "sub") == 0){
          strcat(table, "subjects");
          strcat(what, "sub_id, sub_name");
        }
        else if(strcmp(en.arr[0].name, "reg") == 0){
          strcat(table, "region");
          strcat(what, "reg_id, reg_name");
        }
        else if(strcmp(en.arr[0].name, "city") == 0){
          strcpy(table, "city");
          strcpy(what, "city_id, city_name");
          sprintf(condition, " WHERE reg_id=%s", en.arr[0].value);
        }
        else if(strcmp(en.arr[0].name, "univer") == 0){
          strcat(table, "university");
          strcat(what, "u_id, u_fullname");
          strcat(condition, " WHERE city_id=");
					strcat(condition, en.arr[0].value);
        }
        else if(strcmp(en.arr[0].name, "fac") == 0){
          strcat(table, "faculties");
          strcat(what, "f_id, f_fullname");
          sprintf(condition, " WHERE u_id=%s ORDER BY f_fullname", en.arr[0].value);
        }
        else if(strcmp(en.arr[0].name, "spec") == 0){
          strcat(table, "specialities JOIN coeff_abit ON specialities.s_id=coeff_abit.s_id");
          strcat(what, "specialities.s_id, specialities.s_name");
					if(*en.arr[0].value != '0' && en.arr[0].value != 0 && *en.arr[0].value != 0)
    	      sprintf(condition, " WHERE f_id=%s ORDER BY specialities.s_name", en.arr[0].value);
					else	sprintf(condition, " ORDER BY specialities.s_name");
        }
        sprintf(str, "SELECT %s FROM %s%s", what, table, condition);
    mysql_query(connection, str);
    res = mysql_store_result(connection);
    cout<<"[";
		if(row = mysql_fetch_row(res)){
      cout<<"'"<<row[0]<<"',";
      cout<<"'"<<row[1]<<"'";
		}
    while(row = mysql_fetch_row(res)){
      cout<<",'"<<row[0]<<"',";
      cout<<"'"<<row[1]<<"'";
    }
    cout<<"]";
		mysql_free_result(res);
}

int main()
{
  Template obj;
  mysql.reconnect = true;
  mysql_init(&mysql);
  int n;
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  obj.Controller("../1.html");
  return 0;
}
