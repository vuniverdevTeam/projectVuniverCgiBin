#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>

#define FORM_SIZE 12
#define YEARS 10
using namespace std;

struct Prop{
  char *f, *s;
  float prop1, prop2;
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
  //entr(){arr = new n_v[FORM_SIZE];}
  //~entr(){delete[] arr;}
  n_v *operator[](short index){return (this->arr)+index;}
  const char* operator()(char *str){
    for(short i = 0; i < FORM_SIZE; i++)
      if(strcmp(arr[i].name, str) == 0)
        return arr[i].value;
    return 0;
  }
};

MYSQL *connection, mysql;
MYSQL_ROW row, FSrow;
MYSQL_RES *res, *FSres;

double rating;
float erf(float t) {
	MYSQL_ROW line;
	MYSQL_RES *result;
	char str[44];
	float r;
	if(t>4) t=4;
	else if(t<-4) t=-4;
	sprintf(str, "SELECT ft FROM laplas WHERE t='%f'", t<0 ? (-t) : t);
	mysql_query(connection, str);
	result = mysql_store_result(connection);
	mysql_num_rows(result);
	line = mysql_fetch_row(result);
	r = t<0 ? -atof(line[0]) : atof(line[0]);
	mysql_free_result(result);
	return r;
}
double Probability(int* markArr, double &D, double &M)
{
	double start, X;
	M = 0, D = 0;
	int	i;
	
	int newArr[4];
	int length = 0;
	for (i = 0; i < YEARS; ++i)
	{
		if (markArr[i] > 0)
		{
			newArr[length++] = markArr[i];
		}
	}
	if(length == 1)
		return (rating >= newArr[0]) ? 100 : 0;
	else if (length == 0) return 0;

	for (i = 0; i < length; ++i) {
		M += newArr[i];
	}
	M /= length;
	//
	for (i = 0; i < length; ++i) {
		D += pow(newArr[i] - M, 2.0);
	}
	D /= (length - 1);

	D = sqrt(double(2.0) * D);
	X = (erf((double(M) - double(100)) / D) - erf((double(M) - double(rating)/**1.12*/) / D)) * 50;

	return X;
}

char *ito(int value, char *dest)
{
  int i, n;
  n = value;
  for(i = 0; n != 0; n/=10)
    i++;
  n = i;
  dest[n] = 0;
  for(n--; value != 0 && n>-1; n--, value /= 10)
    dest[n]= value%10 + '0';
  return dest;
}

class Template
{
protected:
  char buff[1001], *ptr;
  entr en;
  void Read();
  void ReadPost();
public:
  void Model1();
  void Model2();
  void Controller();
  friend class SQL;
};

void Template :: Read()
{
  short i, j = 0;
  strcpy(buff, getenv("QUERY_STRING"));
  //cout<<"1:"<<buff<<"<br/>"<<endl;

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
        }//write value
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

