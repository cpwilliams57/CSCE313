#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

//*************************************************************
//put all the arguments for the patient threads
class patient_thread_args{
public:
    BoundedBuffer* buffer;
    FIFORequestChannel* request_channel;
    int num_requests;
    int buffer_size;
    int ecg;
    int length_file;
    string file;
};

//*************************************************************
//put all the arguments for the wroker threads
class worker_thread_args{
public:
    BoundedBuffer* buffer;
    FIFORequestChannel* request_channel;
    HistogramCollection* hist_col;
    pthread_mutex_t w_lock;
    string file;
};

//*************************************************************
//Patient thread functionality
void * patient_thread_function(void *arg)
{
    
    //Recasting data from arg to be used;
    patient_thread_args* tmp = (patient_thread_args*) arg;
    int requests = tmp->num_requests;
    int _ecg = tmp->ecg;
    BoundedBuffer* _buffer = tmp->buffer;
    FIFORequestChannel* _chan = tmp->request_channel;
    string fname = tmp->file;
    
    if(fname == "NULL"){
    //REQUESTING DATA POINTS        
            double file_time = 0;
            for(int i = 0; i < requests; ++i){
                if(_ecg < 1) {
                        cerr << "creating datamsg on person " << _ecg << endl;
                }
                
                datamsg data_msg1 = datamsg(_ecg,file_time,1);      //message for first ecg column
                char* m1 = (char*) &data_msg1;
                _buffer->push(m1, sizeof(datamsg));                 //Store in the buffer
                file_time = file_time + .004;                       //increment time

            }
    }
    else{
        
//        cout << "file request" << endl;
//        //REQUESTING FILES
//        //Get the length of the file **********************************
//        //turn file name into a  usable form
//        filemsg size_request = filemsg(0,0);
//        char file_ar[fname.length()];
//        strcpy(file_ar, fname.c_str());
//
//        //getting the length of the file
//        char messageBuffer[sizeof(filemsg)+ strlen(file_ar) + 1];
//        memcpy(messageBuffer,&size_request,sizeof(size_request));
//        strcpy(messageBuffer+sizeof(file_ar),file_ar);
//        int res = _chan->cwrite(messageBuffer,sizeof(filemsg) + strlen(file_ar) + 1);
//        __int64_t length = *(__int64_t*)_chan->cread(&res);
//
//        //length of the file
////        int bytes_rem = length;
////        //*************************************************************
////        //Push file requests to the buffer
//
//
        
    }
    
}


//*************************************************************
//Functionality of the worker threads
void *worker_thread_function(void *arg)
{
    
    //Retrieve arguements from funciton input
    worker_thread_args* tmp = (worker_thread_args*) arg;
    BoundedBuffer* _buffer = tmp->buffer;
    FIFORequestChannel* w_chan = tmp->request_channel;
    HistogramCollection* data_col = tmp->hist_col;
    string fname = tmp->file;
    
    //cout << "getting data worker" << endl;
    
    if(fname == "NULL"){
        while(true){
            //REQUESTING DATA POINTS
            vector<char> _request1 = _buffer->pop();          //get request from buffer
            char* _request = _request1.data();
            
            if(_request1.size()==0){
                MESSAGE_TYPE q = QUIT_MSG;
                w_chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
                delete w_chan;
                break;
            }
            
            w_chan->cwrite(_request, sizeof(datamsg));          //send the response through the channel
            double* response = (double*)w_chan->cread();        //get response from server
            int patient = ((datamsg*)_request)->person;
            
            //Update histogram
            data_col->update(patient - 1, *response);

        }
    }
    else{
        
//        REQUESTING FILES
//        /*sends the request message through a data channel
//        receives a chunk of the file through a data channel
//        writes the received chunk to a file*/
//
//        //Open file to write to
//        FILE * f1;
//        f1 = fopen("responsefile","wb");
//
//        while(true){
//            //pop request from buffer
//            char* _request = _buffer->pop().data();
//
//            if(((quitmsg*)_request) ->mtype == QUIT_MSG){       //if it is a quit mesage, exit loop
//                break;
//            }
//
//            //write request to channel
//            w_chan->cwrite(_request, sizeof(filemsg));
//
//            //write response to the output file
//            fwrite(w_chan->cread(),1,MAX_MESSAGE,f1);
//        }
//
    }
    
}

//*************************************************************

