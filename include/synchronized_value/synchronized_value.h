#pragma once

#include <mutex>
#include <type_traits>

template<typename ...>
using void_t = void;

template<typename T, typename Tuple, typename = void_t<>>
struct is_aggregate_constructable_ : std::false_type
{
};

template<typename T, typename ... Args>
struct is_aggregate_constructable_<T,
        std::tuple<Args...>,
        void_t<decltype(T{Args()...})>> : std::true_type
{
};

template<typename T, typename ... Args>
struct is_aggregate_constructable :
        is_aggregate_constructable_<T, std::tuple<Args...>>
{
};

// TODO: add timer?
template<typename T, typename Mutex = std::mutex>
class xlock
{
public:
    using value_type = std::conditional_t<std::is_const<T>::value, const T, T>;
    using mutex_type = Mutex;

    xlock(value_type &refValue, Mutex &mutex)
            : refValue_(refValue),
              uniqueLock_(mutex)
    {}

    xlock(const xlock &other) = delete;

    xlock(xlock &&other) = delete;

    xlock &operator=(const xlock &other) = delete;

    xlock &operator=(xlock &&other) = delete;

    value_type *operator->()
    { return &refValue_; }

    const value_type *operator->() const
    { return &refValue_; }

    value_type &operator*()
    { return refValue_; }

    const value_type &operator*() const
    { return refValue_; }

    operator value_type &()
    { return refValue_; }

    operator const value_type &() const
    { return refValue_; }

    ~xlock()
    {

    }

private:
    value_type &refValue_;
    std::unique_lock<mutex_type> uniqueLock_;
};

template<typename T, typename Mutex = std::mutex>
class synchronized_value
{
public:
    using value_type = T;
    using mutex_type = Mutex;
    using xlock_t = xlock<value_type, mutex_type>;
    using cxlock_t = xlock<const value_type, mutex_type>;

    template<typename =
    std::enable_if_t<std::is_default_constructible<T>::value>>
    synchronized_value()
    {};

    explicit synchronized_value(T value)
            : value_(std::forward<T>(value))
    {}
    synchronized_value(const synchronized_value &) = delete;
    synchronized_value &operator=(const synchronized_value &) = delete;

    synchronized_value(synchronized_value &&other) = delete;
    synchronized_value &operator=(synchronized_value &&other) = delete;

    xlock_t operator*()
    { return {value_, mutex_}; }

    cxlock_t operator*() const
    { return {value_, mutex_}; }

    xlock_t operator->()
    { return {value_, mutex_}; }

    cxlock_t operator->() const
    { return {value_, mutex_}; }

    value_type &value()
    { return value_; }

    const value_type& value() const
    { return value_; }

private:
    value_type value_;
    mutable mutex_type mutex_;
};
