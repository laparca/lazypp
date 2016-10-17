namespace lazycpp {

  namespace iterators {
    /**
     * each iterator should define:
     *  value_type
     */
    template<typename BaseIterator, typename MapFunc>
    class map_iterator {
    public:
      typedef std::result_of<MapFunc(BaseIterator::value_type)>::type value_type;
      typedef map_iterator<BaseIterator, MapFunc> iterator;

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

      filter_iterator() delete;
      filter_iterator(FilterFunc filter_func, BaseIterator base) : filter_func_(filter_func), base_(base) {}
      filter_iterator(const filter_iterator<BaseIterator, FilterFunc>& f) : filter_func_(f.filter_func_), base_(f.base_) {}

      std::optional<value_type> next() {
	bool b = false;
	
	do {
	  auto v = base_.next();
	  if (v)
	    b = filter_func_(*v);
	  else
	    return std::optional<value_type>();
	} while(!b);

	return v;
      }

    private:
      FilterFunc filter_func_;
      BaseIterator base_;
    };

    template<typename BaseIterator>
    class take_iterator {
    public:
      typedef typename BaseIterator::value_type value_type;

      take_iterator() delete;
      take_iterator(size_t num, BaseIterator base) : num_(num), base_(base) {}
      take_iterator(const take_iterator<BaseIterator>& t) : num_(t.num_), base_(t.base_) {}

      std::optional<value_type> next() {
	if (num)
	  return base_.next();
	else
	  return std::optional<value_type>();

	num--;
      }

    private:
      size_t num_;
      BaseIterator base_;
    };

    template<typename GenFunc>
    class generate_iterator {
    public:
      typedef std::result_of<GenFunc()>::type value_type;

      take_iterator() delete;
      take_iterator(GenFunc gen_func) : gen_func_(gen_func) {}
      take_iterator(const generate_iterator<GenFunc>& g) : gen_func_(g.gen_func_) {}

      std::optional<value_type> next() {
	return std::optional<value_type>(gen_func());
      }

    private:
      GenFunc gen_func_;
    };

    template<typename BaseIterator, typename TestFunc>
    class take_while_iterator {
    public:
      typedef typename BaseIterator::value_type value_type;

      take_while_iterator() delete;
      take_while_iterator(TestFunc test_func, BaseIterator base) : test_func_(test_func), base_(base), ended_(false) {}
      take_while_iterator(const take_while_iterator<TestFunc, BaseIterator>& t) : test_func_(t.test_func_), base_(t.base_), ended(t.ended_) {}

      std::optinal<value_type> next() {
	if (ended)
	  return std::optional<value_type>();
	
	 auto v = base_.next();
	 if (v && test_func_(*v))
	   return v;

	 ended = true;
	 return std::optional<value_type>();
      }

    private:
      TestFunc test_func_;
      BaseIterator base_;
      bool ended_;
    };

  }
}
