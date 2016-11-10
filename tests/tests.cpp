#include <lazypp.hpp>
#include <vector>
#include <iostream>
#define BOOST_TEST_MODULE lazypp
#include <boost/test/included/unit_test.hpp>
#include <typeinfo>
#include <cxxabi.h>

std::string demangle(const char* name) {
	if (!name) return std::string(name);

	int st;
	char *ptr_name = abi::__cxa_demangle(name, 0, 0, &st);

	if (!ptr_name) {
		std::cerr << "name cannot be demangled" << std::endl;
		return std::string(name);
	}

	std::cerr << "Demangled status: ";
	switch(st) {
		case 0:
			std::cerr << "OK";
			break;
		case -1:
			std::cerr << "Memory allocation error";
			break;
		case -2:
			std::cerr << "Name is not valid name under C++ ABI namgling rules";
			break;
		case -3:
			std::cerr << "One of the arguments is invalid";
			break;
	}
	std::cerr << std::endl;

	std::string demangled_name(ptr_name);
	free(ptr_name);
	return std::move(demangled_name);
}
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

auto square = [](auto&& v) { return v * v; };
auto even = [](auto&& v) { return v % 2 == 0; };

using namespace lazypp::applications;

BOOST_AUTO_TEST_CASE(Checking_generator_and_take_10_elements)
{
    int count = 0;
    (lazypp::from::generator(infinite()) >> take(10)).each([&count](auto&& v) {
        static int v1 = 0;
        BOOST_CHECK_EQUAL( v1++, v );
        count ++;
    });
    BOOST_CHECK_EQUAL( count, 10 );
}

BOOST_AUTO_TEST_CASE(Checking_filtering)
{
    int count = 0;
    (lazypp::from::generator(infinite()) >> filter(even) >> take(10)).each([&count](auto&& v) {
        static int v1 = 0;
        BOOST_CHECK_EQUAL( v1, v );
        v1 += 2;
        count ++;
    });
    BOOST_CHECK_EQUAL( count, 10 );
}

namespace lazypp {
	namespace iterators {
		template<typename BaseIterator>
		class join_iterator {
		public:
			typedef value_type_t<BaseIterator> base_type;
			typedef value_type_t<base_type> value_type;

			join_iterator() = delete;
			join_iterator(BaseIterator& base) : iterator_(base) {}
			join_iterator(BaseIterator&& base) : iterator_(std::move(base)) {}
			join_iterator(const join_iterator<BaseIterator>& it) : iterator_(it.iterator_) {}

			std::optional<value_type> next() {
				return std::optional<value_type>();
			}

		private:
			BaseIterator iterator_;
//			std::optional<value_type> 

		};

		template<typename BaseIterator>
		join_iterator<BaseIterator> make_join_iterator(BaseIterator&& it) {
			return join_iterator<BaseIterator>(std::forward<BaseIterator>(it));
		}
	}

	namespace applications {
		using namespace lazypp::iterators;

		class join_ {
			public:
				template<typename Iterator>
				auto operator()(Iterator&& it) {
					return make_join_iterator(it);
				}
		};

		auto join() {
			return join_();
		}
	}
}

BOOST_AUTO_TEST_CASE(Checking_lazy_algorithm) {
	auto iter = lazypp::from::generator(infinite(1)) >> map([](size_t z) {
		return lazypp::from::range(1u, z) >> map([z](size_t x) {
			return lazypp::from::range(x, z) >> filter([x, z](size_t y) {
				return x*x + y*y == z*z;
			}) >> map([x, z](size_t y) {
				return std::tuple<size_t, size_t, size_t>(x, y, z);
			});
		}) >> join();
	}) >> join();

	std::cerr << demangle(typeid(iter).name()) << std::endl;

	iter.each([](auto&& v) {
		std::cerr << demangle(typeid(v).name()) << std::endl;
	});
}
#if 0
int main() {
    auto show = [](auto&& v) { std::cout << v << std::endl; };
    std::vector<int> values {1, 2, 3, 4, 5, 6, 7, 8};


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
#endif
//vim:tabstop=4:shiftwidth=4:expandtab:autoindent
