/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 2008-2012 by Eremex Ltd.                      */ 
/*                                                                        */ 
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

/* Include necessary files.  */

#include    "tm_api.h"
#include    <FXRTOS.h>

/* Define FXRTOS mapping constants.  */

#define TM_FXRTOS_MAX_THREADS          6
#define TM_FXRTOS_MAX_QUEUES           1
#define TM_FXRTOS_MAX_SEMAPHORES       1
#define TM_FXRTOS_MAX_MUTEXES          1

/* Define the default FX-RTOS stack size.  */

#define TM_FXRTOS_THREAD_STACK_SIZE    1024

/* Define the default FX-RTOS queue size.  */

#define TM_FXRTOS_QUEUE_SIZE           20

/* Define the number of timer interrupt ticks per second.  */

#define TM_FXRTOS_TICKS_PER_SECOND     1000

/* Define FX-RTOS data structures.  */

static fx_thread_t tm_thread_array[TM_FXRTOS_MAX_THREADS];
static fx_msgq_t   tm_queue_array[TM_FXRTOS_MAX_QUEUES];
static fx_sem_t    tm_semaphore_array[TM_FXRTOS_MAX_SEMAPHORES];
static fx_mutex_t  tm_mutex_array[TM_FXRTOS_MAX_MUTEXES];
fx_block_pool_t	   tm_block_pool[1];
uint32_t		   tm_pool_mem[256 + 4/ sizeof(uint32_t)];

/* Interrupt-related stuff. */

static fx_dpc_t    tm_dpc;

/* Define FX-RTOS object data areas.  */

char      tm_thread_stack_area[(TM_FXRTOS_MAX_THREADS*TM_FXRTOS_THREAD_STACK_SIZE)];
char      tm_queue_memory_area[(TM_FXRTOS_MAX_QUEUES*TM_FXRTOS_QUEUE_SIZE)];

/* Define array to remember the test entry function.  */

void      *tm_thread_entry_functions[TM_FXRTOS_MAX_THREADS];
void
fx_intr_handler(void)
{
    ;
}
extern void  tm_interrupt_handler(void);
/* This function is handler of DPC part of the interrupt. */
static void tm_dpc_handler(void* ignored, void* arg)
{
   //tm_interrupt_handler();
}

/* This function is called by HAL as interrupt handler. */
void tm_isr_handler(void)
{
    fx_dpc_request(&tm_dpc, tm_dpc_handler, NULL);
}

/* This function called from main performs basic RTOS initialization, 
   calls the test initialization function, and then starts the RTOS function.  */
void  tm_initialize(void (*test_initialization_function)(void))
{
    fx_sched_state_t state;
    fx_sched_lock(&state);

    fx_dpc_init(&tm_dpc);
    test_initialization_function();

    fx_sched_unlock(state);
}

/* This function takes a thread ID and priority and attempts to create the
   file in the underlying RTOS.  Valid priorities range from 1 through 31, 
   where 1 is the highest priority and 31 is the lowest. If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.   */
int  tm_thread_create(int thread_id, int priority, void (*entry_function)(void))
{
UINT    status;

    /* Create the thread under FX_RTOS.  */
    status =  fx_thread_init(
        &tm_thread_array[thread_id], 
        (void (*)(void*))entry_function, 
        (void*) thread_id, 
        priority,
        &tm_thread_stack_area[thread_id * TM_FXRTOS_THREAD_STACK_SIZE], 
        TM_FXRTOS_THREAD_STACK_SIZE, 
        true
    );

    /* Determine if the thread create was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function resumes the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_resume(int thread_id)
{
unsigned    prev;

    /* Attempt to resume the thread.  */
    prev =  fx_thread_resume(&tm_thread_array[thread_id]);

    /* Determine if the thread resume was successful.  */
    if (prev == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);   
}

/* This function suspends the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_suspend(int thread_id)
{
    /* Attempt to suspend the thread.  */
    int status = fx_thread_suspend();

    /* Determine if the thread suspend was successful.  */

    /*
    if (status == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
    */
}

/* This function relinquishes to other ready threads at the same
   priority.  */
void  tm_thread_relinquish(void)
{
    fx_thread_yield();
}

