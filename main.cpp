#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <sys/mman.h>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "BPlusTree.h"
using namespace std;
/*
struct record{
	char id[7];
	char first_name[15];
	char last_name[15];
	char ssn[12];
	char user_name[15];
	char password[15];
};
*/
vector<record> parse_data_file(){
string line;
ifstream file;
file.open("SampleRecordssorted.csv");
if(!file.is_open()){
	cout<<"Wrong Path"<<endl;
	exit(1);
}
getline(file,line); 
//skip first line

vector<record> data;
int i=0;
while(getline(file,line)){
	if(line.empty()){
		continue;
	}
	if(i==0){
		cout<<line<<endl;
		i=1;
			
	} 
	vector<string> row;
	string word;	
	stringstream s(line);

	while (getline(s, word, ',') ) {
		row.push_back(word);
	}
	record r;
	strcpy (r.id, row[0].c_str());
	strcpy (r.first_name, row[1].c_str());	
	strcpy (r.last_name, row[2].c_str());
	strcpy (r.ssn, row[3].c_str());
	strcpy (r.user_name, row[4].c_str());
	strcpy (r.password, row[5].c_str());
	data.push_back(r);
}
file.close();
return data;
}

vector <record*> map(vector<record> data){
	size_t pagesize = getpagesize();
	cout<<"page size in bytes is: "<<pagesize<<endl;
	vector<record *> address;
	int length = sizeof(record);
	for(int i=0;i<data.size();i++){
		record* region=(record*)mmap(NULL, 
				length, 
				PROT_WRITE|PROT_READ, 
				MAP_ANON|MAP_PRIVATE, 
				-1,
				0);
		if (region == MAP_FAILED){
			perror("error mapping the record");
			exit(1);
		}
		record *t=&data[i];
		memcpy(region, t, length);
		msync(region, length, MS_SYNC);
		address.push_back(region);
	}
	return address;
}

int main(){
	vector<record> data = parse_data_file();
	cout<<data[0].id<<endl;
	
	vector <record*> address = map(data);
	record r= *(address[0]);
	cout<<"first instance of r's id is: \n";
	cout<<r.id<<endl;

	return 0;
}




