/******************************************************************************
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016 Samuel R. Sevilla <samrs65@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
#include <type_traits>
#include <experimental/optional>

namespace std {
    template<typename T>
        using optional = std::experimental::optional<T>;
}

#define BOOST_HAS_CONCEPTS

#ifdef BOOST_HAS_CONCEPTS
#define IF_HAS_CONCEPTS(...) __VA_ARGS__
#else
#define IF_HAS_CONCEPTS(...)
#endif

namespace lazypp {

    namespace iterators {
        //IF_HAS_CONCEPTS(
            template<typename T>
            concept bool LazyIterator = requires(T a) {
                typename T::value_type;
                T(a);
                { a.next() } -> typename std::optional<typename T::value_type>;
            };

			/**
			 * A lazy aplication its a function that gets a LazyIterator and
			 * creates a new lazy iterator surround it.
			 */
            template<typename FuncApply, typename Iterator>
            concept bool LazyAplication = requires(FuncApply f, Iterator i) {
                { f(i) } -> LazyIterator;
                requires LazyIterator<Iterator>;
			};
        //)

        /**
         * each iterator should define:
         *  value_type
         */
        template<typename BaseIterator, typename MapFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
            class map_iterator {
                public:
                    typedef typename BaseIterator::value_type base_value_type;
                    typedef typename std::result_of_t<MapFunc(base_value_type)> value_type;
                    typedef map_iterator<BaseIterator, MapFunc> iterator;

                    map_iterator() = delete;
                    map_iterator(MapFunc map_func, BaseIterator base) : map_func_(map_func), base_(base) {}
                    map_iterator(const map_iterator<BaseIterator, MapFunc>& m) : map_func_(m.map_func_), base_(m.base_) {}

                    std::optional<value_type> next() {
                        auto v = base_.next();
                        if (v)
                            return std::optional<value_type>(map_func_(*v));
                        else
                            return std::optional<value_type>();
                    }

                private:
                    MapFunc map_func_;
                    BaseIterator base_;
            };

        template<typename BaseIterator, typename MapFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
			map_iterator<BaseIterator, MapFunc> make_map_iterator(MapFunc&& f, BaseIterator&& base) {
				return map_iterator<BaseIterator, MapFunc>(std::forward<MapFunc>(f), std::forward<BaseIterator>(base));
			}

        template<typename BaseIterator, typename FilterFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
            class filter_iterator {
                public:
                    typedef typename BaseIterator::value_type value_type;

                    filter_iterator() = delete;
                    filter_iterator(FilterFunc filter_func, BaseIterator base) : filter_func_(filter_func), base_(base) {}
                    filter_iterator(const filter_iterator<BaseIterator, FilterFunc>& f) : filter_func_(f.filter_func_), base_(f.base_) {}

                    std::optional<value_type> next() {
                        for (auto v = base_.next(); v; v = base_.next()) {
                            if (filter_func_(*v))
                                return v;
                        }
                        return std::optional<value_type>();
                    }

                private:
                    FilterFunc filter_func_;
                    BaseIterator base_;
            };

        template<typename BaseIterator, typename FilterFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
			filter_iterator<BaseIterator, FilterFunc> make_filter_iterator(FilterFunc&& f, BaseIterator&& base) {
				return filter_iterator<BaseIterator, FilterFunc>(std::forward<FilterFunc>(f), std::forward<BaseIterator>&& base);
			}

        template<typename BaseIterator> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
            class take_iterator {
                public:
                    typedef typename BaseIterator::value_type value_type;

                    take_iterator() = delete;
                    take_iterator(size_t num, BaseIterator base) : num_(num), base_(base) {}
                    take_iterator(const take_iterator<BaseIterator>& t) : num_(t.num_), base_(t.base_) {}

                    std::optional<value_type> next() {
                        if (num_) {
                            num_--;
                            return base_.next();
                        }
                        else
                            return std::optional<value_type>();
                    }

                private:
                    size_t num_;
                    BaseIterator base_;
            };

        template<typename BaseIterator> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
			take_iterator<BaseIterator> make_take_iterator(size_t num, BaseIterator&& base) {
				return take_iterator<BaseIterator>(num, std::forward<BaseIterator>(base));
			}

        template<typename GenFunc>
            class generate_iterator {
                public:
                    typedef std::result_of_t<GenFunc()> value_type;

                    generate_iterator() = delete;
                    generate_iterator(const GenFunc gen_func) : gen_func_(gen_func) {}
                    generate_iterator(const generate_iterator<GenFunc>& g) : gen_func_(g.gen_func_) {}

                    std::optional<value_type> next() {
                        return std::optional<value_type>(gen_func_());
                    }

                private:
                    GenFunc gen_func_;
            };

		template<typename GenFunc>
			generate_iterator<GenFunc> make_generate_iterator(GenFunc&& gen_func) {
				return generate_iterator<GenFunc>(std::forward<GenFunc>(gen_func));
			}

        template<typename BaseIterator, typename TestFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
            class take_while_iterator {
                public:
                    typedef typename BaseIterator::value_type value_type;

                    take_while_iterator() = delete;
                    take_while_iterator(TestFunc test_func, BaseIterator base) : test_func_(test_func), base_(base), ended_(false) {}
                    take_while_iterator(const take_while_iterator<BaseIterator, TestFunc>& t) : test_func_(t.test_func_), base_(t.base_), ended_(t.ended_) {}

                    std::optional<value_type> next() {
                        if (ended_)
                            return std::optional<value_type>();

                        auto v = base_.next();
                        if (v && test_func_(*v))
                            return v;

                        ended_ = true;
                        return std::optional<value_type>();
                    }

                private:
                    TestFunc test_func_;
                    BaseIterator base_;
                    bool ended_;
            };

        template<typename BaseIterator, typename TestFunc> IF_HAS_CONCEPTS(requires LazyIterator<BaseIterator>)
			take_while_iterator<BaseIterator, TestFunc> make_take_while_iterator(TestFunc&& test_func, BaseIterator&& base) {
				return take_while_iterator<BaseIterator, TestFunc>(std::forward<TestFunc>(test_func), std::forward<BaseIterator>(base));
			}

		/**
		 * FuncNext is a function that returns actual value and increment to the
		 * next value.
		 */
		template<typename T, typename FuncLast, typename FuncNext>
		class range_iterator {
			public:
				typedef T value_type;

				range_iterator() = delete;
				range_iterator(T first, FuncLast is_last, FuncNext func_next) : actual_(first), is_last_(is_last), func_next_(func_next) {}
				range_iterator(const range_iterator<T, FuncLast, FuncNext>& r): actual_(r.actual_), is_last_(r.is_last_), func_next_(r.func_next_) {}

				std::optional<value_type> next() {
					if (is_last_(actual_))
						return std::optional<value_type>();

					return std::optional<value_type>(func_next_(actual_));
				}

			private:
				T actual_;
				FuncLast is_last_;
				FuncNext func_next_;
		};
		template<typename T, typename FuncLast, typename FuncNext>
			range_iterator<T, FuncLast, FuncNext> make_range_iterator(T&& first, FuncLast&& is_last, FuncNext&& func_next) {
				return range_iterator<T, FuncLast, FuncNext>(std::forward<T>(first), std::forward<FuncLast>(is_last), std::forward<FuncNext>(func_next));
			}

		template<typename STLIterator>
			class stl_iterator {
				public:
					typedef typename STLIterator::value_type value_type;

					stl_iterator() = delete;
					stl_iterator(const STLIterator& first, const STLIterator& last) : actual_(first), last_(last) {}
					stl_iterator(STLIterator&& first, STLIterator&& last) : actual_(std::move(first)), last_(std::move(last)) {}
					stl_iterator(const stl_iterator<STLIterator>& s) : actual_(s.actual_), last_(s.last_) {}

					std::optional<value_type> next() {
						if (actual_ == last_)
							return std::optional<value_type>();

						return *actual_++;
					}

				private:
					STLIterator actual_;
					STLIterator last_;
			};
		
		template<typename STLIterator>
			stl_iterator<STLIterator> make_stl_iterator(STLIterator&& first, STLIterator&& last) {
				return stl_iterator<STLIterator>(std::forward<STLIterator>(first), std::forward<STLIterator>(last));
			}

        template<typename T>
        using stl_iterator_t = stl_iterator<std::remove_reference_t<T>>;

        template<typename Iterator> IF_HAS_CONCEPTS(requires LazyIterator<Iterator>)
            class wrapper;

        template<typename T>
        using wrapper_t = wrapper<std::remove_reference_t<T>>;

        template<typename T> IF_HAS_CONCEPTS(requires LazyIterator<T>)
            constexpr wrapper_t<T> wrap(T&& t) {
                return wrapper_t<T>(std::forward<T>(t));
            }

        template<typename Iterator> IF_HAS_CONCEPTS(requires LazyIterator<Iterator>)
            class wrapper {
                public:
					typedef typename Iterator::value_type value_type;

                    wrapper() = delete;
                    wrapper(const wrapper<Iterator>& iterator) : iterator_(iterator.iterator_) {}
                    wrapper(wrapper<Iterator>&& iterator) : iterator_(std::move(iterator.iterator_)) {}
                    wrapper(Iterator&& iterator) : iterator_(std::forward<Iterator>(iterator)) {}

					/**
					 * Grant the hability of use stream sintax for concatenate
					 * lazyness operations.
					 * @param f function that acepts a lazy interator and
					 *          generates a new lazy iterator arround it.
					 * @return New wrapper arround a new lazy iterator.
					 */
					template<typename Func> IF_HAS_CONCEPTS(requires LazyAplication<Func, Iterator>)
						wrapper<std::result_of_t<Func(Iterator)>> operator>>(Func f) {
							return wrap(f(iterator_));
						}

                    template<typename Func>
                        wrapper<map_iterator<Iterator, Func>> map(Func f) {
                            return wrap(map_iterator<Iterator, Func>(f, iterator_));
                        }

                    template<typename Func>
                        wrapper<filter_iterator<Iterator, Func>> filter(Func f) {
                            return wrap(filter_iterator<Iterator, Func>(f, iterator_));
                        }

                    wrapper<take_iterator<Iterator>> take(size_t num_elems) {
                        return wrap(take_iterator<Iterator>(num_elems, iterator_));
                    }

                    template<typename Func>
                        wrapper<take_while_iterator<Iterator, Func>> take_while(Func f) {
                            return wrap(take_while_iterator<Iterator, Func>(f, iterator_));
                        }

                    template<typename Func>
                        void each(Func f) {
                            decltype(iterator_.next()) v;
                            while((v = iterator_.next()))
                                f(*v);
                        }

					template<typename To>
						std::remove_reference_t<To> to() {
							std::remove_reference_t<To> new_container;
						    each([&new_container](auto v) {
						            new_container.push_back(v);
						        });
							return To(std::move(new_container));
						}

					template<typename To, typename Func>
						To fold(To acum, Func f) {
							each([&](auto v) {
									acum = f(acum, v);
								});
							return acum;
						}

                private:
                    Iterator iterator_;
            };
	}

	namespace applications {
		using namespace lazypp::iterators;

		template<typename Func>
		class map_ {
			public:
				map_(Func&& f) : f_(std::forward<Func>(f)) {}

				template<typename Iterator> IF_HAS_CONCEPTS(requires LazyIterator<Iterator>)
				auto operator()(Iterator&& it) {
					return make_map_iterator(f_, std::forward<Iterator>(it));
				}

			private:
				Func f_;
		};

		template<typename Func>
		auto map(Func&& f) {
//			return [f](auto base_iterator) {
//				return make_map_iterator(f, base_iterator);
//			};
			return map_<Func>(std::forward<Func>(f));
		}

		template<typename Func>
		auto filter(Func f) {
			return [f](auto base_iterator) {
				return make_filter_iterator(f, base_iterator);
			};
		}

		template<typename Func>
		auto take_while(Func f) {
			return [f](auto base_iterator) {
				return make_take_while_iterator(f, base_iterator);
			};
		}

		auto take(size_t num_elems) {
			return [num_elems](auto base_iterator) {
				return make_take_iterator(num_elems, base_iterator);
			};
		}
	}

	namespace from {
		
		using namespace lazypp::iterators;

        template<typename Func>
            auto generator(Func f) {
                return wrap(generate_iterator<Func>(f));
            }

		template<typename T, typename LastFunc, typename NextFunc>
			auto range(T& begin, LastFunc last_func, NextFunc next_func) {
				return wrap(range_iterator<T, LastFunc, NextFunc>(begin, last_func, next_func));
			}

		template<typename T>
			auto range(T begin, T end) {
				return range(begin, [end](const T& v){ return v == end; }, [](T& v) { return v++; });
			}
		
		template<typename T, typename NextFunc>
			auto range(T begin, T end, NextFunc next_func) {
				return range(begin, [end](const T& v){ return v == end; }, next_func);
			}

		template<typename T>
			wrapper_t<stl_iterator_t<T>> stl_iterator(T&& first, T&& last) {
				return wrap(stl_iterator_t<T>(std::forward<T>(first), std::forward<T>(last)));
			}

		template<typename T>
			auto stl_container(T& container) {
				return stl_iterator(begin(container), end(container));
			}

	}
}
