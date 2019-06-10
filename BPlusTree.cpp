#include "BPlusTree.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/mman.h>
#include <algorithm>
using namespace std;

BPlusTree::BPlusTree(){
	root=NULL;
}
BPlusTree::~BPlusTree(){
}
void BPlusTree::clear(Node* node){
	if(node->type!=T_LEAF){
		for(int i=0; i<node->size+1; i++){
			clear((Node*)node->child[i]);
		}	
	}
	munmap(node, sizeof(Node));
}



int BPlusTree::min_val(Node node){

	while(node.type!=T_LEAF){
		node=*(Node*)node.child[0];
		//cout<<"in min val"<<endl;
		//cout<<"node type"<<node.type<<endl;
	}
	return node.index[0];
}

Node* BPlusTree::Init(vector<record> data, page* address){
	vector<Node> current_height_nodes;
	vector<Node*> current_height_nodes_address;
	//leafnode
	Node n;
	int i=0;
	if(data.size()%2==0){
	
	}
	else{
		n.type=T_LEAF;
		n.index[i] = (atoi(data[i].id));
		n.child[i] = address;
		i+=1;
		n.index[i] = (atoi(data[i].id));
		n.child[i] = (address);
		i+=1;
		n.index[i] = (atoi(data[i].id));
		n.child[i] = (address);
		i+=1;
		n.size=3;
		current_height_nodes.push_back(n);
	}
	int j=0;
	for(i;i<data.size();i++){
		if(j==0){
			Node new_node;
			new_node.type=T_LEAF;
			new_node.index[j] = (atoi(data[i].id));
			new_node.child[j] = address;
			new_node.size=2;
			current_height_nodes.push_back(new_node);
			//cout<<"current_height node size "<<current_height_nodes.size()<<endl;
			j+=1;
		}
		else{
			current_height_nodes.back().index[j] = (atoi(data[i].id));
			current_height_nodes.back().child[j] = (address);
			j=0;
		}
	}
	int length=sizeof(Node);
	int size=current_height_nodes.size();
	//cout<<"size is "<<size<<endl;
	for(int i=size-1; i>=0;i--){
		Node* region=(Node*)mmap(NULL,
				length,
				PROT_WRITE|PROT_READ,
				MAP_ANON|MAP_PRIVATE,
				-1,
				0);
		if (region == MAP_FAILED){
			perror("error mapping the nodes");
			exit(1);
		}
		if(i!=size-1){
			current_height_nodes[i].next=current_height_nodes_address.back();
		}
		else{
			current_height_nodes[i].next=NULL;
		}
		Node *n=&current_height_nodes[i];
		memcpy(region, n, length);
		msync(region,length,MS_SYNC);
		current_height_nodes_address.push_back(region);
	}
	//make them in order
	reverse(current_height_nodes_address.begin(), current_height_nodes_address.end());

	//cout<<"finished the leaf level \n";
	
	Node *root=helper_init(current_height_nodes_address, "node");

	return root;
}
Node* BPlusTree::helper_init(vector<Node*> address, string flag){
	//cout<<"helper init size of add "<<address.size()<<endl;
	vector<Node> data;
	for(int i=0; i<address.size();i++){
		data.push_back(*address[i]);
	}
	
	if(data.size()<=5){

		Node* root;
		Node n;
		if(flag=="root"){
			n.type=T_ROOT;
		}
		else{
			n.type=T_NODE;
		}
		for(int i=0; i<data.size()-1;i++){
			//the least value of right child
			n.index[i] = min_val(data[i+1]);
		}
		for(int k=0; k<address.size();k++){
			n.child[k] = address[k];	
		}

		n.size=data.size()-1;
		int length=sizeof(Node);
		root=(Node*)mmap(NULL,
				length,
				PROT_WRITE|PROT_READ,
				MAP_ANON|MAP_PRIVATE,
				-1,
				0);
		Node *temp= &n;
		memcpy(root, temp, length);
		msync(root, length, MS_SYNC);
		this->root=root;
		return root;
	}
	else{
		vector<Node> current_nodes;
		vector<Node*> current_nodes_address;
        	Node n;
		n.type=T_NODE;
        	int i=0;
        	if(data.size()%3==0){
        	
		}
		else if(data.size()%3==1){

			n.size=3;
			//put 4 nodes into a node
			//ie 3 index and 4 child

			while(i<4){
				if(i<3){
					n.index[i]=min_val(data[i+1]);
				}
				n.child[i]= address[i];
				i+=1;
			}		
			current_nodes.push_back(n);
		}
		
        	else{
			//put 5 nodes into a node
			//ie 4 index and 5 child
        		n.size=4;
			while(i<5){
                                if(i<4){
                                        n.index[i]=min_val(data[i+1]);
                                }
                                n.child[i]= address[i];
                                i+=1;
                        }
			current_nodes.push_back(n);
		}
		//cout<<"after making it mutiple of three \n";
        	int j=0;
        	for(i;i<data.size();i++){
			//2 index 3 childs
                	if(j==0){
                        	Node new_node;
                        	new_node.type=T_NODE;
                        	new_node.child[j] = address[i];
                        	new_node.size=2;
                        	current_nodes.push_back(new_node);
                        	j+=1;
                	}
                	else if(j==1){
                        	current_nodes.back().index[j-1] =min_val(data[i]);
                        	current_nodes.back().child[j] = (address[i]);

                        	j+=1;
                	}
			else{
				current_nodes.back().index[j-1] =min_val(data[i]);
				
				current_nodes.back().child[j] = (address[i]);
				j=0;
			}
        	}
		//cout<<"after saving all curr nodes \n";

		int length = sizeof(Node);
		for(int k=0; k<current_nodes.size();k++){
			
                	Node* region=(Node*)mmap(NULL,
                                	length,
                                	PROT_WRITE|PROT_READ,
                                	MAP_ANON|MAP_PRIVATE,
                                	-1,
                                	0);
                	if (region == MAP_FAILED){
                        	perror("error mapping the nodes");
                        	exit(1);
                	}
                	Node *t = &current_nodes[k];
			memcpy(region, t, length);
                //	cout<<"after memcpy \n";
			msync(region,length,MS_SYNC);
                	current_nodes_address.push_back(region);
		}

		return helper_init(current_nodes_address, flag);
	}


}


