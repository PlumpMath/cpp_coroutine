#ifndef __COCHANNEL_H__
#define __COCHANNEL_H__

#include <stdio.h>

class Coroutine;

class CoEventLoop;


class CoChannel
{
   public:
     
   	 static CoChannel* create(const std::string& name = "default_name");

   	 void send(void* data);

   	 void close();

   	 bool recv();

   	 void* get_data();

     std::string name();   	 	

   private:

       CoChannel(const std::string& name);

       //the receiver auto release this
       ~CoChannel();  
       
       struct ChannelData
       {
          bool  is_ok_;
    
          void* data_;    
       };

   	   void* recv_data_;

   	   ChannelData* chann_data_;

   	   bool first_call_recv_;
   	
       std::string name_;

       friend class Coroutine;
};

#endif