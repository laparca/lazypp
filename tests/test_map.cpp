#include <lazypp.hpp>
#include <vector>
#include <iostream>

int main() {
	auto show = [](auto&& v) { std::cout << v << std::endl; };
	auto square = [](auto&& v) { return v * v; };
	std::vector<int> values {1, 2, 3, 4, 5, 6, 7, 8};

    lazypp::from::generator([]() {
        static size_t v = 0;
        return v++;
    })
    .filter([](size_t v) { return v % 2 == 0;})
    .take(10)
    .map(square)
    .each(show);

	lazypp::from::range(1, 10)
		.map(square)
		.each(show);

	lazypp::from::range(std::begin(values), std::end(values))
		.each([](auto&& v) { std::cout << *v << std::endl; });
	return 0;
}
