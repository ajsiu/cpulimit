#include <mach/host_priv.h>
#include <sys/time.h>
#include <mach/task.h>
#include <mach/processor_set.h>
#include <mach/mach_port.h>
#include <mach/task_info.h>
#include <mach/thread_act.h>
#include <mach/vm_map.h>
#include <mach/mach_host.h>
#include <signal.h>
#include <stdio.h>

//#include <mach/mach_init.h>

#ifndef __PROCESS_ITERATOR_H
#include "process_iterator.h"
#endif

#define toTimeVal(a, r) { (r)->tv_sec = (a)->seconds; \
    (r)->tv_usec = (a)->microseconds;}

/* Might not work on modern Os X
 * @param pid
 * @return pid's corresponing task port
 */
mach_port_t getTask(pid_t pid) {
    kern_return_t ret;
    task_array_t tasks;
    mach_msg_type_number_t taskCount;
    host_t host = mach_host_self();//needs root to work
    mach_port_t psDef;
    mach_port_t psControl;
    int foundPid;

    if ( (ret = processor_set_default( host, &psDef) ) != KERN_SUCCESS) goto fail;
    if ( (ret = host_processor_set_priv( host, psDef, &psControl )) != KERN_SUCCESS) goto fail;
    if ( (ret = processor_set_tasks( psControl, &tasks, &taskCount) ) != KERN_SUCCESS) goto fail;
    for (int i = 0; i < taskCount; i++) {
        pid_for_task( tasks[i], &foundPid );
        if (foundPid == pid) return tasks[i];
    }
    fprintf(stderr, "No matching pid found. Task must either not exist or be in different processor_set");
    return MACH_PORT_NULL;
fail:
    fprintf( stderr, "Failed with %d", ret);
    return MACH_PORT_NULL;//so we know it failed
}

//will write functions here and then merge this into the rest of the code.
//will also probably cache the results of get tasks. will need to change some structs for that.

//Consider making an alternative method that does it the traditional way if entitlment is provided.

//make cpu sampler that stuns proc, counts cpu usage of threads, and then conts and rets usage.



int getCPUTime( struct process * proc) {

    if (proc->task == MACH_PORT_NULL) proc->task = getTask(proc->pid);//add some redudancy to getTask

    //also need some bullshit with tsamp. tsamp is just dT
    kern_return_t ret;
    thread_array_t threads;
    mach_msg_type_number_t threadCount, count = THREAD_BASIC_INFO_COUNT;
    thread_basic_info_data_t threadInfo;

    struct timeval totalTime;
    totalTime.tv_sec = 0;
    totalTime.tv_usec = 0;

    //kill(pid, SIGSTOP);//stuns proc

    if ( (ret = task_threads( proc->task, &threads, &threadCount) ) != KERN_SUCCESS) {
        //do something
    }
    for ( int i = 0; i < threadCount; i++) {
        if ( (ret = thread_info( threads[i], THREAD_BASIC_INFO, (thread_info_t) &threadInfo, &count) ) != KERN_SUCCESS) continue;

        if ( (threadInfo.flags & TH_FLAGS_IDLE) == 0) {
            struct timeval tv;
            toTimeVal(&threadInfo.user_time, &tv);
            timeradd(&totalTime, &tv, &totalTime);
            toTimeVal(&threadInfo.system_time, &tv);
            timeradd(&totalTime, &tv, &totalTime);
            //add the thread info time to the proc time
        }

        if ( (ret = mach_port_deallocate( mach_task_self() , threads[i]) ) != KERN_SUCCESS) {
            //do something
        }
    }
    proc->total_time = totalTime;
    return 0;
}

double CPUUsage( struct process * proc1, struct process * proc2, struct timeval deltaT) {
    struct timeval usedTime;
    unsigned long long elapsed_us = 0, used_us = 0;
    int whole, part;
    timersub( &proc2->total_time, &proc1->total_time, &usedTime);//in some pref modes it's taking the start time as proc1->total_time
    elapsed_us = deltaT.tv_sec * 1000000ULL + deltaT.tv_usec;
    used_us = usedTime.tv_sec * 1000000ULL + usedTime.tv_usec;
    whole = (used_us * 100ULL) / elapsed_us;
    part = (((used_us * 100ULL) - (whole * elapsed_us)) * 10ULL) / elapsed_us;
    
    double usage = whole;
    return usage + ( (double) part / 100);//rework
    
}
//modify and work the functions into the rest of the program
