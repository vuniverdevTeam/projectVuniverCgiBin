#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mysql.h>

#define FORM_SIZE 16
using namespace std;

int YEAR;
int bCount = 0, cCount  = 0;

struct Prop{
	char *f, *s;
	float prop1, prop2;
};

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
	if(YEAR == 1)
		return (rating == markArr[0]) ? 50 : (rating < markArr[0]) ? 0 : 100;
	for (i = 0; i < YEAR; ++i) {
		M += markArr[i];
	}
	M /= YEAR;
	//
	for (i = 0; i < YEAR; ++i) {
		D += pow(markArr[i] - M, 2.0);
	}
	D /= (YEAR - 1);

	D = sqrt(double(2.0) * D);
	X = (erf((double(M) - double(100)) / D) - erf((double(M) - double(rating)/**1.12*/) / D)) * 50;

	return X;
}

double GammLn(double x)
{
	double cof[]={
			 2.5066282746310005,
			 1.0000000000190015,
			 76.18009172947146,
			-86.50532032941677,
			 24.01409824083091,
			-1.231739572450155,
			 0.1208650973866179e-2,
			-0.5395239384953e-5,
	};
	double y, ser, *co;
	int j;
	ser = cof[1]; 
	y = x;
	co = cof + 2;
	for(j = 2; j < 8; j++) {
			y += 1.;
		ser += (*co)/y;
		co++;
	}
	y = x + 5.5;
	y -= (x + 0.5)*log(y);
	return(-y + log(cof[0]*ser/x));
}

double Beta(double x, double y) {
	return(exp(GammLn(x)+GammLn(y)-GammLn(x+y)));
}