int arr[20]={0};
void Template :: Model1()
{
  char str[128];
  char tmp[12];
  strcpy(tmp, en(")))"));
  tmp[strlen(tmp) - 1] = 0;
  sprintf(str, "SELECT * FROM user_marks WHERE user_id='%s'", tmp);
  mysql_query(connection, str);
  res = mysql_store_result(connection);
  if(row = mysql_fetch_row(res)) {
		cout<<"{mark : ["<<row[2]<<","<<row[4]<<","<<row[6]<<","<<row[8]<<","<<row[1]<<"],";
		cout<<"sub : ["<<row[3]<<","<<row[5]<<","<<row[7]<<"],";
	}
	else {
		cout<<"{mark : ["<<0<<","<<0<<","<<0<<","<<0<<","<<0<<"],";
		cout<<"sub : ["<<2<<","<<3<<","<<4<<"],";
  }
  mysql_free_result(res);
  
  str[0] = 0;
  sprintf(str, "SELECT s_id, f_id FROM user_facult_spec WHERE user_id='%s'", tmp);
  mysql_query(connection, str);
  //cout<<str;
  res = mysql_store_result(connection);
  cout<<"arr : [";
  if(row = mysql_fetch_row(res))
    cout<<row[0]<<","<<row[1];
  while(row = mysql_fetch_row(res))
    cout<<","<<row[0]<<","<<row[1];
  cout<<"]}";
  mysql_free_result(res);
}
short searchMarkFS(const char *f, const char *s)
{
  int i, a, b;
  char tmp1[4], tmp2[8];
  unsigned long n;
  char query_ptr[500];

  sprintf(query_ptr, "SELECT f_id FROM faculties WHERE f_id='%s'", f);
  mysql_query(connection, query_ptr);
  res = mysql_store_result(connection);
  n = mysql_num_rows(res);
  if(n == 0)return 1; //0 rows returned
  if (row = mysql_fetch_row( res ))
    b = atoi(row[0]);
  mysql_free_result(res);
  //cout<<b<<" f<br/>"<<endl;

  sprintf(query_ptr, "SELECT s_id FROM specialities WHERE s_id='%s'", s);
  mysql_query(connection, query_ptr);
  res = mysql_store_result(connection);
  n = mysql_num_rows(res);
  if(n == 0)return 2; //0 rows returned
  while ( row = mysql_fetch_row( res ) ){
    a = atoi(row[0]);
  }
  mysql_free_result(res); 
  //cout<<a<<" s<br/>"<<endl;

  sprintf(query_ptr, "SELECT avg_contract_mark, avg_budget_mark FROM facult_spec_abit WHERE s_id='%s' AND f_id='%s'", ito(a,tmp1), ito(b,tmp2));
  mysql_query(connection, query_ptr);
  res = mysql_store_result(connection);
  n = mysql_num_rows(res);
  if(n == 0)return 3; //0 rows returned
  for (i = 0; i < YEARS; i++)
    if(row = mysql_fetch_row(res)){
      arr[i] = atoi(row[0]);
      arr[i+YEARS] = atoi(row[1]);
    }
    mysql_free_result(res);
    return -1;
}
void Template :: Model2()
{
  double M, D;
  char str[128];
  char tmp[12];
  float m1, m2, m3, m4, m5, k = 0.2;
	//cout<<en("m5");
  m1 = atof(en("m1")), m2 = atof(en("m2")), m3 = atof(en("m3")), m4 = atof(en("m4")), m5 = atof(en("m5"));
  if(m3 == 0)
		k=0.25;
	if(m4 == 0){
		if(k == 0.25) k=0.4;
		else k = 0.25;
	}
  rating = k*m1 + k*m2 + k*m3 + k*m4;
		if(k == float(0.4)) rating += (0.2*m5);
		else rating += (k*m5);
  cout<<"{ F : '";
  sprintf(str, "SELECT u_id, f_fullname FROM faculties WHERE f_id='%s'", en("Fac"));
  mysql_query(connection, str);
  res = mysql_store_result(connection);
  row = mysql_fetch_row(res);
  
  cout<<row[1];

  cout<<"',U : '";
  sprintf(str, "SELECT u_fullname FROM university WHERE u_id='%s'", row[0]);
mysql_free_result(res);
  mysql_query(connection, str);
  res = mysql_store_result(connection);
  row = mysql_fetch_row(res);
  mysql_free_result(res);
  cout<<row[0];

  cout<<"',S : '";
  sprintf(str, "SELECT s_name FROM specialities WHERE s_id='%s'", en("Spec"));
  mysql_query(connection, str);
  res = mysql_store_result(connection);
  row = mysql_fetch_row(res);
  mysql_free_result(res);
  cout<<row[0];

  searchMarkFS(en("Fac"), en("Spec"));

  cout<<"',B : '";
  cout.setf(ios::fixed);
  cout.precision(1);
  cout<<double(Probability(arr+YEARS, D, M));
  cout<<"',C : '";
  cout.setf(ios::fixed);
  cout.precision(1);
  cout<<double(Probability(arr, D, M));
  cout<<"'}";
}

void Template :: Controller()
{
  cout <<"Access-Control-Allow-Origin: *\r\n\r\n";
  int exit = 1, i = 0;
  float m1, m2, m3 = 0, m4 = 0, m5, k = 0.2;
  if(strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
    Read();
  else ReadPost();
  //cout<<(en.arr[0].name[0] == ')');
  if(en.arr[0].name[0] == ')') Model1();
  else {
    Model2();
  }
}

int main()
{
  Template obj;
  mysql.reconnect = true;
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  obj.Controller();
  return 0;
}
