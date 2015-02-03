#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>

using namespace std;

#define YEARS 10
#define FORM_SIZE 8

MYSQL *connection, mysql;
MYSQL_ROW row;
MYSQL_RES *res;

class n_v{
public:
  char *name;
  char * value;
};

class entr{
public:
  n_v arr[FORM_SIZE];
public:
  n_v *operator[](short index){return (this->arr)+index;}
  const char* operator()(char *str) {
    for(short i = 0; i < FORM_SIZE; i++) {
      if(strcmp(arr[i].name, str) == 0)
        return arr[i].value;
  }
  return 0;
  }
};

double uniformDistribution(double start, double end)
{
  return start + rand()%(int)(end - start);
}

double normalDistribution(double Mm, double Dm)
{
  double value = 0;
  int	i;
  const int number = 15;
  double start = Mm - Dm;
  int border = Dm * 2;

  for (i = 0; i < number; ++i) {
    value += start + rand()%border;
  }
  value /= number;
  return value;
}

class Template
{
protected:
  char buff[1001], *ptr;
  entr en;
  void Read();
  void ReadPost();
public:
  int View();
  void Controller();
  void Model1();
  void Model2();
  friend class SQL;
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
      } 
      for(i = 0; true; i++)
        if(ptr[i] == '&') {
          ptr[i] = 0;
          en.arr[j].value = new char[i+1];
          strcpy(en.arr[j++].value, ptr);
          ptr += i;
          break;
        }
        else if(ptr[i] == 0) {
          en.arr[j].value = new char[i+1];
          strcpy(en.arr[j++].value, ptr);
          ptr += i;
          return;
        }
        if(j == FORM_SIZE)return;
  }
}
void Template :: ReadPost()
{
  short i, j = 0;
  long content_length=atol(getenv("CONTENT_LENGTH"));
  if(content_length > 0 && content_length < 1001)
    fread(buff,content_length,1,stdin);
  else return;
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

void Template::Model1 () 
{ 
  double x, y, M1, D1, M2, D2, M3, D3, M4, D4;
  bool check = true;
  M1 = atof(en("M1"));
  D1 = atof(en("D1"));
  M2 = atof(en("M2"));
  D2 = atof(en("D2"));
  M3 = atof(en("M3"));
  D3 = atof(en("D3"));
  M4 = atof(en("M4"));
  D4 = atof(en("D4"));
  if (M1 < 100) {
	cout<<"1";
	check = false;
  }
  if (M1 > 200) {
	cout<<"2";
	check = false;
  }
  if (D1 < 0) {
	cout<<"3";
	check = false;
  }
  if (D1 > 100) {
	cout<<"4";
	check = false;
  }
  if (M2 < 0) {
	cout<<"5";
	check = false;
  }
  if (M2 > 100) {
	cout<<"6";
	check = false;
  }
  if (M3 < 100) {
	cout<<"7";
	check = false;
  }
  if (M3 > 200) {
	cout<<"8";
	check = false;
  }
  if (D3 < 0) {
	cout<<"9";
	check = false;
  }
  if (D3 > 100) {
	cout<<"a";
	check = false;
  }
  if (M4 < 0) {
	cout<<"b";
	check = false;
  }
  if (M4 > 100) {
	cout<<"c";
	check = false;
  }
  if (check == false) return;
  double Mbudg = normalDistribution(M1, D1);
  double Dbudg = normalDistribution(M2, D2);
  double Mcontr = normalDistribution(M3, D3);
  double Dcontr = normalDistribution(M4, D4);
  mysql_query(connection, "SELECT f_id, s_id FROM facult_spec WHERE year=2010");
  res = mysql_store_result(connection);
  while(row = mysql_fetch_row( res )){
    for (int i = 0; i < YEARS; ++i) {
      x = normalDistribution(Mcontr, Dcontr);
      y = normalDistribution(Mbudg, Dbudg);
      sprintf(buff, "UPDATE facult_spec SET avg_contract_mark=%lf, avg_budget_mark=%lf WHERE f_id=%s AND s_id=%s AND year=%d", x, y, row[0], row[1], i+2004);
      mysql_query(connection, buff);
    }
  }
  cout<<"База перегенерована з нормальним розподілом:<br/>";
  cout<<"Бюджет: <br/>"<<"M = "<<Mbudg<<","<<" D = "<<Dbudg;
  cout<<"<br/>Контракт:<br/>"<<"M = "<<Mcontr<<","<<" D = "<<Dcontr;
  //cout<<"<input type=\"button\" value=\"Назад\" onclick=\"history.back()\">"<<endl;
  mysql_free_result(res);
}

void Template:: Model2()
{
  double x, y, left1, right1, left2, right2;
  bool check = true;
  left1 = atof(en("L1"));
  right1 = atof(en("R1"));
  left2 = atof(en("L2"));
  right2 = atof(en("R2"));
  if (left1 < 100) {
	cout<<"0";
	check = false;
  }
  if (left1 > 200) {
	cout<<"1";
	check = false;
  }
  if (right1 < 100) {
	cout<<"2";
	check = false;
  }
  if (right1 > 200) {
	cout<<"3";
	check = false;
  }
  if (left2 < 100) {
	cout<<"4";
	check = false;
  }
  if (left2 > 200) {
	cout<<"5";
	check = false;
  }
  if (right2 < 100) {
	cout<<"6";
	check = false;
  }
  if (right2 > 200) {
	cout<<"7";
	check = false;
  }
  if (left1 > right1) {
	  cout<<"8";
	  check = false;
  }
  if (left2 > right2) {
	  cout<<"9";
	  check = false;
  }
  if (check == false) return;
  mysql_query(connection, "SELECT f_id, s_id FROM facult_spec WHERE year='2004'");
  res = mysql_store_result(connection);
  while(row = mysql_fetch_row( res )){
    for (int i = 0; i < YEARS; ++i) {
      x = uniformDistribution(left1, right1);
      y = uniformDistribution(left2, right2);
      sprintf(buff, "UPDATE `vuniver`.facult_spec SET avg_contract_mark=%.0lf, avg_budget_mark=%.0lf WHERE f_id=%s AND s_id=%s AND year=%d", x, y, row[0], row[1], i+2004);
      //cout<<buff<<endl;
      mysql_query(connection, buff);
    }
  }
  cout<<"База перегенерована за рівномірним розподілом";
  //cout<<"<input type=\"button\" value=\"Назад\" onclick=\"history.back()\">"<<endl;
  mysql_free_result(res); 
}
void Template :: Controller()
{
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
  if(strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
    Read();
  else ReadPost();
  srand(time(0));
  if (en.arr[0].name[0] != 'L') Model1();
  else
    Model2();
}


int main()
{
  setlocale(LC_ALL,"rus");
  mysql.reconnect = true;
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  Template obj;
  obj.Controller();
  return 0;
}
