#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <string.h>

template<typename T>
class Singleton{

public:

   static T* getInstance() {

   	  if(! instance_ ) {

   	  	instance_ = new T();
   	  }

   	  return instance_ ;
   }


private:
	
	Singleton(){

	}    
    
 	static T* instance_;
};

template<typename T>
T* Singleton<T>::instance_ = NULL;

#endif
