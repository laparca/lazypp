#ifndef LAZYPP_HPP

#define LAZYPP_HPP

#include <iterator>
#include <cxxabi.h>
#include <iostream>
#if 0
template<typename T>
std::string demangle(T& v) {
	int status;
	char* demangled = abi::__cxa_demangle(typeid(v).name(), 0, 0, &status);
	std::string result(demangled);
	free(demangled);
	return result;
}

class __log_func {
	public:
		__log_func(std::string cl, std::string str) : class_(cl), name_(str) {
			level_++;
			level();
			std::cout << ">> " << class_ << "::" << name_ << std::endl;
		}

		~__log_func() {
			level();
			std::cout << "<< " << class_ << "::" << name_ << std::endl;
			level_--;
		}
	private:
		std::string class_;
		std::string name_;

		static int level_;
		static void level() {
			for (int i = 0; i < level_; i++) {
				std::cout << "  ";
			}
		}
};
int __log_func::level_ = 0;
#define logf() __log_func __method_logger(demangle(*this), __FUNCTION__)
#else
#define logf() do { } while(0) 
#endif

namespace lazypp {
	namespace iterators {

		/**
		 */
		template<typename InternalIterator, typename T, typename Func>
		class map_iterator: public std::iterator<std::forward_iterator_tag, T> {
			public:
			typedef T value_type;
			typedef typename InternalIterator::difference_type  difference_type;
			typedef T* pointer;
			typedef T& reference;
			typedef typename InternalIterator::iterator_category iterator_category;
			typedef map_iterator<InternalIterator, T, Func> iterator;

			public:
			map_iterator() = delete;
			map_iterator(InternalIterator it, Func func) : it_(it), func_(func) {logf();}
			map_iterator(const iterator& mi) : it_(mi.it_), func_(mi.func_) {logf();}

			iterator operator=(iterator it) {
				logf();
				it_ = it.it_;
				return *this;
			}

			bool operator==(iterator it) {
				logf();
				return it_ == it.it_;
			}

			bool operator!=(iterator it) {
				logf();
				return it_ != it.it_;
			}

			value_type operator*() {
				logf();
				return func_(*it_);
			}

			value_type operator->() {
				logf();
				return func_(*it_);
			}

			iterator& operator++() {
				logf();
				it_++;
				return *this;
			}

			iterator operator++(int) {
				logf();
				InternalIterator it(it_);
				it_++;
				return iterator(it, func_);
			}

			private:
			InternalIterator it_;
			Func func_;
		};
	}

	namespace containers {
		/* Concept LazyIteratorContainer {
		 *    iterator begin();
		 *    iterator end();
		 * }
		 */

		template<typename InternalIterator, typename F>
		class map_container {
			public:
			typedef typename InternalIterator::value_type internal_value_type;
			typedef typename std::result_of<F(internal_value_type)>::type calculated_type;
			typedef typename lazypp::iterators::map_iterator<InternalIterator, calculated_type, F> iterator;
			typedef const iterator const_iterator;

			public:
			map_container() = delete;
			map_container(const map_container<InternalIterator, F>& mc) : begin_(mc.begin_), end_(mc.end_), func_(mc.func_) {logf(); }
			map_container(InternalIterator begin, InternalIterator end, F func) : begin_(begin), end_(end), func_(func) { logf(); }
			iterator begin() { logf(); return iterator(begin_, func_); }
			iterator end() { logf(); return iterator(end_, func_); }

			private:
			InternalIterator begin_;
			InternalIterator end_;
			F func_;
		};
	}

	namespace algorithms {
		/**
		 * Maps the container with a function.
		 */
		template<typename Iter, typename Func>
		auto map(Iter begin, Iter end, Func f) {
			return containers::map_container<Iter, Func>(begin, end, f);
		}

		template<typename Container, typename Func>
		auto map(Container& c, Func f) {
			return map(std::begin(c), std::end(c), f);
		}
/*
		void filter();
		void fold();
		void generator();
*/
	}

	namespace utils {
		template<typename Container>
		class lazy_container {
			public:
			typedef lazy_container<Container> container;

			lazy_container(Container c) : container_(c) {}

			template<typename F>
			auto map(F func) /*-> decltype(make_lazy_container(map(container_, func)))*/ {
				return make_lazy_container(map(container_, func));
			}

			auto begin() /*-> decltype(std::begin(container_))*/ { return std::begin(container_); }
			auto end() /*-> decltype(std::end(container_))*/ { return std::end(container_); }

			private:
			container container_;
		};

		template<typename Container>
		lazy_container<Container> make_lazy_container(Container c) {
			return lazy_container<Container>(c);
		}
	}
}

#endif /* end of include guard: LAZYPP_HPP */
