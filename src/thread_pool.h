// Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
// @brief 

#ifndef APP_ECOM_ANTI_THEMIS_WORKER_WORKER_LIB_THREAD_THREAD_POOL_H
#define APP_ECOM_ANTI_THEMIS_WORKER_WORKER_LIB_THREAD_THREAD_POOL_H

#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <gflags/gflags.h>
#include "task_queue.h"

namespace anti {
namespace themis {
namespace common_lib {

class ThreadPool {
private:
    typedef std::unique_ptr<ThTask> TaskPtr;
public:
    ThreadPool(int32_t backend_threadnum);
    ~ThreadPool();

    template<typename T>
    bool add_task(T&& task) {
        return add_task(task, &_task_queue);
    }

    template<typename T>
    bool add_data_export_task(T&& task) {
        return add_task(task, &_data_out_queue);
    }

    template<typename T>
    bool add_task(T&& task, TaskQueue* queue) {
        std::unique_ptr<ThTask> obj;
        using def_type = typename std::remove_reference<T>::type;
        obj.reset(new def_type(std::move(task)));
        queue->produce(std::move(obj));
        return true;
    }

    void exit();
    static ThreadPool* instance();


private:
    void backend_run(TaskQueue* task_queue);

private:
    bool _init(int32_t backend_threadnum);
    TaskQueue _task_queue;
    std::vector<std::unique_ptr<std::thread>> _threads;

    // Special async thread only for output Data.
    // because most of data will be write into files.
    // and file only receive data one by one,
    // multi-thread write one file will cause problem.
    // Here using one special thread to output Data
    TaskQueue _data_out_queue;
    std::unique_ptr<std::thread> _write_thread;
};

template<typename T>
bool add_task(T&& task) {
    return ThreadPool::instance()->add_task(std::move(task));
}

template<typename T>
bool add_data_export_task(T&& task) {
    return ThreadPool::instance()->add_data_export_task(std::move(task));
}

template<typename T>
bool add_func(T&& func) {
    using def_type = typename std::remove_reference<T>::type;
    return add_task(SimpleTask<def_type>(std::move(func)));
}

template<typename T>
bool add_data_export_func(T&& func) {
    using def_type = typename std::remove_reference<T>::type;
    return add_data_export_task(SimpleTask<def_type>(std::move(func)));
}

}
}  // namespace themis
}  // namespace anti

#endif  // APP_ECOM_ANTI_THEMIS_WORKER_WORKER_LIB_THREAD_THREAD_POOL_H

// Codes are auto generated by God
