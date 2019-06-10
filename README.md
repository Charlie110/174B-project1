# 174B-project
Fuheng Zhao

In order to run my project simply type make and then ./main.


Inside the command line you will have 3 options, search, insert ot quit;

Before this operations, the data from sorterdSampledata.csv is already saved inside the disk and I already created a B+ Tree;

To search, you need to give an id number; 

To insert, you need to provide some information about that record; 

To quit, it will free the tree and the disk space. 


In my implementaion, I only keep the root node inside my main memory. Whenever you need to insert or lookup, I will save
the traversed path inside the buffer. The buffer is like a queue. 
To save the visited node, I will do push.

To get previous node(parent of current node), I will do pop.

After each insert or search, I will only keep the root node and clear the buffer since I don't really need these nodes anymore.

Moreover, I kept the disk page pointer inside my memory. Each pointer is only 4 bytes and the total page used is about 2000 and thus 8000 bytes which is about 2 page is saved in memory. I can do something like a FAT table or Inode table to keep track this page address. However, since the needed memory is small and limited time, I decided to simply same them in a vector. 


For Bulkloading, I did this step in init() function. In my implementation I kept a sparse tree in which each node has 
the minimum child required. 
Algorithm:
1st sort the data record.(given sorted data);
2nd allocate empty page to serve as root and insert reocords into index;
3rd when root is at minimum required index, create a new root page;
4th insert to the rightmost index page, till all entries are indexed;
