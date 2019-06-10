#ifndef BPlusTree_H
#define BPlusTree_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
#include <math.h> 
using namespace std;

const static int page_size=getpagesize();



struct record{
        char id[7];
        char first_name[15];
        char last_name[15];
        char ssn[12];
        char user_name[15];
        char password[15];
};

static int record_length = sizeof(record);

const static int max_amount_of_record_per_page = int(floor(page_size/record_length))-1;

struct page{
	int record_size;
	//max_amount_of_record_per_page is 50
	record records[50];
};
static int page_length=sizeof(page);
enum Token{
	T_ROOT,
	T_LEAF,
	T_NODE,
};

struct Node{
	Token type;
	int index[4];
	void *child[5];
	
	//Node* parent;
	Node* next;
	int size;
};


class BPlusTree{
public:
	BPlusTree();
	~BPlusTree();
	void clear(Node* node);

	Node* root;

	Node* Init(vector<record> data, page* address);
	Node* helper_init(vector<Node*> address, string flag);
	
	void insert(record r, page* add, Node* root, vector<Node*>& path);
	void add_data(int id, page* add, Node* node, Node* parent);
	void helper_split_leaf(int id, page* add, Node* r, vector<Node*>& path);
	void helper_split(Node* parent, Node* leaf, Node* region, vector<Node*>& path);
	
	int min_val(Node node);

	bool search(int id, Node* root, vector<Node*>& path);	
};










#endif
