#pragma once

#include <stdexcept>
#include <type_traits>


#ifdef __has_cpp_attribute
#  if __has_cpp_attribute(nodiscard)
#    define NODISCARD [[nodiscard]]
#  else
#    define NODISCARD
#  endif
#endif


namespace opex {
    namespace traits {
        template<typename... Ts> struct make_void { typedef void type;};
        template<typename... Ts> using void_t = typename make_void<Ts...>::type;

        template<typename>
        struct is_result : public std::false_type {};
    }

    template<typename ValueType, typename ExceptionType = std::exception>
    class NODISCARD result {
    public:
        using value_type = ValueType;
        using exception_type = ExceptionType;

        template <typename E, bool = std::is_base_of<ExceptionType, E>::value>
        struct is_allowed_exception : std::false_type {};

        template <typename E>
        struct is_allowed_exception<E, true> : std::true_type {};

        template <typename, typename = traits::void_t<>>
        struct rebind {};

        template <typename F, typename V>
        struct rebind<F(V), traits::void_t<typename std::result_of<F(V)>::type>> {
            using type = result<typename std::result_of<F(V)>::type, ExceptionType>;
        };

        template <typename, typename = traits::void_t<>>
        struct rebind_err {};

        template <typename F, typename E>
        struct rebind_err<F(E), traits::void_t<typename std::result_of<F(E)>::type>> {
            using type = result<ValueType, typename std::result_of<F(E)>::type>;
        };

        template <typename, typename = traits::void_t<>>
        struct compatible_result_of {};

        template <typename F, typename Arg>
        struct compatible_result_of<F(Arg), traits::void_t<typename std::enable_if<
                traits::is_result<typename std::result_of<F(Arg)>::type>::value &&
                std::is_base_of<typename std::result_of<F(Arg)>::type::exception_type, ExceptionType>::value>::type>> {
            using type = typename std::enable_if<
                    traits::is_result<typename std::result_of<F(Arg)>::type>::value &&
                    std::is_base_of<typename std::result_of<F(Arg)>::type::exception_type, ExceptionType>::value,
                    typename std::result_of<F(Arg)>::type
            >::type;
        };

        ~result() {
            using exception_ptr = std::exception_ptr;

            switch (m_type) {
                case Type::Value:
                    m_value.~ValueType();
                    break;
                case Type::Exception:
                    m_exception.~exception_ptr();
                    break;
            }
        }

        result(result &&other) {
            switch (m_type = other.m_type) {
                case Type::Value:
                    new(&m_value) ValueType{std::move(other.m_value)};
                    break;
                case Type::Exception:
                    new(&m_exception) std::exception_ptr{std::move(other.m_exception)};
                    break;
            }
        }

        result(const result &) = delete;

        explicit result(const ValueType &value):
                m_value(value),
                m_type(Type::Value)
        {}

        explicit result(ValueType &&value):
                m_value(std::move(value)),
                m_type(Type::Value)
        {}

        template<typename NewExceptionType,
                 typename std::enable_if<is_allowed_exception<NewExceptionType>::value>::type* = nullptr>
        static result from_exception(NewExceptionType &&exception) {
            return result{std::make_exception_ptr(std::forward<NewExceptionType>(exception))};
        };

        template<typename NewExceptionType,
                 typename... ArgTypes,
                 typename std::enable_if<is_allowed_exception<NewExceptionType>::value>::type* = nullptr>
        static result make_exception(ArgTypes... args) {
            return from_exception(NewExceptionType{std::forward<ArgTypes>(args)...});
        };

        template<typename Func>
        static result call(Func &&func) {
            try {
                return result{func()};
            } catch (const ExceptionType &) {
                return result{std::current_exception()};
            }
        }

        template<typename Func,
                 typename ResultType = typename rebind<Func(const ValueType &)>::type>
        ResultType map(Func &&func) const& {
            return is_ok() ? ResultType{func(m_value)}
                           : ResultType{m_exception};
        };

