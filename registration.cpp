#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>
#define FORM_SIZE 13
#define YEARS 10
#define MAX_RAND 18446744073709551615
#define SESSION_LENGHT 432000
using namespace std;

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

MYSQL *connection, mysql;
MYSQL_ROW row, FSrow;
MYSQL_RES *res, *FSres;
struct info
	{
	char* name;
	char* value;
	};

info* arr = new info[FORM_SIZE];

class Reg
{
protected:
	
	char query[200];
	int count;
public:
	void read();
	unsigned long long toBase();
	void toCount();
	void convert();
};

void Reg :: convert()
{
	strcpy(query, " UPDATE `users` SET `mail`= REPLACE(`mail`, '%40', '@')");
	mysql_query(connection, query);
}

void Reg :: toCount()
{
	count = 0;
	for(int i = 4; i<11; i+=2)
	{
		if(strlen(arr[i].value) <2)
		{
			count++;
		}
	}
	//cout << count;
}

unsigned long long Reg :: toBase()
{
	unsigned long long ID = generateID(arr[0].value);
	sprintf(query,"INSERT INTO users(mail, password, session_id, session_end) values('%s', '%s', '%llu', '%llu')", arr[0].value, arr[1].value, (unsigned long long)ID, (unsigned long long)time(0) + SESSION_LENGHT);
	mysql_query(connection, query);
	sprintf(query,"SELECT user_id FORM users WHERE mail='%s'", arr[0].value);
	mysql_query(connection, query);
	res = mysql_store_result(connection);
	if (row = mysql_fetch_row( res )){
		sprintf(query,"INSERT INTO user_marks(user_id) VALUES('%s')",row[0], 100, 100, 1, 100,2,100,3,100);
		mysql_query(connection, query);
	}
	mysql_free_result(res);
	switch(count)
	{
	case 2:
		sprintf(query,
		"INSERT INTO user_marks(certificate,mark_1,subject_2,mark_2,subject_3,mark_3,subject_4,mark_4) VALUES('%s','%s','%s','%s','-1','-1','-1','-1')",arr[11].value, arr[4].value, arr[5].value, arr[6].value);
		break;
	case 1:
		sprintf(query,
		"INSERT INTO user_marks(certificate,mark_1,subject_2,mark_2,subject_3,mark_3,subject_4,mark_4) VALUES('%s','%s','%s','%s','%s','%s','-1','-1')",arr[11].value, arr[4].value, arr[5].value, arr[6].value, arr[7].value, arr[8].value);
		break;
	case 0:
		sprintf(query,
		"INSERT INTO user_marks(certificate,mark_1,subject_2,mark_2,subject_3,mark_3,subject_4,mark_4) VALUES('%s','%s','%s','%s','%s','%s','%s','%s')",arr[11].value, arr[4].value, arr[5].value, arr[6].value, arr[7].value, arr[8].value, arr[9].value, arr[10].value);
		break;
	}
	//mysql_query(connection, query);
	return ID;
}


void Reg ::  read()
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
          strcpy(arr[j].value, ptr);
          ptr += i;
          break;
        }
        else if(ptr[i] == 0) {
          arr[j].value = new char[i+1];
          strcpy(arr[j].value, ptr);
          ptr += i;
          return;
        }//write value
				j++;
        if(j == FORM_SIZE)return;
  }
}


int main()
{
  //cout << "Content-type:text/html\r\n\r\n" << endl;
   mysql.reconnect = true;
  mysql_init(&mysql);
  connection = mysql_real_connect(&mysql,"185.25.117.161",
    "vuniver","5S1CQhvO","vuniver",3306,0,0);
  mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");
  Reg obj;
  obj.read();
  //obj.toCount();
  unsigned long long ID = obj.toBase();
  obj.convert();

	char *pt;
	for(pt = arr[0].value; *pt; pt++)
		if(*pt == '%' && *(pt+1) == '4' && *(pt+2) == '0') break;
	*pt = '\0';
	pt += 3;

	FILE *sendmail; 
 sendmail=popen("/usr/lib/sendmail -t","w");
   fprintf(sendmail,"To: %s@%s\n", arr[0].value, pt);
   fprintf(sendmail,"From: alex@inet-tech.org.ua\n");
   fprintf(sendmail,"Subject: Registration\n");
	 fprintf(sendmail,"Content-Type: text/html; charset=\"utf8\"\n");
	 fprintf(sendmail,"\n<br/>Доброго дня,<br/>");
	 fprintf(sendmail,"При реєстрації на сайті <a href=\"http://alex.inet-tech.org.ua/project%20ISM/\">VUNIVER</a> було вказано вагу адресу. Щоб завершити реєстрацію й перейти до вашого кабінету натисніть на посилання нижче:<br/>");
	 fprintf(sendmail,"<br/><p>http://alex.inet-tech.org.ua/project%20ISM/cabinet.html?confirm=%llu\n</p>", ID);
	 fprintf(sendmail,"<br/>Якщо ви не реєструвались на сайті <a href=\"http://alex.inet-tech.org.ua/project%20ISM/\">VUNIVER</a> можете проігнорувати цей лист.\n");
   fprintf(sendmail,".\n");
 pclose(sendmail);

	cout<<"Location: http://alex.inet-tech.org.ua/project ISM/success.html\r\n\r\n"<<endl;
  return 0;
}
