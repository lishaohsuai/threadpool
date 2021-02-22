#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include "threadPool.hh"  
  
  
class MyTask: public zl::Task  
{  
public:  
    MyTask(){}

    virtual int run() {  
        printf("thread[%lu] : %d\n", pthread_self(), *(int*)this->arg_);  
        // sleep(1);
        const int MM = 100000;
        for(int i=0; i<MM; i++)
            for(int j=0; j<MM; j++);  
        return 0;  
    }
};  
  
int main() {  
    char szTmp[] = "hello world";
    std::vector<MyTask> taskObj;
    taskObj.resize(100);
    std::vector<int> vv(100);
    
    for(int i=0; i<100; i++){
        vv[i] = i;
        taskObj[i].setArg((void*)&vv[i]);
    }

    zl::ThreadPool threadPool(16);  
    for(int i = 0; i < 100; i++) {
        threadPool.addTask(&taskObj[i]);  
    }

    while(1) {  
        printf("there are still %d tasks need to process\n", threadPool.size());  
        if (threadPool.size() == 0)
        {  
            threadPool.stop();
            printf("Now I will exit from main\n"); 
            exit(0);   
        }  
        sleep(2);  
    }  

    return 0;
}  