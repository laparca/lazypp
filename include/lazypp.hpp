#ifndef LAZYPP_HPP

#define LAZYPP_HPP

#include <iterator>

namespace lazypp {
	namespace iterators {

		/**
		 */
		template<typename InternalIterator, typename T, typename Func>
		class map_iterator: public std::iterator<std::forward_iterator_tag, T> {
			public:
			typedef T value_type;
			typedef typename InternalIterator::difference_type  difference_type;
			typedef typename T* pointer;
			typedef typename T& reference;
			typedef typename InternalIterator::iterator_category iterator_category;
			typedef InternalIterator iterator;

			public:
			map_iterator() : it_() {}
			map_iterator(InternalIterator it) : it_(it) {}
			map_iterator(const iterator& mi) : it_(mi.it_) {}

			iterator operator=(iterator it) {
				it_ = it.it_;
				return *this;
			}

			bool operator==(iterator it) {
				return it_ == it.it_;
			}

			bool operator!=(iterator it) {
				return it_ != it.it_;
			}

			value_type operator*() {
				return func_(*it_);
			}

			value_type operator->() {
				return func_(*it);
			}

			iterator operator++() {
				it_++;
				return *this;
			}

			iterator operator++(int) {
				iterator it(it_);
				it_++;
				return it;
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
			typedef typename map_iterator<InternalIterator, std::result_of(F(internal_value_type))::type> iterator;

			public:
			map_container(InternalIterator b, InternalIterator e, F func) : begin_(b), end_(e), func_(func) {}
			iterator begin() { return iterator(begin_, func_); }
			iterator end() { return iterator(end_, func_); }

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
		template<typename Iter, typename Fuc>
		auto map(Iter begin, Iter end, Func f) -> decltype(make_map_container(begin, end, f)) {
			return make_map_container(begin, end, f);
		}

		template<typename Container, typename Func>
		auto map(Container c, Func f) -> decltype(map(std::begin(c), std::end(c), f)) {
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
			typename lazy_container<Container> container;

			lazy_container(Container c) : container_(c) {}

			template<typename F>
			auto map(F func) -> decltype(make_lazy_container(map(container_, func))) {
				return make_lazy_container(map(container_, func));
			}

			auto begin() -> decltype(std::begin(container_)) { return std::begin(container_); }
			auto end() -> decltype(std::end(container_)) { return std::end(container_); }

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