//
bool BPlusTree::search(int id, Node* root, vector<Node*>& path){
	//cout<<"in search \n";	
	while(root->type!= T_LEAF){
		//cout<<"root type "<<root->type<<endl;
		path.push_back(root);
		if(id<root->index[0]){
			//cout<<"smaller the 1st index "<<root->index[0]<<endl;
			root=(Node*)(root->child[0]);
		}
		else{	
			int i;
			for(i=root->size-1; i>=0; i--){
				if(id >= root->index[i]){
					//cout<<"larger than node->index[i] "<<root->index[i]<<endl;			
					break;
				}
			}
			root=(Node*)(root->child[i+1]);
		}
	}
	//at leaf node
	for(int i=0; i<root->size;i++){
		if(root->index[i] == id){
			page p=*(page *)(root->child[i]);
			record r;
			for(int j=0; j<p.record_size; j++){
				if(stoi(p.records[j].id)==id){
					r=p.records[j];
					cout<<"find, its id and name are:"<<endl;
					cout<<r.id<<" " <<r.first_name<<" "<<r.last_name<<endl;
					return true;}
			}
		}
	
	}
	cout<<"didnt find!  \n";
	cout<<"wrong id \n";
	return false;
}

void BPlusTree::insert(record r, page* address, Node* root, vector<Node*>&path){
	
	int id = atoi(r.id);
	int i=0;
        while(root->type!= T_LEAF){
		path.push_back(root);
                if(id < root->index[0]){
                        root=(Node*)(root->child[0]);

                }
                else{   
			int i=0;
                        for(i=root->size-1; i>=0; i--){
                                if(id>=root->index[i]){
                                        break;
                                }
                        }
                        root=(Node*)(root->child[i+1]);
                }
        }
	Node* parent=path.back();
	if(root->size<4){
		add_data(id, address, root, parent);
	}
	//we need to split
	else{
		helper_split_leaf(id,address,root, path);
	}
	return;
}


void BPlusTree::add_data(int id,page* address, Node* root, Node* parent){
	int i=0;
	if(id<root->index[0]){
		for(i=root->size; i>0; i--){
			root->index[i]=root->index[i-1];
			root->child[i]=root->child[i-1];
		}
		root->index[0]=id;
		root->child[0]=address;
	}
	else if(id>root->index[ root->size-1]){
		root->index[root->size] = id;
		root->child[root->size] = address;
	}
	else{
		for(i=0; i<root->size-1; i++){
			if(id>root->index[i] && id<root->index[i+1]){
				break;}
		}
		i+=1; // i is the place where we should put record
		for(int j=root->size; j>i; j--){
			root->index[j]=root->index[j-1];
			root->child[j]=root->child[j-1];
		}
		root->index[i]=id;
		root->child[i]=address;
	}
	root->size+=1;

	//make sure the parent index still has the least value of right child
	for(int k=0; k<parent->size; k++){
		parent->index[k]=min_val(*(Node*)parent->child[k+1]);
	}
	return;
}


