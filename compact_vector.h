#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <limits>
#include <type_traits>
#include <algorithm>

template <
	class T,
	int compact_max_size = -1,
	class allocator_type = std::allocator<T>>
class compact_vector
{
public:
	struct full_storage
	{
		T* begin = nullptr;
		size_t capacity = 0;

		T* get(size_t i) noexcept
		{
			return begin + i;
		}

		const T* get(size_t i) const noexcept
		{
			return begin + i;
		}
	};

	static constexpr size_t compact_default_capacity = sizeof(full_storage) / sizeof(T);

	static constexpr size_t compact_default_capacity_nonzero = compact_default_capacity == 0 ? 1 : compact_default_capacity;

	static constexpr size_t compact_capacity = compact_max_size <= 0 ? compact_default_capacity_nonzero : compact_max_size;

	static constexpr size_t vector_max_size = std::numeric_limits<size_t>::max() >> 1;

	struct compact_storage
	{
		T buffer[compact_capacity];

		T* get(size_t i) noexcept
		{
			return &buffer[i];
		}

		const T* get(size_t i) const noexcept
		{
			return &buffer[i];
		}
	};

	// Empty Base Optimization
	struct size_allocator_pair : public allocator_type
	{
		// bitset 1000...000
		static const size_t zero_compact = size_t(1) << (8 * sizeof(size_t) - 1);

		size_allocator_pair(const allocator_type& base) :
			allocator_type(base)
		{}

		size_allocator_pair(allocator_type&& base) :
			allocator_type(base)
		{}

		size_allocator_pair() :
			allocator_type(allocator_type())
		{}

		allocator_type* get_allocator()
		{
			return this;
		}

		const allocator_type* get_allocator() const
		{
			return this;
		}

		size_t get_size() const noexcept
		{
			return size & vector_max_size;
		}

		bool is_compact() const noexcept
		{
			return size & zero_compact;
		}

		void set_size(size_t new_size, bool is_compact)
		{
#ifdef COMPACT_VECTOR_DEBUG
			if (new_size > vector_max_size)
				throw std::exception(u8"попытка создать вектор больше max_size");
#endif
			if (is_compact)
				size = zero_compact | new_size;
			else
				size = new_size;
		}

	private:
		size_t size = zero_compact;
	};

	union
	{
		compact_storage compact;
		full_storage full;
	};

	size_allocator_pair size_allocaltor;

public:
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = T*; // todo
	using const_reverse_iterator = const T*; // todo
	using this_type = compact_vector<T, compact_max_size, allocator_type>;

	/// constructor: default
	/*!
	Constructs an empty container, with no elements.
	*/
	explicit compact_vector(const allocator_type& alloc = allocator_type()) :
		size_allocaltor(alloc)
	{}

	/// constructor: fill
	/*!
	Constructs a container with n elements.
	*/
	explicit compact_vector(size_t n) :
		size_allocaltor()
	{
		resize(n);
	}

	/// constructor: fill
	/*!
	Constructs a container with n elements. Each element is a copy of val.
	*/
	compact_vector(size_t n, const T& val, const allocator_type& alloc = allocator_type()) :
		size_allocaltor(alloc)
	{
		resize(n, val);
	}

