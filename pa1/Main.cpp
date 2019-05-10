#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <cmath>

//Establishing sdefault values for block size and memory size
int DEFAULT_BLOCK_SIZE = 128;
int DEFAULT_MEM_SIZE = 128*1024*1024;//128MB

//Function to validate user inputs are powers of 2
int power_of_two(int user_input){
  int tmp; 
  tmp = pow(2, ceil(log(user_input)/log(2)));
  return tmp;  
}

//Main loop 
int main(int argc, char ** argv) {

  //Initializing basic_block_size and memory_length to defaults
  int tmp_basic_block_size = DEFAULT_BLOCK_SIZE;
  int tmp_memory_length = DEFAULT_MEM_SIZE;
    
  //Reading arguements from command line for basic_block_size and and mem_size 
  int c;

  while ((c = getopt (argc, argv, "b:s:")) != -1)
    switch (c)
      {
      case 'b':
        tmp_basic_block_size = atoi(optarg);
        break;
      case 's':
        tmp_memory_length = atoi(optarg);
        break;
      case '?':
        if (optopt == 'b' || optopt == 's')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  //validating user input for block size and memory length are powers of two
  int basic_block_size = power_of_two(tmp_basic_block_size);
  int memory_length = power_of_two(tmp_memory_length);

  // //printing basic_block_size and memory_length for debug purposes.
   cout << "basic_block_size: " << basic_block_size << " memory_length: " << memory_length << endl;

  // //create memory manager
  BuddyAllocator* allocator = new BuddyAllocator(basic_block_size, memory_length);
    
    
 //test memory manager
 Ackerman* am = new Ackerman ();
 am->test(allocator); // this is the full-fledged test.
  
    cout << "destroy called" << endl;
  // // destroy memory manager
   delete allocator;
}

