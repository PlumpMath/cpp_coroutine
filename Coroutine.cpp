#include <string.h>
#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>

#include "Coroutine.h"

using namespace std;


std::queue<Coroutine*> Coroutine::schedule_que_;  

Coroutine* Coroutine::current_co_ = NULL;  

ucontext_t Coroutine::scheduler_uctx_;

void Coroutine::startCb(unsigned int y, unsigned int x)
{
  unsigned long z;

  //decode the pointer
	z = x<<16;	
	z <<= 16;
	z |= y;

	Coroutine * co = (Coroutine*)z;
    
  //work
	co->cb_(co->cb_param_);    
    
  //The work is done,now exit
	Coroutine::current_co()->exit();
}


Coroutine::Coroutine(GenCb cb, void* param,  const std::string& name)
{
	  int stack_size = 1024*1024;

	  this->name_ = name;

    this->cb_ = cb ;

    this->is_done_ = false;
         
    this->cb_param_ =param ;

    this->stack_sp_ = new char[stack_size];

    memset(&this->uctx_, 0, sizeof this->uctx_);
   
    sigset_t zero;
    sigemptyset(&zero);
    sigprocmask(SIG_BLOCK, &zero, &this->uctx_.uc_sigmask);

    if(getcontext(&this->uctx_) < 0){
		   abort();
    }
   
    this->uctx_.uc_stack.ss_sp   = this->stack_sp_  + 64;
    this->uctx_.uc_stack.ss_size = stack_size - 64;
 
    unsigned long z = (ulong)this;
    unsigned int y = z;

    // handle undefined 32-bit shift case for 32-bit compilers
    // encode the pointer
    z >>= 16;
    unsigned int x = z>>16;
   
    makecontext(&this->uctx_, (void(*)())Coroutine::startCb, 2, y, x);
}

Coroutine::~Coroutine()
{
    delete this->recv_chan_;

    this->recv_chan_ = NULL;

    delete this->stack_sp_;

    this->stack_sp_ = NULL;
}

void Coroutine::en_schedule_que(Coroutine * co){
    
    schedule_que_.push(co);
}

Coroutine* Coroutine::de_schedule_que()
{
	  Coroutine* co = schedule_que_.front();

    schedule_que_.pop();

    return co;
}

void Coroutine::launch_scheduler(GenCb cb, void* param)
{
    go(cb,param,"bootstrap");

    while( ! schedule_que_.empty() )
    {
        Coroutine* co = de_schedule_que();
        
        //cout<<"------coroutine name:"<< co->name_ <<endl;

        current_co_ = co;

        swapcontext(&scheduler_uctx_, &current_co_->uctx_); 

        if(current_co_->is_done())
        {
        	cout<<"---- release co:"<<current_co_->name_<<endl;
          delete current_co_;
          current_co_ = NULL;
        }

        //cout<<"------sswapcontext back"<< endl;       
   }
}

Coroutine* Coroutine::go(GenCb cb, void* param,  const std::string& name)
{
   Coroutine* co = new Coroutine(cb, param, name); 

   co->ready();

   return co;
}

void Coroutine::ready()
{
	Coroutine::en_schedule_que(this);
}

void Coroutine::exit()
{
  this->is_done_ = true;

  this->swapctx();
}

Coroutine* Coroutine::current_co()
{
	return current_co_;
}

void Coroutine::set_data(void*data)
{
	this->data_ = data;
}

void* Coroutine::get_data()
{
	return this->data_;
}

bool Coroutine::is_done()
{
   return this->is_done_;
}

void Coroutine::swapctx()
{
   ucontext_t* from = &this->uctx_;
   
   ucontext_t* to = &scheduler_uctx_;

   if(swapcontext(from,to) < 0)
   {     
      assert(0);
   }
}

int Coroutine::yield()
{
   this->ready();

   this->swapctx();
   
   int n = schedule_que_.size();

   //printf("now n=%d",n);

   return n;
}

void Coroutine::set_recv_channel(CoChannel* chan)
{
 	this->recv_chan_ = chan;
}

CoChannel* Coroutine::get_recv_channel()
{
    return this->recv_chan_;
}