	/// constructor:: range
	/*!
	Constructs a container with as many elements as the range [first,last), 
	with each element emplace-constructed from its corresponding element in that range, in the same order.
	*/
	template <class InputIterator>
	compact_vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) :
		size_allocaltor(alloc)
	{
		assign(first, last);
	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const this_type& x) :
		size_allocaltor(x.get_allocator())
	{
		reserve(x.size());
		copy_data(x.begin(), x.end(), begin());
	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const this_type& x, const allocator_type& alloc) :
		size_allocaltor(alloc)
	{
		reserve(x.size());
		copy_data(x.begin(), x.end(), begin());
	}

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved. 
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(this_type&& x) :
		size_allocaltor(x.get_allocator())
	{
		swap(x);
	}

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved.
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(this_type&& x, const allocator_type& alloc) :
		size_allocaltor(alloc)
	{
		swap(x);
	}

	/// constructor: initializer list
	/*!
	Constructs a container with a copy of each of the elements in il, in the same order.
	*/
	compact_vector(std::initializer_list<T> il, const allocator_type& alloc = allocator_type()) :
		size_allocaltor(alloc)
	{
		assign(il);
	}

	/// destructor
	~compact_vector()
	{
		destruct();
	}

	/// assign: range
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		clear();
		insert(begin(), first, last);
	}

	/// assign: fill
	void assign(size_t n, const T& val)
	{
		clear();
		resize(n, val);
	}

	/// assign: initializer list
	void assign(std::initializer_list<T> il)
	{
		clear();
		insert(begin(), il);
	}

	T& at(size_t n)
	{
		if (n >= size())
			throw std::out_of_range(u8"compact_vector out of range");

		return (*this)[n];
	}

	const T& at(size_t n) const
	{
		if (n >= size())
			throw std::out_of_range(u8"compact_vector out of range");

		return (*this)[n];
	}

	T& back()
	{
		return (*this)[size() - 1];
	}

	const T& back() const
	{
		return (*this)[size() - 1];
	}

	iterator begin() noexcept
	{
		if (is_compact())
			return compact.get(0);
		else
			return full.get(0);
	}

	const_iterator begin() const noexcept
	{
		if (is_compact())
			return compact.get(0);
		else
			return full.get(0);
	}

	size_t capacity() const noexcept
	{
		if (is_compact())
			return compact_capacity;
		return full.capacity;
	}

	const_iterator cbegin() const noexcept
	{
		if (is_compact())
			return compact.get(0);
		else
			return full.get(0);
	}

	const_iterator cend() const noexcept
	{
		if (is_compact())
			return compact.get(size() - 1);
		else
			return full.get(size() - 1);
	}

	void clear() noexcept
	{
		call_destructors(begin(), end());

		size_allocaltor.set_size(0, is_compact());
	}

	const_reverse_iterator crbegin() const noexcept; // todo

	const_reverse_iterator crend() const noexcept; // todo

	T* data() noexcept
	{
		return begin();
	}

	const T* data() const noexcept
	{
		return begin();
	}

	template <class... Args>
	iterator emplace(const_iterator position, Args&&... args); // todo

	template <class... Args>
	void emplace_back(Args&&... args)
	{
		static const size_t args_size = sizeof...(Args); 

		size_t c = capacity();
		size_t required_size = size() + args_size;
		while (required_size > c) c = 2 * c;
		reserve(c);

		::new(end()) value_type(std::forward<Args>(args)...);
		set_new_size(required_size);
	}

	bool empty() const noexcept
	{
		return size() == 0;
	}

	iterator end() noexcept
	{
		if (is_compact())
			return compact.get(size());
		else
			return full.get(size());
	}

	const_iterator end() const noexcept
	{
		if (is_compact())
			return compact.get(size() - 1);
		else
			return full.get(size() - 1);
	}

	iterator erase(const_iterator position)
	{
		iterator p = const_cast<iterator>(position);
		iterator e = end();

		if (p <= e)
			copy_data(p + 1, e, p);
		e--;
		e->~T();
		set_new_size(size() - 1);
		return p;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		iterator f = const_cast<iterator>(first);
		iterator l = const_cast<iterator>(last);
		iterator e = end();

		if (f > l)
			return nullptr;

		if (l <= e)
			copy_data(l, e, f);
		call_destructors(f + (e - l), e);

		set_new_size(size() - (e - l));
		return f;
	}

	T& front()
	{
		return *begin();
	}

	const T& front() const
	{
		return *begin();
	}

	allocator_type get_allocator() const noexcept
	{
		return *size_allocaltor.get_allocator();
	}

	/// insert: single element
	iterator insert(const_iterator position, const T& val)
	{
		return insert(position, 1, val);
	}

	/// insert: fill
	iterator insert(const_iterator position, size_t n, const T& val)
	{
		size_t p = end() - position;
		resize(size() + n, val);

		iterator i = end() - n - 1;
		iterator j = end() - 1;
		for (; p > 0; p--, i--, j--)
			std::iter_swap(i, j);
		return i;
	}

	/// insert: range
	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last)
	{
		size_t n = 0;
		for (InputIterator i = first; i != last; i++, n++);

		if (n == 0)
			return end();

		size_t p = end() - position;
		resize(size() + n, *first);

		iterator i = end() - n - 1;
		iterator j = end() - 1;
		for (size_t counter = p; counter > 0; counter--, i--, j--)
			std::iter_swap(i, j);

		i = begin() + p;
		for (InputIterator j = first; j != last; j++, i++)
			*i = *j;

		return begin() + p;
	}

	/// insert: move
	iterator insert(const_iterator position, T&& val)
	{
		return emplace(position, std::move(value));
	}

	/// initializer list
	iterator insert(const_iterator position, std::initializer_list<T> il)
	{
		if (il.size() == 0)
			return end();

		size_t p = end() - position;
		resize(size() + il.size(), *il.begin());

		iterator i = end() - il.size() - 1;
		iterator j = end() - 1;
		for (size_t counter = p; counter > 0; counter--, i--, j--)
			std::iter_swap(i, j);

		i = begin() + p;
		for (auto& e : il)
		{
			*i = e;
			i++;
		}

		return begin() + p;
	}

	size_t max_size() const noexcept
	{
		return vector_max_size;
	}

	// operator=, copy
	compact_vector& operator= (const compact_vector& x)
	{
		if (this != &x)
		{
			clear();
			reserve(x.size());
			copy_data(x.begin(), x.end(), begin());
		}
		return *this;
	}

	// operator=, move
	compact_vector& operator= (compact_vector&& x)
	{
		if (this != &x)
		{
			destruct();
			swap(x);
		}
		return *this;
	}

	// operator=, initializer list
	compact_vector& operator= (std::initializer_list<T> il)
	{
		clear();
		reserve(il.size());
		copy_data(il.begin(), il.end(), begin());
		return *this;
	}

	T& operator[] (size_t n)
	{
		if (is_compact())
			return *compact.get(n);
		else
			return *full.get(n);
	}

	const T& operator[] (size_t n) const
	{
		if (is_compact())
			return *compact.get(n);
		else
			return *full.get(n);
	}

	void pop_back()
	{
		resize(size() - 1);
	}

	void push_back(const T& val)
	{
		auto size = size() + 1;
		if (size > capacity())
			reserve(2 * capacity());

		::new(end()) T(val);
		set_new_size(size);
	}

	void push_back(T&& val)
	{
		auto size = size() + 1;
		if (size > capacity())
			reserve(2 * capacity());

		::new(end()) T(std::move(val));
		set_new_size(size);
	}

	reverse_iterator rbegin() noexcept; // todo
	const_reverse_iterator rbegin() const noexcept; // todo

	reverse_iterator rend() noexcept; // todo
	const_reverse_iterator rend() const noexcept; // todo

	void reserve(size_t n)
	{
		if (n <= capacity())
			return;

		if (n > max_size())
			std::length_error(u8"попытка выделить памяти больше чем max_size()");

		grow(n);
	}

	void resize(size_t n)
	{
		if (n > size())
		{
			size_t c = capacity();
			while (n > c) c = 2 * c;
			
			reserve(c);

			add_to_end(n);
		}
		else
		{
			call_destructors(begin() + n, end());
			set_new_size(n);
		}
	}

	void resize(size_t n, const T& val)
	{
		if (n > size())
		{
			size_t c = capacity();
			while (n > c) c = 2 * c;

			reserve(c);
			
			add_to_end(n, val);
		}
		else
		{
			call_destructors(begin() + n, end());
			set_new_size(n);
		}
	}

	void shrink_to_fit()
	{
		if (is_compact())
			return;

		if (size() >= compact_capacity)
		{
			size_t new_capacity = size();
			auto ptr_begin = get_allocator().allocate(new_capacity);
			move_data(begin(), end(), ptr_begin);

			get_allocator().deallocate(full.begin, full.capacity);

			full.begin = ptr_begin;
			full.capacity = new_capacity;
		}
		else
		{
			auto b = full.begin;
			auto c = full.capacity;

			move_data(b, b + c, compact.get(0));
			get_allocator().deallocate(b, c);

			size_allocaltor.set_size(size(), true);
		}
	}

	size_t size() const noexcept
	{
		return this->size_allocaltor.get_size();
	}

	void swap(compact_vector& x)
	{
		if (is_compact())
		{
			if (x.is_compact())
				swap_compact_compact(x);
			else
				swap_compact_full(x);
		}
		else
		{
			if (x.is_compact())
				swap_full_compact(x);
			else
				swap_full_full(x);
		}
	}

