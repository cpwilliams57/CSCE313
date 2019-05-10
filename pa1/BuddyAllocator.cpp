#include "BuddyAllocator.h"
#include <iostream>
#include <cmath>
using namespace std;

//Working
BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    //temp variables
    uint temp_mem_pool = _total_memory_length;
    uint temp_basic_block_size = _basic_block_size;
    //total_block points to an array of chars becauseevery char is 1 byte...every char* is 8 bytes
    total_block = new char[temp_mem_pool];
    //Find the number of different block sizes that will be in total memory
    num_block_sizes = log2(temp_mem_pool) - log2(temp_basic_block_size) + 1;
    cout << "numblock sizes: "<<num_block_sizes << endl;
    //cout << "number of different block sizes: " << num_block_sizes << endl << endl;
    BlockHeader* total_block_head = (BlockHeader*) total_block;
    total_block_head->block_size = temp_mem_pool;
    total_block_head->is_free = true;
    total_block_head->next_head = nullptr;
    //Initializing our free list and allocating memory
    for(int i = 0; i < num_block_sizes; i++){
        LinkedList* tmp = new LinkedList();
        free_list.push_back(tmp);
        //cout << "LL push" << endl;
    }
    //putting total block size in last element of the linked list
    free_list[num_block_sizes - 1]->insert(total_block_head);
    total_memory_pool = temp_mem_pool;
    basic_block = temp_basic_block_size;
}
//-----------------------------------------------------------------------

//Buddy Allocator Destructor
BuddyAllocator::~BuddyAllocator(){
    //declaring vector on stack to deallocate memory
    vector<LinkedList*> free_list;
    
    //possible memory leakd
    
    delete[] total_block;
    for (..)
}
//-----------------------------------------------------------------------
//Working
BlockHeader* BuddyAllocator::getbuddy(BlockHeader* find_buddy){
    //Converting given blockhead pointer to a char* so math can be performed
    char* find_my_buddy = (char*) find_buddy;
    //Finding buddy
    char* buddy_location = (char*)(((find_my_buddy-total_block) ^ find_buddy->block_size) + total_block);
    //Returning buddy as a BlockHeader
    BlockHeader* buddy = (BlockHeader*) buddy_location;
    return buddy;
}
//-----------------------------------------------------------------------
//Working
bool BuddyAllocator::arebuddies (BlockHeader* block1, BlockHeader* block2){
    //Temporary Holders
    BlockHeader* tmp1 = nullptr;
    BlockHeader* tmp2 = nullptr;
    //Check to see which is lowest in the address space
    if(block1 >block2){
        tmp1 = block2;
        tmp2 = block1;
    }
    else{
        tmp1 = block1;
        tmp2 = block2;
    }
    //Checking buddy conditions
    if((getbuddy(tmp1) == tmp2)){
        return true;
    }
    else{
        return false;
    }
}
//-----------------------------------------------------------------------
//Working
BlockHeader* BuddyAllocator::merge(BlockHeader* block1, BlockHeader* block2){
    //Given two addresses that are free, update the header for the one with the lower address
    BlockHeader* tmp1 = block1;
    BlockHeader* tmp2 = block2;
    //Checking for the lowest address
    if(block1 > block2){
        tmp1 = block2;
        tmp2 = block1;
    }
    //Locate position of block1 in free list
    uint free_list_position = log2(tmp1->block_size) - log2(basic_block);
    //adjust the block size of tmp1
    tmp1->block_size = tmp1 -> block_size *2;
    //Remove pointers to the merged blocks from
    free_list[free_list_position]->remove(tmp2);
    free_list[free_list_position]->remove(tmp1);
    //Inserting the merged block into the free list at a higher position
    free_list[free_list_position + 1]->insert(tmp1);
    //return address of the merged block
    return block1 < block2 ? block1 : block2;
}
//-----------------------------------------------------------------------
//Working
BlockHeader* BuddyAllocator::split(BlockHeader* block){
    
    //creating temprary headers for assignment and data manipulation
    BlockHeader* block1 = nullptr ;
    BlockHeader* block2 = nullptr;
    //Setting tmp 1 equal to the beginning of the block
    block1 = (BlockHeader*) block;
    //finding current position within the free list
    uint fl_position = log2(block1->block_size) - log2(basic_block);
    //Removing the block to be split from the linked list
    free_list[fl_position]->remove(block1);
    //Setting block size to 1/2
    block1->block_size = block1->block_size/2;
    //Temporary pointer to specific location in the memory
    char* half_block = (char*) block;
    //Pointing half block to the middle of the block
    half_block = half_block + block1-> block_size;
    //Setting temp 2 to be pointer to half block as a block heder
    block2 = (BlockHeader*)half_block;
    //Setting sizes og block 1 and block2 equal
    block2->block_size = block1->block_size;
    block2->is_free = true;
    block1->is_free = true;
    //Inserting the split blocks into the appropriate place in the free list
    free_list[fl_position-1]->insert(block2);
    free_list[fl_position-1]->insert(block1);
    return block;
}
//-----------------------------------------------------------------------

