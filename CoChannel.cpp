#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include "CoChannel.h"
#include "Coroutine.h"


using namespace std;

CoChannel* CoChannel::create(const std::string& name)
{
	return new CoChannel(name);
}

CoChannel::CoChannel(const std::string& name)
{
   this->name_ = name;
   this->first_call_recv_ = true;
   this->recv_data_ = NULL;
   this->chann_data_=NULL;
}

CoChannel::~CoChannel()
{   
   //chann_data_ is releaed after each recv
   //recv_data_ is release by thg receiver if necessary,
   //the channel itself just as a data pipe.

   //there is nothing need to release.
   printf("...........CoChannel released..............................\n");
}

bool CoChannel::recv()
{
   if(this->first_call_recv_)
   {
      Coroutine::current_co()->set_recv_channel(this);

      this->first_call_recv_ = false;
   }      
   
   CoChannel* chann = Coroutine::current_co()->get_recv_channel();

   if(NULL != chann)
   {// current coroutine is a receiver
     
     //printf("this=%d,chann=%d\n",(int)this, (int)chann);
     
     assert(this == chann);

   	 while(NULL == this->chann_data_)
   	 {//
   	 	Coroutine::current_co()->yield();
   	 }
   }
   else
   {
   	 printf("One coroutine can be a sender or receiver, but can not be both\n");

   	 abort();
   }

   //reach here , means we have receved data in the user coroutine
   ChannelData* recvd = this->chann_data_;

   bool is_ok = recvd->is_ok_;

   this->recv_data_ = recvd->data_;

   delete recvd;

   this->chann_data_ = NULL;

   return is_ok;
}

void* CoChannel::get_data()
{
  return this->recv_data_ ;
}

std::string CoChannel::name()
{
   return this->name_;
}

void CoChannel::send(void* data)
{   
   CoChannel* chann = Coroutine::current_co()->get_recv_channel();
   
   if(NULL != chann)
   {//means current coroutine is a receiver
     printf("One coroutine can be a sender or receiver, but can not be both\n");

     abort();
   }

   ChannelData* send = new ChannelData();
   
   send->is_ok_ = true;
   
   send->data_ = data;  
   
   this->chann_data_ = send;
   
   Coroutine::current_co()->yield();  
}

void CoChannel::close()
{
    //will be released when recv
	  ChannelData* send = new ChannelData();
    
    send->is_ok_ = false;
   
    send->data_  = (void*)0;

    this->chann_data_ = send;
   
    Coroutine::current_co()->yield();  
}