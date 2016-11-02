#include <lazypp.hpp>
#include <vector>
#include <iostream>

struct infinite { 
	size_t v_;
	infinite(size_t v = 0) : v_(v) {}
	size_t operator()() {
		return v_++;
	}
};

template<typename T>
struct _less_than {
	T v_;
	_less_than(T&& v) : v_(std::forward<T>(v)) {}
	template<typename T2>
	bool operator()(T2&& v) {
		return v < v_;
	}
};
template<typename T>
_less_than<T> less_than(T&& v) {
	return _less_than<T>(std::forward<T>(v));
}

int main() {
	auto show = [](auto&& v) { std::cout << v << std::endl; };
	auto square = [](auto&& v) { return v * v; };
	auto even = [](auto&& v) { return v % 2 == 0; };
	std::vector<int> values {1, 2, 3, 4, 5, 6, 7, 8};

	using namespace lazypp::applications;

	std::cout << "Testing from generator" << std::endl;
    (lazypp::from::generator(infinite()) >>
		filter(even) >>
		take(10) >>
		map(square)).each(show);

	std::cout << "Testing from range" << std::endl;
	(lazypp::from::range(1, 10) >>
		map(square)).each(show);

    std::cout << "Testing takewhile" << std::endl;
    (lazypp::from::generator(infinite()) >>
        take_while(less_than(10u))).each(show);

	std::cout << "Testing from iterator" << std::endl;
	lazypp::from::stl_iterator(std::begin(values), std::end(values))
		.each(show);

	std::cout << "Testing from stl container" << std::endl;
	lazypp::from::stl_container(values)
		.each(show);

	std::cout << "Testing conversion to stl container" << std::endl;
	std::vector<int> converted = (lazypp::from::range(1, 10) >> map(square)).to<std::vector<int>>();

	for(auto&& v : converted)
		std::cout << v << std::endl;

	std::cout << "Testing folding" << std::endl;
	std::cout << "Is 55 == " << (lazypp::from::range(1, 1000) >> take(10)).fold(0, [](auto acum, auto value) { return acum + value;}) << "?" << std::endl;

	using namespace lazypp::applications;

	std::cout << "Testing operator>>" << std::endl;
	(lazypp::from::generator(infinite()) >> map(square) >> take(10) >> filter(even)).each(show);

	return 0;
}
