#pragma once

#include <stdexcept>
#include <type_traits>

namespace opex {
    namespace traits {
        template <typename Func, typename... Arg>
        struct result_of {
            using type = decltype(std::declval<Func>()(std::declval<Arg>()...));
        };

        template <typename Func, typename... Arg>
        using result_of_t = typename result_of<Func, Arg...>::type;
    }

    template<typename ValueType, typename BaseExceptionType = std::exception>
    class result {
    public:
        using value_type = ValueType;
        using base_exception_type = BaseExceptionType;

        ~result() {
            using exception_ptr = std::exception_ptr;

            switch (m_type) {
                case Type::Value:
                    m_value.~value_type();
                    break;
                case Type::Exception:
                    m_exception.~exception_ptr();
                    break;
            }
        }

        result(result &&other) {
            switch (m_type = other.m_type) {
                case Type::Value:
                    new(&m_value) value_type{std::move(other.m_value)};
                    break;
                case Type::Exception:
                    new(&m_exception) std::exception_ptr{std::move(other.m_exception)};
                    break;
            }
        }

        result(const result &) = delete;

        explicit result(const value_type &value):
                m_value(value),
                m_type(Type::Value)
        {}

        explicit result(value_type &&value):
                m_value(std::move(value)),
                m_type(Type::Value)
        {}

        template<typename ExceptionType, typename std::enable_if<std::is_base_of<base_exception_type, ExceptionType>::value>::type * = nullptr>
        static result from_exception(ExceptionType &&exception) {
            return result{std::make_exception_ptr(std::forward<ExceptionType>(exception))};
        };

        template<typename ExceptionType, typename... ArgTypes, typename std::enable_if<std::is_base_of<base_exception_type, ExceptionType>::value>::type * = nullptr>
        static result make_exception(ArgTypes... args) {
            return from_exception(ExceptionType{std::forward<ArgTypes>(args)...});
        };

        template<typename Func>
        static result call(Func &&func) {
            try {
                return result{func()};
            } catch (const base_exception_type &) {
                return result{std::current_exception()};
            }
        }

        template<typename Func, typename ResultType = result<traits::result_of_t<Func, ValueType>, BaseExceptionType>>
        ResultType map(Func &&func) const& {
            return is_ok() ? ResultType{func(m_value)}
                           : ResultType{m_exception};
        };

        template<typename Func, typename ResultType = result<traits::result_of_t<Func, ValueType&&>, BaseExceptionType>>
        ResultType map(Func &&func) && {
            return is_ok() ? ResultType{func(std::move(m_value))}
                           : ResultType{std::move(m_exception)};
        };

        template<typename Func, typename ResultType = result<ValueType, decltype(std::declval<Func>()(std::declval<const BaseExceptionType&>()))>>
        ResultType map_err(Func &&func) const& {
            return is_ok() ? ResultType(m_value)
                           : ResultType::from_exception(err_visit(std::forward<Func>(func)));
        };

        template<typename Func, typename ResultType = result<ValueType, decltype(std::declval<Func>()(std::declval<BaseExceptionType&&>()))>>
        ResultType map_err(Func &&func) && {
            return is_ok() ? ResultType(std::move(m_value))
                           : ResultType::from_exception(std::move(*this).err_visit(std::forward<Func>(func)));
        };

        template<typename Func>
        auto err_visit(Func &&func) const& -> decltype(std::declval<Func>()(std::declval<const BaseExceptionType&>())) {
            if (!is_err())
                throw std::logic_error("err_visit can only be called on error'd instances");

            try {
                std::rethrow_exception(m_exception);
            } catch (const BaseExceptionType &exc) {
                return func(exc);
            }

            throw std::logic_error("BUG: We failed to catch our exception...");
        }

        template<typename Func>
        auto err_visit(Func &&func) && -> decltype(std::declval<Func>()(std::declval<BaseExceptionType&&>())) {
            if (!is_err())
                throw std::logic_error("err_visit can only be called on error'd instances");

            try {
                std::rethrow_exception(m_exception);
            } catch (BaseExceptionType &exc) {
                return func(std::move(exc));
            }

            throw std::logic_error("BUG: We failed to catch our exception...");
        }

        const ValueType& unwrap() const { throw_on_err(); return m_value; }
        ValueType& unwrap()             { throw_on_err(); return m_value; }

        bool is_ok() const noexcept  { return m_type == Type::Value; }
        bool is_err() const noexcept { return m_type == Type::Exception; }

        const ValueType* operator->() const { return &unwrap(); }
        ValueType* operator->()             { return &unwrap(); }
        const ValueType& operator*() const { return unwrap(); }
        ValueType& operator*()             { return unwrap(); }

        explicit operator bool() const noexcept { return is_ok(); }
        bool operator!() const noexcept         { return is_err(); }

        std::string what() const noexcept {
            try {
                throw_on_err();
                return {};
            }
            catch (const std::exception &e) { return e.what(); }
            catch (const std::string &s) { return s; }
            catch (const char *p) { return p; }
            catch (...) { return {}; }
        }

    private:
        explicit result(std::exception_ptr &&exception):
                m_exception(std::move(exception)),
                m_type(Type::Exception)
        {}

        explicit result(const std::exception_ptr &exception):
                m_exception(exception),
                m_type(Type::Exception)
        {}

        void throw_on_err() const {
            if (is_err())
                std::rethrow_exception(m_exception);
        }

    private:
        union {
            ValueType m_value;
            std::exception_ptr m_exception;
        };
        enum class Type {
            Value, Exception
        } m_type;

        template <typename T, typename E>
        friend class result;
    };


    template <typename BaseExceptionType = std::exception, typename Func,
            typename ValueType = traits::result_of_t<Func>>
    result<ValueType, BaseExceptionType> call(Func &&func) {
        return result<ValueType, BaseExceptionType>::call(std::forward<Func>(func));
    };
}