/* This function suspends the specified thread for the specified number
   of seconds. */
void tm_thread_sleep(int seconds)
{
    fx_thread_sleep(((UINT) seconds)*TM_FXRTOS_TICKS_PER_SECOND);
}

/* This function creates the specified queue.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_create(int queue_id)
{
UINT    status;

    /* Create the specified queue with 16-byte messages.  */
    status =  fx_msgq_init(&tm_queue_array[queue_id],  
                          (uintptr_t*)&tm_queue_memory_area[queue_id*TM_FXRTOS_QUEUE_SIZE], 
                          TM_FXRTOS_QUEUE_SIZE/sizeof(uintptr_t), FX_SYNC_POLICY_DEFAULT);

    /* Determine if the queue create was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function sends a 16-byte message to the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_send(int queue_id, unsigned long *message_ptr)
{
   UINT    status;
   status = fx_msgq_back_timedsend(&tm_queue_array[queue_id], message_ptr[0], 0);

    /* Determine if the queue send was successful.  */
    if (status == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);   
}

/* This function receives a 16-byte message from the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_receive(int queue_id, unsigned long *message_ptr)
{
   UINT    status;
   status = fx_msgq_timedreceive(&tm_queue_array[queue_id], (uintptr_t*) message_ptr, 0);
   
    /* Determine if the queue send was successful.  */
    if (status == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR); 
}

/* This function creates the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_create(int semaphore_id)
{
UINT    status;

    /*  Create semaphore.  */
    status =  fx_sem_init(&tm_semaphore_array[semaphore_id], 1, UINT32_MAX, FX_SYNC_POLICY_DEFAULT);

    /* Determine if the semaphore create was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function gets the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_get(int semaphore_id)
{
UINT    status;

    /*  Get the semaphore.  */
    status =  fx_sem_timedwait(&tm_semaphore_array[semaphore_id], 0);

    /* Determine if the semaphore get was successful.  */
    if (status == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_put(int semaphore_id)
{
UINT    status;

    /*  Get the semaphore.  */
    status =  fx_sem_post(&tm_semaphore_array[semaphore_id]);

    /* Determine if the semaphore get was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function creates the specified mutex.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_mutex_create(int mutex_id, int priority_ceiling)
{
UINT    status;
UINT    ceiling = FX_MUTEX_CEILING_DISABLED;

    if(priority_ceiling != 0)
    {
        ceiling = priority_ceiling;
    }

    /*  Create mutex.  */
    status =  fx_mutex_init(&tm_mutex_array[mutex_id], ceiling, FX_SYNC_POLICY_DEFAULT);

    /* Determine if the mutex create was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function gets the specified mutex.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int    tm_mutex_get(int mutex_id)
{
UINT    status;

    /*  Get the mutex.  */
    status =  fx_mutex_timedacquire(&tm_mutex_array[mutex_id], 0);

    /* Determine if the mutex get was successful.  */
    if (status == FX_THREAD_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function puts the specified mutex.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int    tm_mutex_put(int mutex_id)
{
UINT    status;

    /*  Put the mutex.  */
    status =  fx_mutex_release(&tm_mutex_array[mutex_id]);

    /* Determine if the semaphore get was successful.  */
    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function creates the specified memory pool that can support one or more
   allocations of 128 bytes.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int    tm_memory_pool_create(int pool_id)
{
	UINT	status;
    status = fx_block_pool_init(&tm_block_pool[pool_id], tm_pool_mem, sizeof(tm_pool_mem), 128, FX_SYNC_POLICY_FIFO);

    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function allocates a 128 byte block from the specified memory pool.
   If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR
   should be returned.  */
int    tm_memory_pool_allocate(int pool_id, unsigned char **memory_ptr)
{
	UINT status;
	status = fx_block_pool_alloc(&tm_block_pool[pool_id], memory_ptr, NULL);

	if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}

/* This function releases a previously allocated 128 byte block from the specified
   memory pool. If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR
   should be returned.  */
int    tm_memory_pool_deallocate(int pool_id, unsigned char *memory_ptr)
{
	UINT status;
	(void)pool_id;
	status = fx_block_pool_release(memory_ptr);

    if (status == FX_STATUS_OK)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}