#ifdef COMPACT_VECTOR_DEBUG
public:
#else
private:
#endif

	bool is_compact() const noexcept
	{
		return size_allocaltor.is_compact();
	}

	void destruct()
	{
		call_destructors(begin(), end());

		if (!is_compact())
			get_allocator().deallocate(full.begin, full.capacity);
		size_allocaltor.set_size(0, true);
	}

	// swap для случая, когда this->is_compact() == false && x.is_compact() == false
	void swap_full_full(this_type& x)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (is_compact() != false || x.is_compact() != false)
			throw std::exception(u8"неправильный вызов swap_full_full");
#endif // COMPACT_VECTOR_DEBUG

		std::swap(size_allocaltor, x.size_allocaltor);
		std::swap(full.begin, x.full.begin);
		std::swap(full.capacity, x.full.capacity);
	}

	// swap для случая, когда this->is_compact() == true && x.is_compact() == false
	void swap_compact_full(this_type& x)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (is_compact() != true || x.is_compact() != false)
			throw std::exception(u8"неправильный вызов swap_compact_full");
#endif // COMPACT_VECTOR_DEBUG

		auto x_begin = x.full.begin;
		auto x_capacity = x.full.capacity;

		move_data(begin(), end(), x.compact.get(0));
		full.begin = x_begin;
		full.capacity = x_capacity;

		std::swap(size_allocaltor, x.size_allocaltor);
	}

	// swap для случая, когда this->is_compact() == false && x.is_compact() == true
	void swap_full_compact(this_type& x)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (is_compact() != false || x.is_compact() != true)
			throw std::exception(u8"неправильный вызов swap_full_compact");
