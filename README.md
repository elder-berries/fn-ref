# fn-ref

## Overview 

`FnRef` implements a non-owning callable wrapper that avoids the indirection cost of
traditional type-erasure designs. This is achieved through template specialization,
which allows aggressive compiler optimizations. Storing the concrete callable type, 
the compiler can often see through the forwarding layer when the target is
visible at the call site. `FnRef` typically preserves inlining potential suited
for hot loops or frequent callback interrupts where performance is critical.

## Introduction

We begin with a brief account of the simplest usage we have for the construct. 

```c++

#include "fn_ref.hpp"

int add(int a, int b) {
    return a + b;
}

int main() {

    // Automatic type deduction implicitly resolves to
    // FnRef<int(*)(int, int)>.
    FnRef f(&add); 

    // Devitualization invokes direct call as 
    // int result = add(2, 3);
    int result = f(2, 3);

    return 0;
}
```

## Supported Callables

`FnRef` transparently wraps any standard `C++` callable:

- **Free functions pointers**
- **Member functions** (`static` or non-`static` and both `const` or non-`const`)
- **Lambdas** (captureless and capturing)
- **Functors**

## The Contract

A number of considerations suggest themselves:

### ABI stability and size

`FnRef` guarantees that the binary representation of each specialization is equivalent to
its underlying pointer type. Layout is therefore trivially stable across all translation
units of a single compilation. We formally prove this in Section [**Tests**](./tests). However,
for completeness, we briefly address here the same claims it proposes and subsequent practical limitations.

A fundamental property is that two objects with equivalent representation
compare equal. It is easy to realise why this is the case among `FnRef` specializations
and their respective internal pointers. Clearly, the expectation is that `FnRef` will not
add any trailing padding, and directly inherit the alignment of the underlying
pointer, which is already natural to the platform. Because non-static member functions, capturing lambdas, and functors have compiler
specific ABI's, `FnRef` will be **unstable across compilation boundaries** by definition. 

While `FnRef` introduces no additional ABI complexity, it cannot be stable beyond
the types it contains. It is intended to be used as header-only library within a single binary.


#### Pointer sizes

It is also worth noting that callable pointer size may vary. On a typical LP64 system:

```
+--------------------------+----------+
| Callable Type            |   size   |
+--------------------------+----------+
| Free function pointers   |  8 bytes |
| Member function pointers | 16 bytes |
| Lambdas                  |  8 bytes |
| Functors                 |  8 bytes |
+--------------------------+----------+
```

`FnRef` **will** preserve the native size and storage alignment of these pointers. 

### Lifetime management

The user, hereafter called **you**, is responsible for callable lifetime management. As the primary caretaker, 
you must see to it that the the callable outlives the wrapper. Undefined behaviour 
is guaranteed when making poor decisions such as returning an `FnRef` object 
from a callable only defined in a temporary context. For instance,

```c++
auto create_unsafe() {
    // Define local lambda.
    auto foo = [](int a, int b) -> int { return a + b; };
    // ret unsafe.
    return FnRef(&foo);
}

int main() {
    auto unsafe = create_unsafe();
    int result = unsafe(2, 3);
}
```

will compile but produce gibberish/crash at runtime. Of course, this is because the internal pointer
address is no longer valid. From the generated `asm`, we might observe 

```asm
create_unsafe():
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32                  ; alloc stack frame.
    
    ; ... construct lambda on stack.
    ; ... FnRef stores lambda stack address to internal pointer.
    
    mov     rax, qword ptr [rbp - 8] ; ret FnRef instance, unwound stack.
    add     rsp, 32                  ; destroy stack frame.
    pop     rbp
    ret                              ; ret instruction, dangling ptr.
```

### Exception safety

As of now, `FnRef::operator()`  is **not** `noexcept`. This is somewhat intentional 
since preserving a simple forwarding interface is more
important than trying to impose exception guarantees at the wrapper level.
Users that require non-throwing behavior should express that requirement on the
callable itself or at the call site.
