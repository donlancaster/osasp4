#include <queue>
#include <functional>
#include <Windows.h>
using namespace std;
class TaskQueue
{
private:
    queue<function<void(void)>> queue;
    HANDLE mutex;

public:
    void addTask(function<void(void)> task);
    bool tryGetTask(function<void(void)>* task);
    TaskQueue();
    ~TaskQueue();
};

