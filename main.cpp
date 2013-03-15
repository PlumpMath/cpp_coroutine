#include <iostream>

#include "Coroutine.h"
#include "CoChannel.h"

using namespace std;

void consumer(void* n)
{
    CoChannel* chan = (CoChannel*)n;

	  cout<<"consumer begin............."<<endl;   

    while(chan->recv())
    {
       int a =(int)chan->get_data();

       cout<<"rev:"<<a<<endl;
    } 
    
	  cout<<"consumer end............."<<endl; 
}

void producer(void* n)
{
    CoChannel* chan = (CoChannel*)n;

	  cout<<"producer begin............."<<endl;
	  
    for (int i = 0; i < 20; ++i)
    {
       cout<<"send i :"<< i <<endl;
       chan->send((void*)i);
    }

    chan->close();

    cout<<"producer end............."<<endl;
}

//make the compiler happy.... 
void run(void*)
{
     

   cout<<"in run"<<endl;   

   //******the channel will be released automatically by the receiver
   CoChannel* ch = CoChannel::create();
   //******the channel will be released automatically by the receiver
    
   Coroutine::go(consumer,(void*)ch,"co1");
   
   Coroutine::go(producer,(void*)ch,"co2");   

   cout<<"run finished"<<endl;
}

int main(){

    cout<<"Hello world!"<<endl;    
   
    Coroutine::launch_scheduler(run, (void*)NULL);

	  return 0;
}
