#include "TaskQueue.h"
void TaskQueue::addTask(function<void(void)> task) {
    WaitForSingleObject(mutex, INFINITE);
    queue.push(task);
    ReleaseMutex(mutex);
}

bool TaskQueue::tryGetTask(function<void(void)>* task) {
    WaitForSingleObject(mutex, INFINITE);
    bool notEmpty = !queue.empty();
    if (notEmpty) {
        *task = queue.front();
        queue.pop();
    }
    else {
        *task = NULL;
    }
    ReleaseMutex(mutex);
    return notEmpty;
}

TaskQueue::TaskQueue() {
    mutex = CreateMutex(NULL, false, NULL);
}

TaskQueue::~TaskQueue() {
    CloseHandle(mutex);
}