#endif // COMPACT_VECTOR_DEBUG

		auto this_begin = full.begin;
		auto this_capacity = full.capacity;

		move_data(x.begin(), x.end(), compact.get(0));
		x.full.begin = x_begin;
		x.full.capacity = x_capacity;

		std::swap(size_allocaltor, x.size_allocaltor);
	}

	// swap для случая, когда this->is_compact() == true && x.is_compact() == true
	void swap_compact_compact(this_type& x)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (is_compact() != true || x.is_compact() != true)
			throw std::exception(u8"неправильный вызов swap_compact_compact");
#endif // COMPACT_VECTOR_DEBUG

		swap_compact_compact(x, std::is_trivially_copyable<T>::type());
	}

	void swap_compact_compact(this_type& x, std::integral_constant<bool, true>)
	{
		compact_storage tmp;
		std::memcpy(&tmp, &compact, sizeof(compact_storage));
		std::memcpy(&compact, &x.compact, sizeof(compact_storage));
		std::memcpy(&x.compact, &tmp, sizeof(compact_storage));
		
		std::swap(size_allocaltor, x.size_allocaltor);
	}

	void swap_compact_compact(this_type& x, std::integral_constant<bool, false>)
	{
		size_t s1 = size();
		size_t s2 = x.size();
		size_t s = std::max(s1, s2);

		for (size_t i = 0; i < s; i++)
		{
			if (i < s1 && i < s2)
			{
				std::iter_swap(compact.get(i), x.compact.get(i));
			}
			else if (i < s2)
			{
				::new(compact.get(i)) T(std::move(*x.compact.get(i)));
				x.compact.get(i)->~T();
			}
			else
			{
				::new(x.compact.get(i)) T(std::move(*compact.get(i)));
				compact.get(i)->~T();
			}
		}

		std::swap(size_allocaltor, x.size_allocaltor);
	}

	template<typename InputIterator>
	static void call_destructors(InputIterator first, InputIterator last)
	{
		call_destructors(first, last, std::is_trivial<T>::type());
	}

	// для тривиальных типов деструктор вызывать не нужно
	template<typename InputIterator>
	static void call_destructors(InputIterator first, InputIterator last, std::integral_constant<bool, true>)
	{
	}

	// для нетривиальных типов вызывается деструктор
	template<typename InputIterator>
	static void call_destructors(InputIterator first, InputIterator last, std::integral_constant<bool, false>)
	{
		for (; first != last; first++)
			first->~T();
	}

	void grow(size_t new_size)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (new_size <= capacity())
			throw std::exception(u8"попытка уменьшить размер вектора");
