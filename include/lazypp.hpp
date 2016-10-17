#include <type_traits>
#include <experimental/optional>

namespace std {
    template<typename T>
        using optional = std::experimental::optional<T>;
}
namespace lazycpp {

    namespace iterators {
        /**
         * each iterator should define:
         *  value_type
         */
        template<typename BaseIterator, typename MapFunc>
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

        template<typename BaseIterator, typename FilterFunc>
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

        template<typename BaseIterator>
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

        template<typename BaseIterator, typename TestFunc>
            class take_while_iterator {
                public:
                    typedef typename BaseIterator::value_type value_type;

                    take_while_iterator() = delete;
                    take_while_iterator(TestFunc test_func, BaseIterator base) : test_func_(test_func), base_(base), ended_(false) {}
                    take_while_iterator(const take_while_iterator<TestFunc, BaseIterator>& t) : test_func_(t.test_func_), base_(t.base_), ended_(t.ended_) {}

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

        template<typename Iterator>
            class iterator_wrapper {
                public:
                    iterator_wrapper() = delete;
                    iterator_wrapper(const iterator_wrapper<Iterator>& iterator) : iterator_(iterator.iterator_) {}
                    iterator_wrapper(Iterator iterator) : iterator_(iterator) {}

                    template<typename Func>
                        iterator_wrapper<map_iterator<Iterator, Func>> map(Func f) {
                            return iterator_wrapper<map_iterator<Iterator, Func>>(map_iterator<Iterator, Func>(f, iterator_));
                        }

                    template<typename Func>
                        iterator_wrapper<filter_iterator<Iterator, Func>> filter(Func f) {
                            return iterator_wrapper<filter_iterator<Iterator, Func>>(filter_iterator<Iterator, Func>(f, iterator_));
                        }

                    iterator_wrapper<take_iterator<Iterator>> take(size_t num_elems) {
                        return iterator_wrapper<take_iterator<Iterator>>(take_iterator<Iterator>(num_elems, iterator_));
                    }

                    template<typename Func>
                        iterator_wrapper<take_while_iterator<Iterator, Func>> take_while(Func f) {
                            return iterator_wrapper<take_while_iterator<Iterator, Func>>(take_while_iterator<Iterator, Func>(f, iterator_));
                        }

                    template<typename Func>
                        void each(Func f) {
                            decltype(iterator_.next()) v;
                            while((v = iterator_.next()))
                                f(*v);
                        }

                private:
                    Iterator iterator_;
            };

        template<typename Func>
            iterator_wrapper<generate_iterator<Func>> from_generator(Func f) {
                return iterator_wrapper<generate_iterator<Func>>(generate_iterator<Func>(f));
            }
    }
}
