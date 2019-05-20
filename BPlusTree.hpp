#ifndef BPlusTree_H
#define BPlusTree_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

struct record{
        char id[7];
        char first_name[15];
        char last_name[15];
        char ssn[12];
        char user_name[15];
        char password[15];
};

enum Token{
	T_ROOT,
	T_LEAF,
	T_NODE,
};

struct Node{
	Token type;
	int index[4];
	void *child[5];
	Node* parent;
	Node* next;
	int size;
};


class BPlusTree{
public:
	Node* Init(vector<record> data, vector<record*> address);
	Node* helper_init(vector<Node> data, vector<Node*> address);
	void insert(record r,Node* root);
	bool search(int id, Node* root);
	void range(int id1, int id2, Node* root);
};










#endif
