#ifndef SAPPHIRE_TASK_POOL_H
#define SAPPHIRE_TASK_POOL_H

#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>

// Taken from:
// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

// Work is submitted to this pool of various thread workers
// TODO: Non STL-version (for ports?)
class WorkerPool {
public:
    using WorkerFunction = std::function<void()>;

protected:
    void worker_loop();

    std::vector<std::thread> worker_threads;
    std::queue<WorkerFunction> queued_funcs;

    std::mutex queue_mutex;
    std::condition_variable wait_condition;

    bool terminate = false;

public:
    WorkerPool(uint32_t requested_workers = -1);

    void submit_func(const WorkerFunction& func);
};

#endif