void BPlusTree::helper_split_leaf(int id, page* address, Node* leaf, vector<Node*>& path){
	// xxxx =>  leaf => new_node => xxxx
	Node new_node;
	new_node.type=T_LEAF;
	new_node.index[0]=leaf->index[2];
	new_node.child[0]=leaf->child[2];

	new_node.index[1]=leaf->index[3];
	new_node.child[1]=leaf->child[3];
	new_node.size=2;
	
	//int prev_largest_val=leaf->index[3];
	leaf->size=2;
	int length=sizeof(Node);	
	Node* region=(Node*)mmap(NULL,
			length,
			PROT_WRITE|PROT_READ,
			MAP_ANON|MAP_PRIVATE,
			-1,
			0);
	if (region == MAP_FAILED){
		perror("error mapping the nodes");
		exit(1);
	}
	Node *t = &new_node;
	memcpy(region, t, length);
	msync(region,length,MS_SYNC);

	region->next=leaf->next;
	leaf->next=region;	
	
	//after making the new node, add data
	Node* parent = path.back();
	if(id<region->index[0]){
		add_data(id, address, leaf, parent);
	}
	else{
		add_data(id,address,region, parent);
	}
	parent=path.back();
	path.pop_back();
	helper_split(parent, leaf, region, path);
}
void BPlusTree::helper_split(Node* parent, Node* leaf, Node* region, vector<Node*>& path){
	//    parent
	// leaf region
	int j=0;
	while(1){
		if((Node*)parent->child[j] == leaf){
			break;
		}
		j++;
		if(j>=5){
			cout<<"wrong in helper split"<<endl;
			exit(1);
		}
	}

	//j is the index where the address of leaf stored in parent->child[]
	if(parent->size<4){
		//cout<<"parent size <4"<<endl;

                for(int k=parent->size+1; k>j;k--){
                        parent->child[k]=parent->child[k-1];
                }
                parent->child[j]=leaf;
                parent->child[j+1]=region;
		parent->size+=1;
		for(int k=0; k<parent->size;k++){
			parent->index[k]=min_val(*(Node*)parent->child[k+1]);
		}

        }
        else{
		// we need to split again

		//parent newnode
		// leaf->region
		Node new_node;
		new_node.type=T_LEAF;
        	new_node.index[0]=parent->index[2];
        	new_node.child[0]=parent->child[2];

        	new_node.index[1]=parent->index[3];
        	new_node.child[1]=parent->child[3];
		
		new_node.child[2]=parent->child[4];
        	new_node.size=2;
		parent->size=2;
        	int length=sizeof(Node);
        	Node* new_node_address=(Node*)mmap(NULL,
                	        length,
                        	PROT_WRITE|PROT_READ,
                        	MAP_ANON|MAP_PRIVATE,
                        	-1,
                        	0);
        	if (new_node_address == MAP_FAILED){
                	perror("error mapping the nodes");
                	exit(1);
        	}
        	Node *t = &new_node;
        	memcpy(new_node_address, t, length);
        	msync(new_node_address,length,MS_SYNC);
		

		if(j<=1){
			//put leaf and region in parent node
                	for(int k=parent->size; k>j;k--){
                        	parent->child[k]=parent->child[k-1];
                	}
                	parent->child[j]=leaf;
                	parent->child[j+1]=region;\
		}
		else if(j==2){
			//put leaf in parent, region in new node;
			parent->child[2]=leaf;
			new_node_address->child[0]=region;
		}
		else if(j==3){
			//put leaf and region in new node
			new_node_address->child[0]=leaf;
			new_node_address->child[1]=region;
		}
		else{
			//put leaf and region in new node

                        new_node_address->child[0]=new_node_address->child[1];
                        new_node_address->child[1]=leaf;
			new_node_address->child[2]=region;
		}

		for(int k=0; k<parent->size;k++){
                        parent->index[k]=min_val(*(Node*)parent->child[k+1]);
		}
		for(int k=0; k<new_node_address->size;k++){
                        new_node_address->index[k]=min_val(*(Node*)new_node_address->child[k+1]);
                }
		
		
		if(parent->type==T_ROOT){
			parent->type=T_LEAF;
			Node new_root;
                	new_root.child[0]=parent;
                	new_root.child[1]=new_node_address;
			new_root.index[0]=min_val(*(Node*)new_root.child[1]);

			new_node.size=1;
                	int length=sizeof(Node);
                	Node* add=(Node*)mmap(NULL,
                        	        length,
                                	PROT_WRITE|PROT_READ,
                                	MAP_ANON|MAP_PRIVATE,
                                	-1,
                                	0);
                	Node *t = &new_root;
                	memcpy(add, t, length);
                	msync(add,length,MS_SYNC);
			this->root=t;
			return;
		}
		else{
			Node* new_parent = path.back();
			path.pop_back();
			helper_split(new_parent, parent, new_node_address, path);
		}
        }
	return;
}

















