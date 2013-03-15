#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <ucontext.h>
#include <queue>
#include <string>

#include "CoChannel.h"

typedef void(* GenCb)(void*);

class Coroutine
{
    public:
	   
      static Coroutine* go(GenCb cb, void* param, const std::string& name);

      static Coroutine* current_co();

      static void launch_scheduler(GenCb cb, void* param);      

      void set_data(void*data);

      void* get_data();           

      int  yield();

      void exit(); 

      std::string name();      

   private:

      static void startCb(unsigned int y, unsigned int x);      
      
      static void en_schedule_que(Coroutine * co);

      static Coroutine* de_schedule_que();

      Coroutine(GenCb cb, void* param, const std::string& name);
      
      ~Coroutine();

      bool is_done();

      void ready();

      void swapctx();

      void set_recv_channel(CoChannel* chan);

      CoChannel* get_recv_channel();
      
      std::string name_; 

      GenCb cb_ ;
            
      void* cb_param_ ;      

      char* stack_sp_;

      ucontext_t uctx_;     

      bool is_done_;

      void* data_;

      void* data2_;   

      CoChannel* recv_chan_;

      static std::queue<Coroutine*> schedule_que_;  

      static Coroutine* current_co_;  

      static ucontext_t scheduler_uctx_;

      friend class CoChannel;

};

#endif