double b(int* arr, double rating)
{
	if(YEAR == 1)
		return (rating == arr[0]) ? 50 : (rating < arr[0]) ? 0 : 100;
	int length = 0;
	int	i;
	for (i = 0; i < YEAR; ++i)
	{
		if (arr[i] > 0) length++;
	}
	double alpha, beta;
	double B, F, x, y, function;
	double M = 0, D = 0;
	double* markArr = new double[length];
	
	int sum = 0;
	const int count = 24000;

	for (i = 0; i < length; ++i) {
		markArr[i] = arr[i];
		markArr[i] -= 100;
		markArr[i] /= 100;
	}
	rating -= 100;
	rating /= 100;

	for (i = 0; i < length; ++i) {
		M += markArr[i];
	}
	M /= length;
	for (i = 0; i < length; ++i) {
		D += pow(markArr[i] - M, 2.0);
	}

	if (length != 1) D /= (length - 1);

	if (D < 0.0001) {
		if ((float)rating == (float)M) return 50;
		else if (rating > M) return 100;
		else if (rating < M) return 0;
	}

	alpha = M * (((M * (1 - M)) / D) - 1);
	beta = (1 - M) * (((M * (1 - M)) / D) - 1);

	if (alpha < 1) alpha = 1;
	if (beta < 1) beta = 1;

	B = Beta(alpha, beta);

	/*if (alpha < 1 || beta < 1) {
		cout<<"error";
		return 0;
	}*/
	double x_moda = alpha - 1;
	x_moda /= (alpha + beta - 2);
	function = (1 / B) * pow(x_moda, alpha - 1);
	function *= pow(1 - x_moda, beta - 1);

	srand(time(0));
	for (i = 0; i < count; ++i) {
		x = rand()%1001;
		x /= 1000;
		x *= rating;
		y = rand()%1001;
		y /= 1000;
		y *= function;
		F = (1 / B) * pow(x, alpha - 1);
		F *= pow(1 - x, beta - 1);
		if (F > y) sum++;
	}
	double Probability = sum * rating * function / (double)count;
	int p = int(Probability*100);
	Probability = p + (int)((Probability*100 - p) * 100) / 100.0;
	if (Probability > 100) return 100;
	if (Probability < 0) return 0;
	return Probability;
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

int CMP(const void *A, const void *B)
{
	if(((Prop*)A)->prop2 > ((Prop*)B)->prop2)
		return -1;
	if(((Prop*)A)->prop2 < ((Prop*)B)->prop2)
		return 1;
	if(((Prop*)A)->prop1 > ((Prop*)B)->prop1)
		return -1;
	if(((Prop*)A)->prop1 < ((Prop*)B)->prop1)
		return 1;
	return 0;
}

class SQL
{
private:
	int query_state;
public:
	SQL(){mysql_query(connection, "SET NAMES utf8 COLLATE utf8_unicode_ci");}
	short searchMarkUFS(const char *u, const char *f, const char *s, int *arr);
	Prop *searchAllFS(int &n, const char *f = 0, const char *s = 0);
	short searchMarkFS(const char *f, const char *s, int *arr);
};

short SQL :: searchMarkUFS(const char *u, const char *f, const char *s, int *arr)
{
	int i, a, b;
	char tmp1[4], tmp2[8];
	unsigned long n;
	char query_ptr[500];
	sprintf(query_ptr, "SELECT u_id FROM university WHERE u_id='%s'", u);
	mysql_query(connection, query_ptr);
	res = mysql_store_result(connection);
	n = mysql_num_rows(res);
	if(n == 0){
		return 0; //0 rows returned
	}
	if (row = mysql_fetch_row( res ))
		a = atoi(row[0]);
	mysql_free_result(res); 

	sprintf(query_ptr, "SELECT f_id FROM faculties WHERE u_id='%s' AND f_id='%s'", ito(a, tmp1), f);
	mysql_query(connection, query_ptr);
	res = mysql_store_result(connection);
	n = mysql_num_rows(res);
	if(n == 0)return 1; //0 rows returned
	if (row = mysql_fetch_row( res ))
		b = atoi(row[0]);
	mysql_free_result(res);

	sprintf(query_ptr, "SELECT s_id FROM specialities WHERE s_id='%s'", s);
	query_state = mysql_query(connection, query_ptr);
	res = mysql_store_result(connection);
	n = mysql_num_rows(res);
	if(n == 0)return 2; //0 rows returned
	while ( row = mysql_fetch_row( res ) ){
		a = atoi(row[0]);
	}
	mysql_free_result(res); 

	sprintf(query_ptr, "SELECT avg_contract_mark, avg_budget_mark FROM facult_spec_abit WHERE s_id='%s' AND f_id='%s'", ito(a,tmp1), ito(b,tmp2));
	query_state = mysql_query(connection, query_ptr);
	res = mysql_store_result(connection);
	n = mysql_num_rows(res);
	YEAR  = n;
	double* bArr = new double[4];
	double* cArr = new double[4];
	if(n == 0)return 3; //0 rows returned
	for (i = 0; i < n; i++)
		if(row = mysql_fetch_row(res)){
			arr[i] = atoi(row[0]);
			arr[i+n] = atoi(row[1]);
		}
		mysql_free_result(res);
		return -1;
}

short SQL :: searchMarkFS(const char *f, const char *s, int *arr)
{
	int i;
	int n; 
	
	double* bArr = new double[4];
	double* cArr = new double[4];
	
	char query_ptr[500];
	sprintf(query_ptr, "SELECT avg_contract_mark, avg_budget_mark FROM facult_spec_abit WHERE s_id='%s' AND f_id='%s'", s, f);
	mysql_query(connection, query_ptr);
	res = mysql_store_result(connection);
	n = mysql_num_rows(res);
	YEAR = n;
	if(mysql_num_rows(res) == 0)return 3; //0 rows returned
	for (i = 0; i < YEAR; i++)
		if(row = mysql_fetch_row(res)){
			arr[i] = atoi(row[0]);
			arr[i+YEAR] = atoi(row[1]);
		}
	mysql_free_result(res);
	return -1;
}
Prop *SQL :: searchAllFS(int &n, const char *f, const char *s){
	n = 0;
	double M, D;
	int* marks = new int[2*4];
	mysql_query(connection, "SELECT f_id, s_id FROM coeff_abit");
	Prop *arr = new Prop[10000];
	FSres = mysql_store_result(connection);
	for(n = 0; FSrow = mysql_fetch_row(FSres); n++){
		if(*f != 0 && strcmp(f,FSrow[0]) != 0) {
			n--;
			continue;
		}
		if(*s != 0 && strcmp(s,FSrow[1]) != 0) {
			n--;
			continue;
		}
		arr[n].f=strdup(FSrow[0]);
		arr[n].s=strdup(FSrow[1]);
		searchMarkFS(FSrow[0], FSrow[1], marks);
		arr[n].prop1 = Probability(marks, M, D);
		arr[n].prop2 = Probability(marks+YEAR, M, D);
	}
	qsort(arr, n, sizeof(Prop), CMP);
	mysql_free_result(FSres);
	return arr;
}

class Template
{
protected:
	char buff[1001], *ptr;
	ifstream fin;
	entr en;
	short err, errMark, errSub;
	void Read();
	void ReadPost();
public:
	int View();
	void Model(int dec);
	void Controller(char* file);
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
int arr[20]={0};
float m1, m2, m3 = 0, m4 = 0, m5, k = 0.2;
void Template :: Model(int dec)
{
	double M, D;
	char str[128];
	switch(dec)
	{
	case 'A' :
		cout<<"{ marks : ['"<< en("mark5")<<"', '"<< en("mark1")<<"', '"<< en("mark2")<<"', '"<< en("mark3")<<"', '"<< en("mark4")<<"']";
		cout<<", subjs : ['1', '"<< en("sub2")<<"', '"<< en("sub3")<<"', '"<< en("sub4")<<"']";
		cout<<", reg : '"<< en("reg")<<"', city : '"<< en("city")<<"', Univer : '"<< en("Univer")<<"', Fac : '"<< en("Fac")<<"', Spec : '"<< en("Spec")<<"'}";
		break;
	case 'U' :
		sprintf(str, "SELECT u_fullname FROM university WHERE u_id='%s'", en("Univer"));
		mysql_query(connection, str);
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		mysql_free_result(res);
		cout<<row[0];
		break;
	case 'F' :
		sprintf(str, "SELECT f_fullname FROM faculties WHERE f_id='%s'", en("Fac"));
		mysql_query(connection, str);
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		mysql_free_result(res);
		cout<<row[0];
		break;
	case 'S' :
		sprintf(str, "SELECT s_name FROM specialities WHERE s_id='%s'", en("Spec"));
		mysql_query(connection, str);
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		mysql_free_result(res);
		cout<<row[0];
		break;
	case 'M' :
		cout.setf(ios::fixed);
		cout.precision(1);
		cout<<k<<"*"<<m1<<"+"<<k<<"*"<<m2;
		if(m3 != 0) cout<<"+"<<k<<"*"<<m3;
		if(m4 != 0) cout<<"+"<<k<<"*"<<m4;
		if(k != float(0.4))cout<<"+"<<k<<"*"<<m5<<"=";	
		else cout<<"+"<<"0.2*"<<m5<<"=";	
		cout<<rating;
		break;
	case 'C' :
			sprintf(str, "SELECT optionValue FROM config WHERE optionId='1'");
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			mysql_free_result(res);
		cout.setf(ios::fixed);
		cout.precision(1);
		if(row[0][0] == '0') cout<<Probability(arr, M, D)<<"%"<<endl;
		else cout<<b(arr, rating)<<"%"<<endl;
		//cout<<"M: "<<M<<"<br/>"<<endl;
		//cout<<"D: "<<D<<endl;
		break;
	case 'B':
			sprintf(str, "SELECT optionValue FROM config WHERE optionId='1'");
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			mysql_free_result(res);
		cout.setf(ios::fixed);
		cout.precision(1);
		if(row[0][0] == '0') cout<<Probability(arr+YEAR, M, D)<<"%"<<endl;
		else cout<<b((arr+YEAR), rating)<<"%"<<endl;
		//cout<<"M: "<<M<<"<br/>"<<endl;
		//cout<<"D: "<<D<<endl;
		break;
	case 'c':
		for(short i = 0; i < YEAR; i++)
			cout<<"<td>"<<arr[i]<<"</td>"<<endl;
		break;
	case 'b':
		for(short i = 0; i < YEAR; i++)
			cout<<"<td>"<<arr[i+YEAR]<<"</td>"<<endl;
		break;
	case 'H':{
		SQL obj;
		int n, i, j = 1;
		const char *r=en("reg"), *c=en("city"), *s=en("Spec"), *f=en("Fac"), *u=en("Univer");
		Prop *arr;
		mysql_free_result(res);
		arr=obj.searchAllFS(n, f, s);
		for(i = 0; i < n; i++){

			sprintf(str, "SELECT u_id FROM faculties WHERE f_id='%s'", arr[i].f);
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			mysql_free_result(res);

			if(*u != 0 && strcmp(u, row[0]) != 0) continue;

			sprintf(str, "SELECT u_fullname, reg_id, city_id, rating FROM university WHERE u_id='%s'", row[0]);
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			mysql_free_result(res);

			if(*r != 0 && strcmp(r, row[1]) != 0) continue;
			if(*c != 0 && strcmp(c, row[2]) != 0) continue;
		
			cout<<"<tr><td>"<<row[3]<<"</td>";
			cout.setf(ios::fixed);
			cout.precision(1);
			cout<<"<td class=\"budg\">"<<arr[i].prop2<<"%</td> <td class=\"contr\">"<<arr[i].prop1<<"%</td>";

			cout<<"<td>"<<row[0];

			sprintf(str, "SELECT f_fullname FROM faculties WHERE f_id='%s'", arr[i].f);
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			cout<<"</td><td>"<<row[0];
			mysql_free_result(res);

			sprintf(str, "SELECT s_name FROM specialities WHERE s_id='%s'", arr[i].s);
			mysql_query(connection, str);
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			cout<<"</td><td>"<<row[0];
			mysql_free_result(res);

			cout<<"</td>";
	cout << "<td id = 'addFav' class = 'btn btn-success' onclick = 'getInfo(event)'>Зберегти у кабінет</td></tr>";
			j++;
		}
		if(j == 1)
			cout<<"<tr><td colspan=\"6\" align=\"center\">"<<"Співпадінь не знайдено"<<"</td></tr>";
		delete[]arr;
		break;
					 }
	default: cout << " " << endl;
	}
}

void Template :: Controller(char* file)
{
	int exit = 1, i = 0;
	bool isNorm, singleMode = true;
	if(strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
		Read();
	else ReadPost();
	SQL obj;
	errMark = 0, errSub = 0;
	err = obj.searchMarkUFS(en("Univer"), en("Fac"), en("Spec"), arr);

	if(*en("sub3")!='\0' && !strcmp(en("sub2"), en("sub3")))
		errSub += SUB23;
	if(*en("sub4")!='\0' && !strcmp(en("sub2"), en("sub4")))
		errSub += SUB24;
	if(*en("sub3")!='\0' && en("sub4") && !strcmp(en("sub3"), en("sub4")))
		errSub += SUB34;

	m1 = atof(en("mark1")), m2 = atof(en("mark2")), m3 = atof(en("mark3")), m4 = atof(en("mark4")), m5 = atof(en("mark5"));
	if(m1<100 || m1>200)
		errMark += MARK1;
	if(m2<100 || m2>200)
		errMark += MARK2;
	if(m3 == 0)
		k=0.25;
	else if(m3<100 || m3>200)
		errMark += MARK3;
	if(m4 == 0){
		if(k == 0.25) k=0.4;
		else k = 0.25;
	}
	else if(m4<100 || m4>200)
		errMark += MARK4;
	if(m5<100 || m5>200)
		errMark += MARK5;

	singleMode = atoi(en("Univer")) && atoi(en("Fac")) && atoi(en("Spec"));
	isNorm = singleMode ? (err == -1 && errMark == 0 && errSub == 0):(errMark == 0 && errSub == 0);
	if(isNorm){
		if(singleMode)
			fin.open(file);
		else fin.open("../VUNIVER/input2.html");
		cout <<"Content-Type: text/html\r\n\r\n";
		if(!fin.is_open())return;
		rating = k*m1 + k*m2 + k*m3 + k*m4;
		if(k == float(0.4)) rating += (0.2*m5);
		else rating += (k*m5);
	}
	else{
		fin.open("../VUNIVER/back.html");
		if(!fin.is_open())return;
		cout <<"Content-Type: text/html\r\n\r\n";
		while(!fin.eof())
		{
			fin.getline(buff, 1000);
			cout<<buff<<endl;
		}
		//fin.close();
		return;
	}
	while(exit != 0)
	{
		exit = View();
		if(exit == -1)
		{
			if(ptr[1]=='~'){
					Model(ptr[0]);
					cout<<ptr+2<<endl;
			}
			else cout<<"~"<<ptr<<endl;
		}
	}
}

Template obj;
int main()
{
	mysql.reconnect = true;
	mysql_init(&mysql);
	connection = mysql_real_connect(&mysql,"185.25.117.161",
		"vuniver","5S1CQhvO","vuniver",3306,0,0);

	obj.Controller("../VUNIVER/input.html");
	return 0;
}
