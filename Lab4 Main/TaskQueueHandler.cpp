#include "TaskQueueHandler.h"

bool TaskQueueHandler::finished() {
    bool result;
    WaitForSingleObject(finishMutex, INFINITE);
    result = finish;
    ReleaseMutex(finishMutex);
    return result;
}

DWORD WINAPI TaskQueueHandler::threadProc(LPVOID lpParameter) {
    TaskQueueHandler* thisHandler = (TaskQueueHandler*)lpParameter;
    function<void(void)> task;
    while (thisHandler->tasks->tryGetTask(&task) || !thisHandler->finished()) {
        if (task != NULL) {
            task();
        }
    }
    return 0;
}
void TaskQueueHandler::startTasks(int numThreads) {
    for (int i = 0; i < numThreads; i++) {
        hThreads.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, this, NULL, NULL));
    }
}

void TaskQueueHandler::waitForComplition() {
    WaitForSingleObject(finishMutex, INFINITE);
    finish = true;
    ReleaseMutex(finishMutex);
    WaitForMultipleObjects(hThreads.size(), &hThreads[0], true, INFINITE);
}

TaskQueueHandler::TaskQueueHandler(TaskQueue* tasks) {
    this->tasks = tasks;
    finishMutex = CreateMutex(NULL, false, NULL);
}

TaskQueueHandler::~TaskQueueHandler() {
    for (auto it = hThreads.begin(); it != hThreads.end(); it++) {
        CloseHandle(*it);
    }
    CloseHandle(finishMutex);
}