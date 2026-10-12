#ifndef FN_REF_HPP
#define FN_REF_HPP


#include <utility>


template <typename T>
class FnRef;


template <typename Ret, typename... Args>
class FnRef<Ret(*)(Args...)> {

    using functor = Ret(*)(Args...);
    functor f;

public:
    explicit FnRef(functor f_) : f(f_) {};

    Ret operator()(const Args&... args) const {
        return f(args...);
    }
};


template <typename Class, typename Ret, typename... Args>
class FnRef<Ret(Class::*)(Args...)> {
    
    using functor = Ret(Class::*)(Args...);
    functor f;

public:
    explicit FnRef(functor f_) : f(f_) {};

    Ret operator()(Class& obj, const Args&... args) const {
        return (obj.*f)(args...);
    }
};


template <typename Class, typename Ret, typename... Args>
class FnRef<Ret(Class::*)(Args...) const> {
    
    using functor = Ret(Class::*)(Args...) const;
    functor f;

public:
    explicit FnRef(functor f_) : f(f_) {};

    Ret operator()(const Class& obj, const Args&... args) const {
        return (obj.*f)(args...);
    }
};


template <typename Callable>
class FnRef {
    
    using functor = Callable*;
    functor f;
        
    template <typename... Args>
    using Ret = decltype(std::declval<Callable>()(std::declval<Args>()...));

public:
    explicit FnRef(functor f_) : f(f_) {}

    template <typename... Args>
    auto operator()(Args&&... args) const -> Ret<Args...> {
        return (*f)(std::forward<Args>(args)...);
    }
};


template <typename Callable>
FnRef(Callable) -> FnRef<Callable>;
            
        
#endif