#endif // COMPACT_VECTOR_DEBUG

		auto ptr_begin = get_allocator().allocate(new_size);
		move_data(begin(), end(), ptr_begin);

		if (!is_compact())
			get_allocator().deallocate(full.begin, full.capacity);

		full.begin = ptr_begin;
		full.capacity = new_size;

		if (is_compact())
			this->size_allocaltor.set_size(size(), false);
	}

	void add_to_end(size_t n)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (size() > n)
			throw new std::exception(u8"попытка увеличить вектор на отрицательное число");
#endif // COMPACT_VECTOR_DEBUG

		auto end_ptr = end();
		for (auto i = n - size(); i > 0; i--, end_ptr++)
			::new (end_ptr) T();

		set_new_size(n);
	}

	void add_to_end(size_t n, const T& val)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (size() > n)
			throw new std::exception(u8"попытка увеличить вектор на отрицательное число");
#endif // COMPACT_VECTOR_DEBUG

		auto end_ptr = end();
		for (auto i = n - size(); i > 0; i--, end_ptr++)
			::new (end_ptr) T(val);

		set_new_size(n);
	}

	void set_new_size(size_t new_size)
	{
		this->size_allocaltor.set_size(new_size, is_compact());
	}

	static void move_data(iterator first, iterator last, iterator target)
	{
		move_data(first, last, target, std::is_trivially_copyable<T>::type());
	}

	// для тривиальных типов можно использовать memcpy
	static void move_data(iterator first, iterator last, iterator target, std::integral_constant<bool, true>)
	{
		std::memcpy(target, first, last - first);
	}

	// для нетривиальных типов вызывается std::move
	static void move_data(iterator first, iterator last, iterator target, std::integral_constant<bool, false>)
	{
		for (; first != last; first++, target++)
			::new(target) T(std::move(*first));
		call_destructors(first, last);
	}

	static void copy_data(const_iterator first, const_iterator last, iterator target)
	{
		copy_data(first, last, target, std::is_trivially_copyable<T>::type());
	}

	// для тривиальных типов можно использовать memcpy
	static void copy_data(const_iterator first, const_iterator last, iterator target, std::integral_constant<bool, true>)
	{
		std::memcpy(target, first, last - first);
	}

	// для нетривиальных типов вызывается поэлементное копирование
	static void copy_data(const_iterator first, const_iterator last, iterator target, std::integral_constant<bool, false>)
	{
		for (; first != last; first++, target++)
		{
			*target = *first;
		}
	}
};

