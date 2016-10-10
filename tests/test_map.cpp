#include <lazypp.hpp>
#include <vector>
#include <iostream>

int main() {
	std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	for(auto value : lazypp::algorithms::map(values, [](int v){ return v*v; }))
		std::cout << value << std::endl;

	return 0;
}
