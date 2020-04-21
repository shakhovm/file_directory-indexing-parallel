#ifndef CONC_QUEUE_H
#define CONC_QUEUE_H


#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <iostream>

template <typename T>
class synch_queue {
private:
    std::deque<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv;
    std::condition_variable size_notifier;
    std::atomic_size_t counter = 0;
    std::atomic_size_t mul = -1;

    const size_t SIZE_BORDER = 1000000000;
public:
    std::atomic_size_t byte_size = 0;
    synch_queue() = default;
    ~synch_queue() = default;
    void push(const T& value) {

        std::unique_lock<std::mutex> lg{mutex_};

        size_notifier.wait(lg, [this]{return byte_size < SIZE_BORDER;});
        queue_.push_back(value);

        cv.notify_one();
    }

    void push(T&& value) {

            std::unique_lock<std::mutex> lg{mutex_};

            size_notifier.wait(lg, [this]{return byte_size < SIZE_BORDER;});
            queue_.push_back(value);


        cv.notify_one();
    }

    void increase_size(size_t size) {
        byte_size += size;
    }

    void descrease_size(size_t size) {
        byte_size -= size;

        size_notifier.notify_all();
    }

    template <typename ...Args>
    void emplace(Args&&... args) {
        {
            std::unique_lock<std::mutex> lg{mutex_};
            size_notifier.wait(lg, [this](){return byte_size < SIZE_BORDER;});
            queue_.emplace_back(args...);
        }

        cv.notify_one();
    }

    void set_mul(size_t value) {
        mul = value;
    }

    void increase_value() {
        counter++;
    }

    size_t get_counter() {
        std::lock_guard<std::mutex> lg{mutex_};
        return counter;
    }

    size_t get_mul() {
        std::lock_guard<std::mutex> lg{mutex_};
        return mul;
    }

    T pop() {
        std::unique_lock<std::mutex> ul{mutex_};
        cv.wait(ul, [this]{return !queue_.empty(); });
        T elem = queue_.front();
        queue_.pop_front();

        return elem;
    }

    bool empty() const {
        std::lock_guard lg{mutex_};
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard lg{mutex_};
        return queue_.size();
    }
};

#endif

