// Copyright Take Vos 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <concepts>
#include <coroutine>
#include <type_traits>
#include <cassert>
#include <exception>
#include <iterator>
#include <string>
#include <utility>

namespace csp { inline namespace v1 {

/** A return value for a generator-function.
 * A generator-function is a coroutine which co_yields zero or more values.
 *
 * The generator object returned from the generator-function is used to retrieve
 * the yielded values through an forward-iterator returned by the
 * `begin()` and `end()` member functions.
 *
 * Incrementing the iterator will resume the generator-function until
 * the generator-function co_yields another value.
 */
template<typename T>
class generator {
public:
    using value_type = T;

    static_assert(not std::is_reference_v<value_type>);
    static_assert(not std::is_const_v<value_type>);

    class promise_type {
    public:
        generator get_return_object()
        {
            return generator{handle_type::from_promise(*this)};
        }

        value_type const& value() const noexcept
        {
            assert(_value_ptr != nullptr);
            return *_value_ptr;
        }

        static std::suspend_never initial_suspend() noexcept
        {
            return {};
        }

        static std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        std::suspend_always yield_value(value_type const &arg) noexcept
        {
            _value_ptr = std::addressof(arg);
            return {};
        }

        void return_void() noexcept {}

        // Disallow co_await in generator coroutines.
        void await_transform() = delete;

        void unhandled_exception() noexcept
        {
            _value_ptr = nullptr;
            _exception = std::current_exception();
        }

        void rethrow()
        {
            if (auto ptr = _exception) {
                _exception = nullptr;
                std::rethrow_exception(ptr);
            }
        }

    private:
        std::exception_ptr _exception = nullptr;
        value_type const *_value_ptr = nullptr;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    class value_proxy {
    public:
        value_proxy(value_type const& value) noexcept : _value(value) {}

        value_type const& operator*() const noexcept
        {
            return _value;
        }

    private:
        value_type _value;
    };

    /** A forward iterator which iterates through values co_yieled by the generator-function.
     */
    class const_iterator {
    public:
        using difference_type = ptrdiff_t;
        using decayed_value_type = std::decay_t<value_type>;
        using pointer = decayed_value_type const *;
        using reference = decayed_value_type const&;
        using iterator_category = std::input_iterator_tag;

        explicit const_iterator(handle_type coroutine) : _coroutine{coroutine} {}

        /** Resume the generator-function.
         */
        const_iterator& operator++()
        {
            _coroutine.resume();
            _coroutine.promise().rethrow();
            return *this;
        }

        value_proxy operator++(int)
        {
            auto tmp = value_proxy(_coroutine.promise().value());
            _coroutine.resume();
            _coroutine.promise().rethrow();
            return tmp;
        }

        /** Retrieve the value co_yielded by the generator-function.
         */
        decltype(auto) operator*() const
        {
            return _coroutine.promise().value();
        }

        pointer operator->() const noexcept
        {
            return std::addressof(_coroutine.promise().value());
        }

        [[nodiscard]] bool at_end() const noexcept
        {
            return not _coroutine or _coroutine.done();
        }

        /** Check if the generator-function has finished.
         */
        [[nodiscard]] bool operator==(std::default_sentinel_t) const noexcept
        {
            return at_end();
        }

    private:
        handle_type _coroutine;
    };

    explicit generator(handle_type coroutine) : _coroutine(coroutine) {}

    generator() = default;
    ~generator()
    {
        if (_coroutine) {
            _coroutine.destroy();
        }
    }

    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;

    generator(generator&& other) noexcept : _coroutine{std::exchange(other._coroutine, {})} {}

    generator& operator=(generator&& other) noexcept
    {
        hi_return_on_self_assignment(other);
        if (_coroutine) {
            _coroutine.destroy();
        }
        _coroutine = std::exchange(other._coroutine, {});
        return *this;
    }

    /** Start the generator-function and return an iterator.
     */
    const_iterator begin() const
    {
        return const_iterator{_coroutine};
    }

    /** Start the generator-function and return an iterator.
     */
    const_iterator cbegin() const
    {
        return const_iterator{_coroutine};
    }

    /** Return a sentinel for the iterator.
     */
    std::default_sentinel_t end() const
    {
        return {};
    }

    /** Return a sentinel for the iterator.
     */
    std::default_sentinel_t cend() const
    {
        return {};
    }

    template<typename TT>
    operator TT() const noexcept{
        TT ss;
        for (auto s : *this) {
            if constexpr(std::is_same_v<decltype(s), TT> == true){
                ss+=TT(s);
            }else if constexpr(std::is_same_v<std::string,decltype(s)> == true){
                ss+=TT::fromStdString(s);
            }
            else {
                ss+=TT::fromStdString(s.toStdString());
            }
        }
        return ss;
    }


private:
    handle_type _coroutine;
};

}} // namespace csp::v1
