[![GitHub license](https://img.shields.io/badge/license-LGPL-blue.svg)](https://raw.githubusercontent.com/laparca/lazypp/master/LICENSE)
[![Build Status](https://travis-ci.org/laparca/lazypp.svg?branch=master)](https://travis-ci.org/laparca/lazypp)

# lazypp
Lazy++ it's an implementation of lazy evaluation for C++. It's a simple library wich porpose is to learn more about lazyness and C++.

## Design principles

Lazypp was born with the idea of haskell lazyness. It's based on Rust lazy implementation.

### Lazy Iterators

```
template<typename T>
concept bool LazyIterator = requires(T a) {
    typename value_type_t<T>;
    T(a);
    { a.next() } -> typename std::optional<value_type_t<T>>;
};
```

### Generators

### Lazy applications

## How to add your own applications?
