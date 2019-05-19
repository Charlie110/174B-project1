#ifndef BPlusTree_H
#define BPlusTree_H

#include <iostream>
#include <cstring>
#include <stdlib>
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

Class BPlusTree{
public:
	struct Node{
		Token type;
		vector<int> index;
		vecotr<void*> child;
		Node* parent;
		Node* next;
	}

	Node* Init(vector<record> data);
	void insert(record r);
	bool search(int id);
	void range(int id1, int id2);
}










#endif
