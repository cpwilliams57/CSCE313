
//************* CHANGES WILL BE MADE HERE ****************

#include "common.h"
#include "FIFOreqchannel.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdio.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));
    
    //Variables for time keeping
    struct timeval start1, end1, start2, end2, start3, end3;
    long microsec1,microsec2, microsec3, useconds, seconds;
    
    //establishing forking conditions
    int pid;
    pid = fork();
    
    if(pid == 0){ //run dataserver
        execlp("./dataserver",NULL);
    }
    
    else{ // run client
        
        //*********************** PART 1 ************************
        
        //connection to data server established
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        
        //create new excel file called x1
        ofstream x1;
        x1.open("received/x1.csv");

        //start timer
        gettimeofday(&start1, NULL);

        //get egc1 data
        double file_time = 0;
        for(int i = 0; i < 15000; i ++){
            //get egc1 data
            datamsg ecg1_data = datamsg(1,file_time,1);
            chan.cwrite((char*) &ecg1_data, sizeof(datamsg));
            double* ecg1_tmp = (double*) chan.cread();

            //get egc2 data
            datamsg ecg2_data = datamsg(1,file_time,2);
            chan.cwrite((char*) &ecg2_data, sizeof(datamsg));
            double* ecg2_tmp = (double*) chan.cread();

            //write to file
            x1 << file_time << "," << *ecg1_tmp << "," << *ecg2_tmp << "\n";
            file_time = file_time + .004;
        }

        //Close file
        x1.close();

        //end timer
        gettimeofday(&end1, NULL);
        seconds  = end1.tv_sec  - start1.tv_sec;
        useconds = end1.tv_usec - start1.tv_usec;
        microsec1 = 1000000 * seconds + useconds;

        //*********************** PART 2 ************************

        //Creating and opening y1 csv file
        ofstream y1;
        y1.open("received/y1.csv");

        //send message to getting the file length of 1.csv
        filemsg size_request = filemsg(0,0);
        //array of chars for the file name
        char name[]= "1.csv";

        //Getting size of file
        char messageBuffer[sizeof(filemsg) + strlen(name) + 1];
        memcpy(messageBuffer, &size_request, sizeof(size_request));

        //start timer
        gettimeofday(&start2, NULL);

        //pointing to begining of freespace and putting ame
        strcpy(messageBuffer+sizeof(filemsg),name);
        int res = chan.cwrite(messageBuffer, sizeof(filemsg) + strlen(name) + 1);
        __int64_t length = *(__int64_t*) chan.cread(&res);

        //Setting variables for loop
        int bytesrem = length;
        int offset = 0;
        int bytesBack;

        //Sending series of messages to get the remainder of the file
        while(bytesrem > 0){

            filemsg data_chunk_request = filemsg(offset,min(MAX_MESSAGE, bytesrem));
            char messageBuffer[sizeof(filemsg) + strlen(name) + 1];
            memcpy(messageBuffer, &data_chunk_request, sizeof(data_chunk_request));
            strcpy(messageBuffer+sizeof(filemsg),name);
            chan.cwrite(messageBuffer, sizeof(filemsg) + strlen(name) + 1);
            y1 << chan.cread(&bytesBack);
            bytesrem -= bytesBack;
            offset += bytesBack;
        }

        //Close the file
        y1.close();

        //end timer
        gettimeofday(&end2, NULL);
        seconds  = end2.tv_sec  - start2.tv_sec;
        useconds = end2.tv_usec - start2.tv_usec;
        microsec2 = 1000000 * seconds + useconds;

        //*********************** PART 3 ************************

        //verify that this is correct...
        FILE * f1;
        f1 = fopen("received/binary1m", "wb");

        //Parameters for the binry file
        char name_bin[]= "bin1m";
        filemsg size_request_bin = filemsg(0,0);
        
        
        //timer
        gettimeofday(&start3, NULL);

        
        //getting the length od the file
        char messageBuffer_bin[sizeof(filemsg) + strlen(name_bin) + 1];
        memcpy(messageBuffer_bin, &size_request_bin, sizeof(size_request_bin));
        strcpy(messageBuffer_bin+sizeof(filemsg),name_bin);
        int res_bin = chan.cwrite(messageBuffer_bin, sizeof(filemsg) + strlen(name_bin) + 1);
        __int64_t length_bin = *(__int64_t*) chan.cread(&res_bin);
        
        int _bytesrem = length_bin;
        int _offset = 0;
        int _bytesBack;

        //Sending series of messages to get the remainder of the file
        while(_bytesrem > 0){

            filemsg data_chunk_request = filemsg(_offset,min(MAX_MESSAGE, _bytesrem));
            char messageBuffer_bin[sizeof(filemsg) + strlen(name_bin) + 1];
            memcpy(messageBuffer_bin, &data_chunk_request, sizeof(data_chunk_request));
            strcpy(messageBuffer_bin+sizeof(filemsg),name_bin);
            chan.cwrite(messageBuffer_bin, sizeof(filemsg) + strlen(name_bin) + 1);
            fwrite(chan.cread(&_bytesBack),1,_bytesBack,f1);
            _bytesrem -= _bytesBack;
            _offset += _bytesBack;
        }

        fclose(f1);
        
        gettimeofday(&end3, NULL);
        seconds  = end3.tv_sec  - start3.tv_sec;
        useconds = end3.tv_usec - start3.tv_usec;
        microsec3 = 1000000 * seconds + useconds;

        //*********************** PART 4 ************************

        //Creating the new channel message
        channelmsg channel_msg;
        chan.cwrite((char*)&channel_msg, sizeof(channelmsg));
        string key = chan.cread();

        //Initialize the new channel
        FIFORequestChannel new_can (key, FIFORequestChannel::CLIENT_SIDE);

        //get the new data points
        datamsg new_point1 = datamsg(2,.004,1);
        chan.cwrite((char*) &new_point1, sizeof(datamsg));
        double* point1 = (double*) chan.cread();
        cout << "New Channel data point1: " << *point1 << endl;

        datamsg new_point2 = datamsg(2,.008,1);
        chan.cwrite((char*) &new_point2, sizeof(datamsg));
        double* point2 = (double*) chan.cread();
        cout << "New Channel data point2: " << *point2 << endl;


        //*********************** Quit Message  ************************
        
        //Send thte quit message
        quitmsg quit_msg;
        chan.cwrite((char*)&quit_msg, sizeof(quit_msg));
        wait(&pid);
    }
    
//    cout << "Data point transfer time in microseconds: " << microsec1 << endl;
//    cout << "File message transfer time  in microseconds: " << microsec2 << endl;
    cout << "Binary file transfer time in micro seconds: " << microsec3 << endl;

  
	return 0;
}

