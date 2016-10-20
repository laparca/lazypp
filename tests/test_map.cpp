#include <lazypp.hpp>
#include <vector>
#include <iostream>

int main() {
	auto show = [](auto&& v) { std::cout << v << std::endl; };
	auto square = [](auto&& v) { return v * v; };
	std::vector<int> values {1, 2, 3, 4, 5, 6, 7, 8};

	std::cout << "Testing from generator" << std::endl;
    lazypp::from::generator([]() {
			static size_t v = 0;
			return v++;
		})
		.filter([](size_t v) { return v % 2 == 0;})
		.take(10)
		.map(square)
		.each(show);

	std::cout << "Testing from range" << std::endl;
	lazypp::from::range(1, 10)
		.map(square)
		.each(show);

	std::cout << "Testing from iterator" << std::endl;
	lazypp::from::stl_iterators(std::begin(values), std::end(values))
		.each(show);

	std::cout << "Testing from stl container" << std::endl;
	lazypp::from::stl_container(values)
		.each(show);

	std::cout << "Testing conversion to stl container" << std::endl;
	std::vector<int> converted = lazypp::from::range(1, 10)
		.map(square)
		.to<std::vector<int>>();

	for(auto&& v : converted)
		std::cout << v << std::endl;

	return 0;
}