        template<typename Func,
                 typename ResultType = typename rebind<Func(ValueType &&)>::type>
        ResultType map(Func &&func) && {
            return is_ok() ? ResultType{func(std::move(m_value))}
                           : ResultType{std::move(m_exception)};
        };

        template<typename Func,
                 typename ResultType = typename rebind_err<Func(const ExceptionType &)>::type>
        ResultType map_err(Func &&func) const& {
            return is_ok() ? ResultType(m_value)
                           : ResultType::from_exception(err_visit(std::forward<Func>(func)));
        };

        template<typename Func,
                 typename ResultType = typename rebind_err<Func(ExceptionType &&)>::type>
        ResultType map_err(Func &&func) && {
            return is_ok() ? ResultType(std::move(m_value))
                           : ResultType::from_exception(std::move(*this).err_visit(std::forward<Func>(func)));
        };

        const result& and_select(const result &other ) const& { return is_ok() ? other : *this; }
              result& and_select(      result &other ) &      { return is_ok() ? other : *this; }
              result  and_select(      result &&other) &&     { return is_ok() ? std::move(other) : std::move(*this); }

        const result& or_select(const result &other ) const& { return is_err() ? other : *this; }
              result& or_select(      result &other ) &      { return is_err() ? other : *this; }
              result  or_select(      result &&other) &&     { return is_err() ? std::move(other) : std::move(*this); }

        template<typename Func,
                 typename ResultType = typename compatible_result_of<Func(const ValueType &)>::type>
        ResultType and_then(Func &&func) const& {
            return is_ok() ? func(m_value)
                           : ResultType{m_exception};
        };

        template<typename Func,
                typename ResultType = typename compatible_result_of<Func(ValueType &&)>::type>
        ResultType and_then(Func &&func) && {
            return is_ok() ? func(std::move(m_value))
                           : ResultType{std::move(m_exception)};
        };



        template<typename Func>
        auto err_visit(Func &&func) const& -> typename std::result_of<Func(const ExceptionType&)>::type {
            if (!is_err())
                throw std::logic_error("err_visit can only be called on error'd instances");

            try {
                std::rethrow_exception(m_exception);
            } catch (const ExceptionType &exc) {
                return func(exc);
            }

            throw std::logic_error("BUG: We failed to catch our exception...");
        }

        template<typename Func>
        auto err_visit(Func &&func) && -> typename std::result_of<Func(ExceptionType&&)>::type {
            if (!is_err())
                throw std::logic_error("err_visit can only be called on error'd instances");

            try {
                std::rethrow_exception(m_exception);
            } catch (ExceptionType &exc) {
                return func(std::move(exc));
            }

            throw std::logic_error("BUG: We failed to catch our exception...");
        }

        const ValueType&  unwrap() const& { throw_on_err(); return m_value; }
              ValueType&  unwrap() &      { throw_on_err(); return m_value; }
              ValueType&& unwrap() &&     { throw_on_err(); return std::move(m_value); }

        bool is_ok() const noexcept  { return m_type == Type::Value; }
        bool is_err() const noexcept { return m_type == Type::Exception; }

        const ValueType* operator->() const { return &unwrap(); }
              ValueType* operator->()       { return &unwrap(); }
        const ValueType&  operator*() const& { return unwrap(); }
              ValueType&  operator*() &      { return unwrap(); }
              ValueType&& operator*() &&     { return std::move(*this).unwrap(); }

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


    namespace traits {
        template<typename T, typename E>
        struct is_result<result<T, E>> : public std::true_type {};
    }


    template<typename ExceptionType = std::exception, typename Func,
              typename ValueType = typename std::result_of<Func()>::type>
    result<ValueType, ExceptionType> call(Func &&func) {
        return result<ValueType, ExceptionType>::call(std::forward<Func>(func));
    };
}
