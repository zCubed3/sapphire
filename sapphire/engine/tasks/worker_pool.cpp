#include "worker_pool.hpp"

// Taken from:
// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

void WorkerPool::worker_loop() {
    while (!terminate) {
        WorkerFunction func;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            wait_condition.wait(lock, [this] {
                return !queued_funcs.empty() || terminate;
            });

            if (terminate) {
                break;
            }

            func = queued_funcs.front();
            queued_funcs.pop();
        }

        func();
    }
}

WorkerPool::WorkerPool(uint32_t requested_workers) {
    uint32_t actual_workers = requested_workers;

    if (requested_workers == 0) {
        actual_workers = -1;
    }

    if (std::thread::hardware_concurrency() < requested_workers) {
        actual_workers = std::thread::hardware_concurrency();
    }

    worker_threads.resize(actual_workers);
    for (uint32_t w = 0; w < actual_workers; w++) {
        worker_threads[w] = std::thread(&WorkerPool::worker_loop, this);
    }
}

void WorkerPool::submit_func(const WorkerFunction &func) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        queued_funcs.push(func);
    }

    wait_condition.notify_one();
}