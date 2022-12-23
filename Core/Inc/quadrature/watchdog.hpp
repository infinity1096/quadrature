#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

using WatchDogCallback = void(*)(void);

class WatchDog{
    public:
    WatchDog(long threshold_ms = 1000) : watchdog_threshold_ms(threshold_ms) {}

    void setCallback(WatchDogCallback callback){
        callback_function = callback;
    }

    void feedWatchDog(long time_ms){
        last_watchdog_time = time_ms;
    }

    void checkWatchDog(long time_ms){
        if (time_ms - last_watchdog_time >= watchdog_threshold_ms){
            callback_function();
        }
    }

    private:
    long watchdog_threshold_ms = 1000;

    long last_watchdog_time = 0;
    WatchDogCallback callback_function = nullptr;
};

#endif