int main(int argc, char *argv[]){
    //Setting default values for command line arguements
    int n = 100;                //number of requests per "patient"
    int p = 10;                 // number of patients [1,15]
    int w = 2;                //number of worker threads
    int b = 20; 	            // capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	    // capacity of the file buffer
    string f = "NULL";           //file name
    srand(time_t(NULL));
    
    //*************************************************************
    //reading arguements from the command line
    int c;
    while ((c = getopt (argc, argv, "n:p:w:b:m:f:")) != -1)
        switch (c)
    {
        
        case 'n':
            n = atoi(optarg);
            break;
        case 'p':
            p = atoi(optarg);
            break;
        case 'w':
            w = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 'm':
            m = atoi(optarg);
            break;
        case 'f':
            f = optarg;
            break;
        case '?':
            if (optopt == 'n' || optopt == 'p'|| optopt == 'w'|| optopt == 'b'|| optopt == 'm'|| optopt == 'f')
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
    
    //*************************************************************
    //running data server as a child process of client
    
    string size_string = to_string(m);
    
    int pid = fork();
    if (pid == 0){
        
        //passing m to the dataserver
        execl ("dataserver", "dataserver", size_string.c_str(),(char *)NULL);
        
    }
    
    //Create the requent channel
	FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
   
    //create the bounded buffer
    BoundedBuffer request_buffer(b);
	
	
	//Start timer
    struct timeval start, end;
    gettimeofday (&start, 0);

    //*************** PATIENT THREADS ****************************
    vector<pthread_t> request_threads(p,0);
    
    //creating all of the request threads
    if(f == "NULL"){
        //If a file name is not given
        for(int i = 0; i < p; i++){
            
            //set patient args
            patient_thread_args* request_args = new patient_thread_args();
            request_args->request_channel = chan;
            request_args->num_requests = n;
            request_args->buffer_size = b;
            request_args->buffer = &request_buffer;
            request_args->file = f;
            request_args->ecg = i + 1;
            
            //create patient threads and call patient thread function
            pthread_create(&request_threads[i], NULL, &patient_thread_function,(void*)request_args);
        }
    }
    else{
        
        //create a single request thread
//        patient_thread_args* request_args = new patient_thread_args();
//        request_args->num_requests = n;
//        request_args->buffer_size = b;
//        request_args->buffer = &request_buffer;
//        request_args->file = f;
//        request_args->ecg = 1;
//        pthread_create(&request_threads[0], NULL, &patient_thread_function,(void*)request_args);
//        
//        cout << "request thread created" << endl;

    }
    
    //********************* WORKER THREADS ************************
    //declare the histogram collectiojn
    //Create histogram collection
    HistogramCollection hc;
    
    //populate the histogram collection.
    for(int i = 0; i < p; i++){
        Histogram* hist = new Histogram(20,-4,2);
        hc.add(hist);
    }

    
    pthread_t worker_threads[w];
    

    //creating all of the worker threads
    for(int i = 0; i < w; i++){
        //create a new request channel for each worker thread
        channelmsg* chan_msg = new channelmsg;
        chan->cwrite((char*)chan_msg, sizeof(channelmsg));
        char* key = chan->cread();
        string k(key);
        FIFORequestChannel* worker_chan = new FIFORequestChannel(k,FIFORequestChannel::CLIENT_SIDE);
        
        //set worker args
        worker_thread_args* worker_args = new worker_thread_args();
        worker_args->request_channel = worker_chan;
        worker_args->buffer = &request_buffer;
        worker_args->hist_col = &hc;
        worker_args->file = f;
        worker_args->buffer = &request_buffer;

        //Create the threads and call worker thread funciton
        pthread_create(&worker_threads[i], NULL, &worker_thread_function,(void*)worker_args);
    }

    //*************************************************************
    //JOIN AND PUSH QUITS
    //Join all request threads
    for(int i = 0; i < p; i++ ){
        pthread_join(request_threads[i], NULL);
    }
    
    //push quit messages for all of the workers
    for(int i = 0; i < w; i++){
        //quitmsg quit_msg;
        //char* q = (char*) &quit_msg;
        request_buffer.push(NULL,0/* sizeof(quitmsg)*/);
    }
    
    //join all of the worker threads
    for(int i = 0; i < w; i++ ){
        pthread_join(worker_threads[i], NULL);
    }
    
    
    //*************************************************************

    //Ending timer and displaying time taken to the console
    gettimeofday (&end, 0);
	hc.print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    //Writing quit message to the channel
    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    //wait(NULL);
}


//read M every time...
//calculates total number of requests
//use fseek to move the pointer
//take file size and divide by 256
// use test
