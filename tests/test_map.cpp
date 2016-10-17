#include <lazypp.hpp>
#include <vector>
#include <iostream>

template<typename T>
T square(T v) {
    return v * v;
}

int main() {
    lazycpp::iterators::from_generator([]() {
        static size_t v = 0;
        return v++;
    })
    .filter([](size_t v) { return v % 2 == 0;})
    .take(10)
    .map([](size_t v) { return v*v;})
    .each([](size_t v) { std::cout << v << std::endl; });

	return 0;
}
