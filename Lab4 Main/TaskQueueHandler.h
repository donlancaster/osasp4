#pragma once
#include "TaskQueue.h"
#include <vector>
using namespace std;
class TaskQueueHandler
{
private:
    TaskQueue* tasks;
    bool finish;
    HANDLE finishMutex;
    std::vector<HANDLE> hThreads;
    static DWORD WINAPI threadProc(LPVOID lpParameter);
    bool finished();
public:
    void startTasks(int numThreads);
    void waitForComplition();
    TaskQueueHandler(TaskQueue* tasks);
    ~TaskQueueHandler();
};

