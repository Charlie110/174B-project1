#include "BPlusTree.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/mman.h>
#include <algorithm>
using namespace std;

Node* BPlusTree::Init(vector<record> data, vector<record*> address){
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
		n.child[i] = (address[i]);
		i+=1;
		n.index[i] = (atoi(data[i].id));
		n.child[i] = (address[i]);
		i+=1;
		n.index[i] = (atoi(data[i].id));
		n.child[i] = (address[i]);
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
			new_node.child[j] = (address[i]);
			new_node.size=2;
			current_height_nodes.push_back(new_node);
			j+=1;
		}
		else{
			current_height_nodes.back().index[j] = (atoi(data[i].id));
			current_height_nodes.back().child[j] = (address[i]);
			j=0;
		}
	}
	int length=sizeof(Node);
	int size=current_height_nodes.size();
	for(int i=size; i>=0;i--){
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
		if(i!=size){
			current_height_nodes[i].next=current_height_nodes_address.back();
		}
		Node *n=&current_height_nodes[i];
		memcpy(region, n, length);
		msync(region,length,MS_SYNC);
		current_height_nodes_address.push_back(region);
	}
	//make them in order
	reverse(current_height_nodes_address.begin(), current_height_nodes_address.end());

	cout<<"finished the leaf level \n";
	Node *root=helper_init(current_height_nodes, current_height_nodes_address);


	return root;
}

Node* BPlusTree::helper_init(vector<Node> data, vector<Node*> address){
	//cout<<"in helper init \n";
	if(data.size()<=5){
	//	cout<<"make the root node \n";
		Node* root;
		Node n;
		n.type=T_ROOT;
		for(int i=0; i<data.size()-1;i++){
			n.index[i] = (data[i].index[ data[i].size-1 ]);
	//		cout<<"root index: "<<n.index[i]<<endl;
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
		return root;
	}
	else{
		//cout<<"make inner node \n";
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
					n.index[i]=data[i].index[ data[i].size-1 ];
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
                                        n.index[i]=data[i].index[ data[i].size-1 ];
                                }
                                n.child[i]= address[i];
                                i+=1;
                        }
			current_nodes.push_back(n);
		}
		//cout<<"after making it mutiple of three \n";
        	int j=0;
        	for(i;i<data.size();i++){
                	if(j==0){
                        	Node new_node;
                        	new_node.type=T_NODE;
                        	new_node.index[j] = data[i].index[ data[i].size-1 ];
                        	new_node.child[j] = address[i];
                        	new_node.size=2;
                        	current_nodes.push_back(new_node);
                        	j+=1;
                	}
                	else if(j==1){
                        	current_nodes.back().index[j] =data[i].index[ data[i].size-1 ] ;
                        	current_nodes.back().child[j] = (address[i]);
				//cout<<current_nodes.back().size<<endl;
                        	j+=1;
                	}
			else{
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

		return helper_init(current_nodes, current_nodes_address);
	}


}


bool BPlusTree::search(int id, Node* root){
	//cout<<"in search \n";
	//cout<<root->index[0]<<endl;
	//cout<<"root size is "<<root->size<<endl;
	//cout<<"root type: "<<root->type<<endl;
	int i=0;
	
	while(root->type!= T_LEAF){
	//	cout<<"finding \n";
//		cout<<"size is:"<<root->size<<endl;
//		cout<<"largest index is: "<<root->index[ root->size  ]<<endl;
		if(id > root->index[ root->size-1  ]){
			cout<<"if clause \n";
			root=(Node*)(root->child[ root->size+1 ]);
		
		}
		else{
//			cout<<"else clasue \n";
			for(i=0; i<root->size; i++){
				if(id<=root->index[i]){
					break;
				}
			}
			root=(Node*)(root->child[i]);
//			cout<<"else clause new id is "<<root->index[0]<<endl;
		}
	}
	//at leaf node
//	cout<<"at leaf node \n";	
	for(i=0; i<root->size;i++){
		if(root->index[i] == id){
			record r=*(record *)(root->child[i]);
			cout<<"find, its id and name are:"<<endl;
			cout<<r.id<<" " <<r.first_name<<" "<<r.last_name<<endl;

			return true;
		}
	
	}
	cout<<"didnt find \n";
	return false;
}


