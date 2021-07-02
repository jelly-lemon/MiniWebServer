#pragma once

#include <thread>
#include <functional>
#include "HttpResponse.cpp"
#include "SyncQueue.cpp"
#include "Logger.cpp"

using namespace std;

/**
 * 对线程池封装成类
 */
class ThreadPool {
private:
    int poolSize;       // 线程池容量
    mutex m_mutex;
    int currentWorkerNumber;
    SyncQueue<SOCKET> taskQueue;  // 任务队列
    function<void()> onTaskFinishedCallback;

public:

    /**
     * 线程池初始化
     */
    explicit ThreadPool(int poolSize = 0):
            poolSize(poolSize), currentWorkerNumber(0), onTaskFinishedCallback(NULL){
        if (poolSize == 0) {
            this->poolSize = getCPULogicCoresNumber() + 1;
        }
    }

    bool submitTask(SOCKET connSocket);

    int createNewThread();

    SOCKET getTask();

    void onWorkerFinished();

    void setOnTaskFinishedCallback(function<void()> onTaskFinishedCallback);

    static void *worker_main(void *args);
};






/**
 * 子线程函数
 */
void *ThreadPool::worker_main(void *args) {
    //
    // 取任务并执行
    //
    auto *pThreadPool = (ThreadPool *)args;
    info(" new worker, currentWorkerNumber: %d\n", pThreadPool->currentWorkerNumber);
    while (true) {
        try {
            SOCKET connSocket = pThreadPool->getTask();
            HttpResponse response(connSocket);
            response.handleRequest();
            if (pThreadPool->onTaskFinishedCallback != NULL) {
                pThreadPool->onTaskFinishedCallback();
            }
        } catch(exception &e) {
            info("worker finished: %s\n", e.what());
            break;
        }
    }

    //
    // 退出线程
    //
    pThreadPool->onWorkerFinished();

    return NULL;
}

/**
 * 提交任务到队列
 *
 * @return: 提交成功或失败
 */
bool ThreadPool::submitTask(SOCKET connSocket) {
    if (taskQueue.put(connSocket)) {
        info(" task size: %d\n", taskQueue.getSize());
        lock_guard<mutex> guard(m_mutex);
        if (currentWorkerNumber < poolSize) {
            createNewThread();
            currentWorkerNumber++;
        }
        return true;
    }

    return false;
}

/**
 * 创建新线程
 */
int ThreadPool::createNewThread() {
    pthread_t worker;
    int rt = pthread_create(&worker, NULL, ThreadPool::worker_main, (void*)this);

    return rt;
}

/**
 * 获取任务
 */
SOCKET ThreadPool::getTask() {
    return taskQueue.get();
}

/**
 * 某个线程结束时，现有线程数量减 1
 */
void ThreadPool::onWorkerFinished() {
    lock_guard<std::mutex> guard(m_mutex);
    currentWorkerNumber--;
    info(" onWorkerFinished, currentWorkerNumber: %d\n", currentWorkerNumber);
}

void ThreadPool::setOnTaskFinishedCallback(function<void()> onTaskFinishedCallback) {
    this->onTaskFinishedCallback = onTaskFinishedCallback;
}











