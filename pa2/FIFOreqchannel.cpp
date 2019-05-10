// **************** NO CHANGE NECESSARY ******************

#include "common.h"
#include "FIFOreqchannel.h"
using namespace std;

//*************************************************

FIFORequestChannel::FIFORequestChannel(const string _name, const Side _side) : my_name( _name), my_side(_side)
{
    //setting names of the pipes
	pipe1 = "fifo_" + my_name + "1";
	pipe2 = "fifo_" + my_name + "2";
    
    //read and write operations for the pipes, client and server side
	if (_side == SERVER_SIDE){
		wfd = open_pipe(pipe1, O_WRONLY);
		rfd = open_pipe(pipe2, O_RDONLY);
	}
	else{
        //if it is the client side pipe
		rfd = open_pipe(pipe1, O_RDONLY);
		wfd = open_pipe(pipe2, O_WRONLY);
		
	}
	
}
//*************************************************

FIFORequestChannel::~FIFORequestChannel()
{ 
	close(wfd);
	close(rfd);

	remove(pipe1.c_str());
	remove(pipe2.c_str());
}
//*************************************************

int FIFORequestChannel::open_pipe(string _pipe_name, int mode)
{
	mkfifo (_pipe_name.c_str (), 0600);
	int fd = open(_pipe_name.c_str(), mode);
	if (fd < 0){
		EXITONERROR(_pipe_name);
	}
	return fd;
}
//*************************************************

char* FIFORequestChannel::cread(int *len)
{
	char * buf = new char [MAX_MESSAGE];
	int length; 
	length = read(rfd, buf, MAX_MESSAGE);
	if (len)	// the caller wants to know the length
		*len = length;
	return buf;
}
//*************************************************

int FIFORequestChannel::cwrite(char* msg, int len)
{
	if (len > MAX_MESSAGE){
		EXITONERROR("cwrite");
	}
	if (write(wfd, msg, len) < 0){
		EXITONERROR("cwrite");
	}
	return len;
}

