#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <sys/mman.h>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include "BPlusTree.hpp"
#include <math.h>
using namespace std;

static int max_buffer_size=18;

static vector<Node* >path;

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
		//if(i==0){
		//	cout<<line<<endl;
		//	i=1;	
		//}

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

vector <page*> map(vector<record> data){
	
	vector<page> pages;
	vector<page*> page_address;
	int i=0;
	while(i<data.size()){
		page p;
		for(int j=0; j<max_amount_of_record_per_page; j++){
			if(j==0){
				//initialize
				p.record_size=0;
			}
			p.records[j]=data[i];
			p.record_size+=1;
			i++;
			if(i>=data.size()){
				pages.push_back(p);
				break;
			}
		}
		pages.push_back(p);
	}
	int page_length=sizeof(page);
	for(int i=0;i<pages.size();i++){

		page* region=(page*)mmap(NULL, 
				page_length, 
				PROT_WRITE|PROT_READ, 
				MAP_ANON|MAP_PRIVATE, 
				-1,
				0);
		if (region == MAP_FAILED){
			perror("error mapping the record");
			exit(1);
		}
		page *t=&pages[i];
		memcpy(region, t, page_length);
		msync(region, page_length, MS_SYNC);
		page_address.push_back(region);
	}
	return page_address;
}

page* insert_data_to_disk(vector<page*>& pages, record r){
	page* last_page=pages.back();
	page p;
	page* region;
	page* t;
	if(last_page->record_size < max_amount_of_record_per_page){
		p.record_size=last_page->record_size+1;
		for(int i=0; i<last_page->record_size; i++){
			p.records[i]=last_page->records[i];
		}	
		p.records[p.record_size-1]=r;
		munmap(last_page, page_length);
		region=(page*)mmap(NULL,
					page_length,
					PROT_WRITE|PROT_READ,
					MAP_ANON|MAP_PRIVATE,
					-1,
					0);
		t=&p;
		pages.pop_back();
	}
	else{
		p.record_size=1;
		p.records[0]=r;
		region=(page*)mmap(NULL,
				page_length,
				PROT_WRITE|PROT_READ,
				MAP_ANON|MAP_PRIVATE,
				-1,
				0);
		t=&p;
	}

	memcpy(region, t, page_length);
	msync(region, page_length, MS_SYNC);
	pages.push_back(region);
	return region;

}

int main(){
	cout<<"max amount of records per page is : "<<max_amount_of_record_per_page<<endl;
	vector<record> data = parse_data_file();	
	vector <page*> page_address = map(data);
	page r= *(page_address[0]);
	cout<<"first instance of r's id is: "<<r.records[0].id<<endl;
	
	vector<Node*> all_curr_nodes;

	for(int i=0; i<page_address.size();i++){
		page p=*(page_address[i]);
		int n=p.record_size;
		vector<record> records(p.records,p.records+n);
		BPlusTree B;	
		all_curr_nodes.push_back(B.Init(records, page_address[i]));
		
	}
	
	BPlusTree main_tree;
	Node* root=main_tree.helper_init(all_curr_nodes, "root");
	vector<Node*> buffer;
	string input="";
	

	while(1){
		cout<<"type insert, search or quit "<<endl;
		cin>>input;
		if(input == "insert"){
			string id;
			string fn="";
			string ln="";
			string ssn="";
			string user_name="";
			string password="";
			cout<<"record id 6 digit"<<endl;
			cin>>id;
			
			cout<<"first name"<<endl;
			cin>>fn;
			cout<<"last name"<<endl;
			cin>>ln;
			cout<<"ssn"<<endl;
			cin>>ssn;
			cout<<"user name"<<endl;
			cin>>user_name;
			cout<<"password"<<endl;
			cin>>password;
			
			record r;

			strcpy(r.id, id.c_str());
			strcpy(r.first_name, fn.c_str());
			strcpy(r.last_name,ln.c_str());
			strcpy(r.ssn, ssn.c_str());
			strcpy(r.user_name,user_name.c_str());
			strcpy(r.password,password.c_str());
			page* lastpage=insert_data_to_disk(page_address, r);
			Node* node=main_tree.root;
			main_tree.insert(r,lastpage, node, buffer);
			cout<<"buffer is now "<<buffer.size()<<endl;
			cout<<"clear buffer for reuse"<<endl;
			buffer.clear();
		}
		else if(input=="search"){
			cout<<"give 6 digit id "<<endl;
			int id;
			cin>>id;
			Node* node=root;
			main_tree.search(id, node, buffer);
			cout<<"buffer used "<<buffer.size()<<endl;
			cout<<"clear buffer for reuse"<<endl;
			buffer.clear();
		}
		else if(input=="quit"){
			cout<<"clean buffer"<<endl;
			buffer.clear();
			cout<<"free disk memory"<<endl;
			for(int i=0; i<page_address.size(); i++){
				munmap(page_address[i], page_length);
			}
			main_tree.clear(main_tree.root);

			exit(0);
		}
	
	}
	return 0;
}




