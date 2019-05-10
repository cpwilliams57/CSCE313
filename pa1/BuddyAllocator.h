 
#ifndef _BuddyAllocator_h_                  
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned int uint;


class BlockHeader{

	public: 
    bool is_free = 1;
		uint block_size = 0; 
		BlockHeader* next_head = nullptr;
};
//-----------------------------------------------------------------------
class LinkedList{

private:
	
    BlockHeader* LL_head = nullptr;//head of the list
    uint LL_size = 0;//varible  to store the size of the linked list block header

public:

    LinkedList(){
        LL_head = nullptr;
        LL_size = 0;
    }
//-----------------------------------------------------------------------
    ~LinkedList(){
        //temporary header for iteration set to linked list head
        BlockHeader* tmp_head = LL_head;
        
        //While tmp_head points to something go through list
        while(tmp_head != nullptr){
            BlockHeader* tmp_next = tmp_head -> next_head;
            delete tmp_head;
            tmp_head = tmp_next;
        }
    }
//-----------------------------------------------------------------------
    //member function for returning th header of the free list
    BlockHeader* get_LL_head(){
        return LL_head;
    }
//-----------------------------------------------------------------------
    //Member funtion for returning the size of the free list
    uint get_LL_size(){
        return LL_size;
    }
//-----------------------------------------------------------------------
//Working
	void insert (BlockHeader* new_block_header){
        //If Linked list is empty, insert element into first position of linked list
        if(LL_size < 1){
            LL_head = new_block_header;
            LL_size = 1;
        }
        //if the Linked List is not empty, insert the new block header at the beginning of the list
        else{
            //create a block head pointer to temporairly point to current block head
            BlockHeader* tmp_head = nullptr;
            //point the Link list head at the new block header
            tmp_head = LL_head;
            //Make the next of LL_head the previous head of the list
            LL_head = new_block_header;
            LL_head->next_head = tmp_head;
            //Increase the size of the LL_size variable by 1;
            LL_size = LL_size  + 1;
        }
	}
 //-----------------------------------------------------------------------
 //working
    //implementation of linked list remove based off of implementaion by Seth Barberee
	// removes a specified block header from the list
	void remove (BlockHeader* header_for_removal){
        //Want to know our position in the LL, whats ahead and whats behind
        BlockHeader* curr_head = LL_head;
        
        //Check to see if the head of the list is the head for removal
        if(curr_head == header_for_removal){
            if(curr_head->next_head!=nullptr){
                LL_head = curr_head -> next_head;
                LL_size = LL_size - 1;
            }
            else{
                //If the list is empty set the head to null
                LL_head = nullptr;
                LL_size = 0;
            }
        }
        else{
            //Iterate theough the list until we find the
            for(int i=0; i < LL_size; i++ ){
                //check to see if the head is found
                if(curr_head->next_head == header_for_removal){
                    if(curr_head->next_head->next_head != nullptr){
                        //if check to see if close to the end of the list 
                        curr_head->next_head = curr_head->next_head->next_head;
                        LL_size--;
                    }
                    else{
                        curr_head->next_head = nullptr;
                        LL_size--;
                    }
                }
                else{
                    curr_head = curr_head->next_head;
                }
            }
        }
    }
    
};

//-----------------------------------------------------------------------
class BuddyAllocator{
    
private:
    uint total_memory_pool = 0;//Initializing total memory to be used for allocation
    uint basic_block;//Initializing basic block size;
    uint num_block_sizes;//Initializing number of block sizes
    char* total_block = nullptr;//Initializing a pointer to the total memory block header
    vector<LinkedList*> free_list;

private:

	BlockHeader* getbuddy(BlockHeader * addr);
	bool arebuddies(BlockHeader* block1, BlockHeader* block2);
	BlockHeader* merge(BlockHeader* block1, BlockHeader* block2);
	BlockHeader* split(BlockHeader* block);
    uint find_fl_position(BlockHeader* b);
    

public:
	BuddyAllocator (uint _basic_block_size, uint _total_memory_length);
	~BuddyAllocator();
	char* alloc(int _length);
	int free(char* _a);
	void debug ();

};

#endif 
