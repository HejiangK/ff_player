//
// Created by ken on 2018/6/28.
//

#ifndef RTMPTOHLS_SYNC_QUEUE_H
#define RTMPTOHLS_SYNC_QUEUE_H


#include <mutex>
#include <list>
#include <condition_variable>

template <typename T> class KSyncQueue
{
public:
    KSyncQueue()
    {
        disposed = false;
    }

    virtual ~KSyncQueue()
    {
        terminal();

        clear();
    }

    void push(T t)
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        queue.push_back(t);

        condition.notify_all();
    }

    void remove(T t)
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        auto it = queue.begin();

        while (it != queue.end())
        {
            if(*it == t)
            {
                queue.erase(it);

                break;
            }

            it ++;
        }

    }

    T front(bool isSync = true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        T t = nullptr;

        if(queue.size() == 0 && isSync && !disposed)
            condition.wait(lock);

        if(queue.size() != 0)
        {
            t = queue.front();

            queue.pop_front();
        }

        return t;
    }

    T back(bool isSync = true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        T t = nullptr;

        if(queue.size() == 0 && isSync && !disposed)
            condition.wait(lock);

        if(queue.size() > 0)
        {
            t = queue.back();

            queue.pop_back();
        }


        return t;
    }

    T at(int index)
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        T t = nullptr;

        if(!queue.empty() && index < queue.size())
        {
            auto it = queue.begin();

            for(int i = 0; i < index; i ++)
            {
                it++;
            }

            t = *it;

        }

        return t;
    }

    int length()
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        return (int)queue.size();
    }

    void terminal()
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        disposed = true;

        condition.notify_all();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        queue.clear();
    }
private:

    std::list<T>            queue;

    std::mutex              queueMutex;

    std::condition_variable condition;

    bool                    disposed;

};


#endif //RTMPTOHLS_SYNC_QUEUE_H
