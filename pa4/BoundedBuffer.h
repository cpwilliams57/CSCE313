#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <mutex>

using namespace std;

class BoundedBuffer
{
private:
  int cap;
    
public:
    
    pthread_mutex_t mtx;    //mutex
    pthread_cond_t prod_done, cons_done;     //condition variables?
    queue<vector<char>> safe_buffer;        //data structure for bounded buffer
    
    
    
    //*************************************************************
    //constructor
    BoundedBuffer(int _cap):safe_buffer(queue<vector<char>>()){
        pthread_mutex_init(&mtx, NULL);
        pthread_cond_init(&prod_done,NULL);
        pthread_cond_init(&cons_done,NULL);
        cap = _cap;
	}
    
    //*************************************************************
    //destructor
	~BoundedBuffer(){

	}
    
    //*************************************************************
    //size of bounded buffer if needed
    int size(){
        int size;
        pthread_mutex_lock(&mtx);
        size = safe_buffer.size();
        pthread_mutex_unlock(&mtx);
        return size;
    }

    //*************************************************************
    //push a char pointer of a certain length to the queue
	void push(char* data, int len){
        
        pthread_mutex_lock(&mtx);                    //lock
        vector<char> temp(data, data+len);           //temporary vector containing the data to be pushed to
        while(safe_buffer.size() == cap){            //condition, wait for consume to finish
            //cout<<"Buffer is full, waiting for worker threads to drain out request\n";
            pthread_cond_wait(&cons_done,&mtx);
        }
        safe_buffer.push(temp);
        //push element to the buffer
        pthread_cond_signal(&prod_done);             //signal
        pthread_mutex_unlock(&mtx);                  //unlock

        
	}
 
    //*************************************************************
    //return a vector of char pointers
	vector<char> pop(){

        pthread_mutex_lock(&mtx);                      //lock
         while(safe_buffer.size() == 0 ){              //condition, wait for production to finish
             pthread_cond_wait(&prod_done,&mtx);
         }
        vector<char> t = safe_buffer.front();          //get the front element of the vector
        safe_buffer.pop();                             //pop front element from the vector
        pthread_cond_signal(&cons_done);               //send signal that consuming is finished
        pthread_mutex_unlock(&mtx);                    //unlock
        return t;                                      //return the vector of chars
	}
};

#endif /* BoundedBuffer_ */
