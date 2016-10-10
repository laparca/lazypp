#include <lazypp.hpp>
#include <vector>
#include <iostream>

template<typename T>
T square(T v) {
    return v * v;
}

int main() {
	std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto sq = lazypp::algorithms::map(values, square<int>);

	for(auto value : sq)
		std::cout << value << std::endl;

	return 0;
}
