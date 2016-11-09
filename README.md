[![GitHub license](https://img.shields.io/badge/license-LGPL-blue.svg)](https://raw.githubusercontent.com/laparca/lazypp/master/LICENSE)
[![Build Status](https://travis-ci.org/laparca/lazypp.svg?branch=master)](https://travis-ci.org/laparca/lazypp)

# lazypp
Lazy++'s an implementation of lazy evaluation for C++. It's not a total lazy evaluation, but an aproximation like Rust implementarion. For a better lazy aproximation in C++ we recomend to read [Getting Lazy with C++](https://bartoszmilewski.com/2014/04/21/getting-lazy-with-c/) from [Bartosz Milewski's blog](https://bartoszmilewski.com/).

Lazy++'s a simple library wich porpose is to learn more about lazyness and C++.

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