//working
char* BuddyAllocator::alloc(int _length) {
    //Find length including the block header
    uint needed_length = _length + sizeof(BlockHeader);
    //Find closest power of 2
    needed_length = pow(2,ceil(log(needed_length)/log(2)));
    // IF the needed length is less than the basic block, set it equal to basic block
    if(needed_length < basic_block){
        needed_length = basic_block;
    }
    //Return a null ptr if there is a request that is too big
    if(needed_length > total_memory_pool){
        cout << "not enough mem" << endl;
        return nullptr;
    }
    //find closest block size
    uint free_list_pos = log2(needed_length) - log2(basic_block);
    BlockHeader* allocated_block;
    
    // if free list of size actual length is not empty, allocate and remove from list
    if(free_list[free_list_pos]->get_LL_size() > 0){
        allocated_block = free_list[free_list_pos]->get_LL_head();
        allocated_block->is_free = false;
        free_list[free_list_pos]->remove(allocated_block);
    }
    //We need to find a bigger block for allocation
    else{
        //look for a bigger free block
        free_list_pos = free_list_pos + 1;
        bool found_block = false;
        //while we havent found a good block and not at end of free_list
        while((found_block == false) && (free_list_pos < num_block_sizes)){
            //Check to see if there is a free block
            if(free_list[free_list_pos]->get_LL_size() > 0){
                found_block = true;
            }
            else{
                //Increase position in free list
                free_list_pos = free_list_pos+1;
            }
        }
        //If we couldnt find a block, return no free blocks
        if((found_block) == false){
            cout << "No mem left" << endl;
            return nullptr;
        }
        else{
            //recursively split block down to block size
            BlockHeader* splitting_block = free_list[free_list_pos]->get_LL_head();
            uint size_of_split = splitting_block->block_size;
            while((size_of_split > needed_length) && (free_list_pos > 0)){
                splitting_block = split(splitting_block);
                size_of_split = splitting_block->block_size;
                free_list_pos = free_list_pos - 1;
            }
        }
        //return allocated block
        allocated_block = free_list[free_list_pos]->get_LL_head();
        allocated_block->is_free = false;
        free_list[free_list_pos]->remove(allocated_block);
    }
    //add size of block header
    return (char*)allocated_block+ sizeof(BlockHeader);
}
//-----------------------------------------------------------------------

int BuddyAllocator::free(char* _a) {
    
    //locate header of block
    char* char_head_pointer = nullptr;
    char_head_pointer = _a - sizeof(BlockHeader);
    
    //Initialaizing variables for merge
    BlockHeader* head_pointer = (BlockHeader*) char_head_pointer;
    head_pointer->is_free = true;
    uint fl_pos = find_fl_position(head_pointer);
    BlockHeader* buddy_pointer = getbuddy(head_pointer);
    
    //establishing conditions for merging
    bool in_free_list = fl_pos < (num_block_sizes - 1);
    bool buddy_is_free = buddy_pointer -> is_free;
    
    //Recursively merge blocks until they cant be merged
    //While the linked list in free_list is not empty and we arent in the last fl position...
    while(in_free_list && buddy_is_free){
        //merge the muddies and get the new buddies
        head_pointer = merge(head_pointer, buddy_pointer);
        buddy_pointer = getbuddy(head_pointer);
        
        //New conditions for merge
        fl_pos = fl_pos+1;
        in_free_list = fl_pos < (num_block_sizes - 1);
        if(!in_free_list)
            break;
        buddy_is_free = buddy_pointer -> is_free;
    }
    head_pointer->is_free = true;
    return 0;
}

//-----------------------------------------------------------------------


void BuddyAllocator::debug (){
    //implement as needed
}

//-----------------------------------------------------------------------

uint BuddyAllocator::find_fl_position(BlockHeader* b){
    uint position = log2(b->block_size)-log2(basic_block);
    return position;
}
