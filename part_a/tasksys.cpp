#include "tasksys.h"
#include <thread>

IRunnable::~IRunnable() {}

ITaskSystem::ITaskSystem(int num_threads) {}
ITaskSystem::~ITaskSystem() {}

/*
 * ================================================================
 * Serial task system implementation
 * ================================================================
 */

const char* TaskSystemSerial::name() {
    return "Serial";
}

TaskSystemSerial::TaskSystemSerial(int num_threads): ITaskSystem(num_threads) {
}

TaskSystemSerial::~TaskSystemSerial() {}

void TaskSystemSerial::run(IRunnable* runnable, int num_total_tasks) {
    for (int i = 0; i < num_total_tasks; i++) {
        runnable->runTask(i, num_total_tasks);
    }
}

TaskID TaskSystemSerial::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                          const std::vector<TaskID>& deps) {
    // You do not need to implement this method.
    return 0;
}

void TaskSystemSerial::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelSpawn::name() {
    return "Parallel + Always Spawn";
}

TaskSystemParallelSpawn::TaskSystemParallelSpawn(int num_threads): ITaskSystem(num_threads) {
  //
  // TODO: CS149 student implementations may decide to perform setup
  // operations (such as thread pool construction) here.
  // Implementations are free to add new class member variables
  // (requiring changes to tasksys.h).
  //
  // register num_threads to the class
  TaskSystemParallelSpawn::num_threads = num_threads;
}

TaskSystemParallelSpawn::~TaskSystemParallelSpawn() {}

void func(IRunnable *runnable, int threadidx, int amount_of_work_per_thread,
          int num_total_tasks) {
  int start_work_idx = threadidx * amount_of_work_per_thread;
  for (int i = start_work_idx; i < start_work_idx + amount_of_work_per_thread;
       i++) {
    if (i >= num_total_tasks)
      break;
    runnable->runTask(i, num_total_tasks);
  }
}

void TaskSystemParallelSpawn::run(IRunnable* runnable, int num_total_tasks) {


    //
    // TODO: CS149 students will modify the implementation of this
    // method in Part A.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    /*for (int i = 0; i < num_total_tasks; i++) {
      runnable->runTask(i, num_total_tasks);
    }*/
    int num_threads = TaskSystemParallelSpawn::num_threads;
    std::thread thread_buf[num_threads];
    // split whole work to num_threads parts
    // assign the work to each thread & spawn them
    // thread 0: does work from 0 to num_total_tasks / num_threads - 1
    // thread 1: does work from num_total_tasks / num_threads to 2 *
    //            num_total_tasks/num_threads -1
    int amount_of_work_per_thread = num_total_tasks / num_threads;
    for (int i = 0; i < num_threads; i += 1) {
      // func: spawns indices, part of works to runnable->runTask(...)
      thread_buf[i] = std::thread(func, runnable, i, amount_of_work_per_thread,
                                  num_total_tasks);
    }
    // wait for return
    for (int i = 0; i < num_threads; i++) {
      thread_buf[i].join();
    }
}

TaskID TaskSystemParallelSpawn::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                                 const std::vector<TaskID>& deps) {
    // You do not need to implement this method.
    return 0;
}

void TaskSystemParallelSpawn::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Thread Pool Spinning Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelThreadPoolSpinning::name() {
    return "Parallel + Thread Pool + Spin";
}

TaskSystemParallelThreadPoolSpinning::TaskSystemParallelThreadPoolSpinning(int num_threads): ITaskSystem(num_threads) {
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
    TaskSystemParallelThreadPoolSpinning::num_threads = num_threads;
}

TaskSystemParallelThreadPoolSpinning::~TaskSystemParallelThreadPoolSpinning() {}

void func2(IRunnable *runnable, int startidx, int bound) {
  for (int i = startidx; i < bound; i++)
    runnable->runTask(i, bound);
}

void TaskSystemParallelThreadPoolSpinning::run(IRunnable *runnable,
                                               int num_total_tasks){

    //
    // TODO: CS149 students will modify the implementation of this
    // method in Part A.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    /*for (int i = 0; i < num_total_tasks; i++) {
        runnable->runTask(i, num_total_tasks);
    }*/
    // create threadpool
    int num_threads = TaskSystemParallelThreadPoolSpinning::num_threads;
    std::thread threadpool[num_threads];

    // assign the work: we decide to do it statically
    int work_to_do = num_total_tasks / num_threads;
    for (int i = 0; i < num_threads; i++) {
      // spawn threads
      int thread_start_idx = i * work_to_do;
      int bound = thread_start_idx + work_to_do;
      if (bound >= num_total_tasks)
        bound = num_total_tasks;
      threadpool[i] = std::thread(/*runnable->runTask*/ func2, runnable,
                                  thread_start_idx, bound);
    }
    // join the threads
    for (int i = 0; i < num_threads; i++)
      threadpool[i].join();
}

TaskID TaskSystemParallelThreadPoolSpinning::runAsyncWithDeps(
    IRunnable *runnable, int num_total_tasks, const std::vector<TaskID> &deps) {
  // You do not need to implement this method.
  return 0;
}

void TaskSystemParallelThreadPoolSpinning::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Thread Pool Sleeping Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelThreadPoolSleeping::name() {
    return "Parallel + Thread Pool + Sleep";
}

TaskSystemParallelThreadPoolSleeping::TaskSystemParallelThreadPoolSleeping(int num_threads): ITaskSystem(num_threads) {
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

TaskSystemParallelThreadPoolSleeping::~TaskSystemParallelThreadPoolSleeping() {
    //
    // TODO: CS149 student implementations may decide to perform cleanup
    // operations (such as thread pool shutdown construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

void TaskSystemParallelThreadPoolSleeping::run(IRunnable* runnable, int num_total_tasks) {


    //
    // TODO: CS149 students will modify the implementation of this
    // method in Parts A and B.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    for (int i = 0; i < num_total_tasks; i++) {
        runnable->runTask(i, num_total_tasks);
    }
}

TaskID TaskSystemParallelThreadPoolSleeping::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                                    const std::vector<TaskID>& deps) {


    //
    // TODO: CS149 students will implement this method in Part B.
    //

    return 0;
}

void TaskSystemParallelThreadPoolSleeping::sync() {

    //
    // TODO: CS149 students will modify the implementation of this method in Part B.
    //

    return;
}
