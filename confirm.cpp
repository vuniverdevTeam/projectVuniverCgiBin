#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>
#define FORM_SIZE 2
#define YEARS 10
#define MAX_RAND 18446744073709551615
#define SESSION_LENGHT 600
using namespace std;

MYSQL *connection, mysql;
MYSQL_ROW row;
MYSQL_RES *res;

class n_v{
public:
  char *name;
  char * value;
  //  ~n_v(){delete[] name, delete[] value;}
};

class entr{
public:
  n_v arr[FORM_SIZE];
  short n;
public:
  //entr(){arr = new n_v[FORM_SIZE];}
  //~entr(){delete[] arr;}
  n_v *operator[](short index){return (this->arr)+index;}
  const char* operator()(char *str){
    for(short i = 0; i < n; i++)
      if(strcmp(arr[i].name, str) == 0)
        return arr[i].value;
    return 0;
  }
};

unsigned long long int rand64(){
  unsigned long long int r64 = 0;
  r64=rand();
  r64=r64 << 16;
  r64+=rand();
  r64=r64 << 15;
  r64+=rand();
  r64=r64 << 15;
  r64+=rand();
  return r64;
}



unsigned long long generateID(char *str){
  srand(time(0));
  srand(rand64()-atoi(str));
  return rand64()%MAX_RAND;
}

class Template
{
private:
    char buff[1001], *ptr;
protected:
  entr en;
  void Read();
  void ReadPost();
public:
  void Controller();
};

void Template :: Read()
{
  short i;
en.n = 0;
  strcpy(buff, getenv("QUERY_STRING"));
  //cout<<"1:"<<buff<<"<br/>"<<endl;

  for(ptr=buff; *ptr != 0; ptr++){
    for(i = 0; ptr[i] != 0; i++)
      if(ptr[i] == '='){
        ptr[i] = 0;
        en.arr[en.n].name = new char[i+1];
        strcpy(en.arr[en.n].name, ptr);
        ptr += i+1;
        break;
      } //write name
      for(i = 0; true; i++)
        if(ptr[i] == '&') {
          ptr[i] = 0;
          en.arr[en.n].value = new char[i+1];
          strcpy(en.arr[en.n++].value, ptr);
          ptr += i;
          break;
        }
        else if(ptr[i] == 0) {
          en.arr[en.n].value = new char[i+1];
          strcpy(en.arr[en.n++].value, ptr);
          ptr += i;
          return;
        }//write value
        if(en.n == FORM_SIZE)return;
  }
}
void Template :: ReadPost()
{
  short i;
  en.n=0;
  long content_length=atol(getenv("CONTENT_LENGTH"));
  if(content_length > 0 && content_length < 1001)
    fread(buff,content_length,1,stdin);
  else return;
  buff[content_length] = 0;
  //cout<<buff;
  for(ptr=buff; *ptr != 0; ptr++){
    for(i = 0; true; i++)
      if(ptr[i] == '=' || ptr[i] == 0){
        ptr[i] = 0;
        en.arr[en.n].name = new char[i+1];
        strcpy(en.arr[en.n].name, ptr);
        ptr += i+1;
        break;
      } //write name
      if(ptr>=buff+content_length) return;
      for(i = 0; true; i++)
        if(ptr[i] == '&' || ptr[i] == 0){
          ptr[i] = 0;
          en.arr[en.n].value = new char[i+1];
          strcpy(en.arr[en.n++].value, ptr);
          ptr += i;
          break;
        }//write value
        if(en.n == FORM_SIZE || ptr>=buff+content_length)return;
  }
}

void Template :: Controller()
{
  if(strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
    Read();
  else ReadPost();
  //cout <<"Content-Type: text/html\r\n\r\n";
  if(en("confirm") && en("confirm")[0] && en("confirm")[0] != '0') {
    sprintf(buff, "SELECT session_end, user_id, is_active FROM users WHERE session_id='%s'", en("confirm"));
    mysql_query(connection, buff);
    res = mysql_store_result(connection);
    row = mysql_fetch_row( res );
		if(row[2][0] == '0') {
			if(time(0) < atoi(row[0])){
					unsigned long long ID = generateID(row[1]);
					sprintf(buff, "UPDATE users SET is_active=1, session_id='%llu', session_end='%llu' WHERE user_id='%s'", (unsigned long long)ID, (unsigned long long)time(0) + SESSION_LENGHT, row[1]);
					//mysql_free_result(res);
					mysql_query(connection, buff);
					cout<<"auth="<<ID;
					return;
			}
			cout<<"Late";
		}
  }
  cout<<"Wrong!";
}

int main(){
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
  Template obj;
  mysql.reconnect = true;
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  obj.Controller();
}
