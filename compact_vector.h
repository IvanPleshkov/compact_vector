#pragma once

#define COMPACT_VECTOR_DEBUG

#include <cstddef>
#include <memory>
#include <utility>
#include <limits>
#include <type_traits>

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

	static constexpr size_t compact_capacity = compact_max_size < 0 ? compact_default_capacity : compact_max_size;

	static constexpr size_t vector_max_size = std::numeric_limits<size_t>::max() >> 1;

	struct compact_storage
	{
		T buffer[compact_capacity];

		T* get(size_t i) noexcept
		{
			return buffer[i];
		}

		const T* get(size_t i) const noexcept
		{
			return buffer[i];
		}
	};

	// Empty Base Optimization
	// хак оптимизации памяти. Промежуточный класс позволяет не использовать лишнюю память в случае
	// sizeof(allocator_type) == 0
	struct size_allocator_pair : public allocator_type
	{
		size_allocator_pair(const allocator_type& base) :
			size(0),
			allocator_type(base)
		{}

		size_allocator_pair(allocator_type&& base) :
			size(0),
			allocator_type(base)
		{}

		size_allocator_pair() :
			size(0),
			allocator_type(allocator_type())
		{}

		allocator_type* get_allocator()
		{
			return this;
		}

		size_t get_size()
		{
			return size & vector_max_size;
		}

		bool is_compact()
		{
			return !(bool)(size >> (sizeof(size_t) - 1));
		}

		void set_size(size_t new_size, bool is_compact)
		{
#ifdef COMPACT_VECTOR_DEBUG
			if (new_size > vector_max_size)
				throw std::exception(u8"попытка создать вектор больше max_size");
#endif

			size = !is_compact;
			size = size << (sizeof(size_t) - 1);
			size = size | new_size;
		}

	private:
		size_t size;
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
	using reverse_iterator = T*;
	using const_reverse_iterator = const T*;
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
		insert(begin(), first, last);
	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x) :
		size_allocaltor(x.get_allocator())
	{
		
	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x, const allocator_type& alloc) :
		size_allocaltor(alloc)
	{
		// todo
	}

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved. 
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(compact_vector&& x) :
		size_allocaltor(std::move(*x.size_allocaltor.get_allocator()))
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
	compact_vector(compact_vector&& x, const allocator_type& alloc) :
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
		insert(begin(), il);
	}

	/// destructor
	~compact_vector()
	{
		destruct();
	}

	/// assign: range
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);

	/// assign: fill
	void assign(size_t n, const T& val);

	/// assign: initializer list
	void assign(std::initializer_list<T> il);

	T& at(size_t n)
	{
		if (n >= size())
			throw std::out_of_range();

		return (*this)[n];
	}

	const T& at(size_t n) const
	{
		if (n >= size())
			throw std::out_of_range();

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

	const_iterator cbegin() const noexcept;

	const_iterator cend() const noexcept;

	void clear() noexcept
	{
		destruct();
	}

	const_reverse_iterator crbegin() const noexcept;

	const_reverse_iterator crend() const noexcept;

	T* data() noexcept
	{
		return begin();
	}

	const T* data() const noexcept
	{
		return begin();
	}

	template <class... Args>
	iterator emplace(const_iterator position, Args&&... args);

	template <class... Args>
	void emplace_back(Args&&... args);

	bool empty() const noexcept
	{
		return size() == 0;
	}

	iterator end() noexcept
	{
		if (is_compact())
			return compact.get(size() - 1);
		else
			return full.get(size() - 1);
	}

	const_iterator end() const noexcept
	{
		if (is_compact())
			return compact.get(size() - 1);
		else
			return full.get(size() - 1);
	}

	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);

	T& front();
	const T& front() const;

	allocator_type get_allocator() const noexcept
	{
		return *size_allocaltor.get_allocator();
	}

	/// insert: single element
	iterator insert(const_iterator position, const T& val);

	/// insert: fill
	iterator insert(const_iterator position, size_t n, const T& val);

	/// insert: range
	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last);

	/// insert: move
	iterator insert(const_iterator position, T&& val);

	/// initializer list
	iterator insert(const_iterator position, std::initializer_list<T> il);

	size_t max_size() const noexcept
	{
		return vector_max_size;
	}

	// operator=, copy
	compact_vector& operator= (const compact_vector& x);

	// operator=, move
	compact_vector& operator= (compact_vector&& x);

	// operator=, initializer list
	compact_vector& operator= (std::initializer_list<T> il);

	T& operator[] (size_t n)
	{
		if (is_compact())
			return compact.get(n);
		else
			return full.get(n);
	}

	const T& operator[] (size_t n) const
	{
		if (is_compact())
			return compact.get(n);
		else
			return full.get(n);
	}

	void pop_back()
	{
		resize(size() - 1);
	}

	void push_back(const T& val);
	void push_back(T&& val);

	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;

	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;

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
			// todo
		}
		else
		{
			call_destructors(begin() + n, end());
			allocator_type.set_size(n, is_compact());
		}
	}

	void resize(size_t n, const T& val)
	{
		if (n > size())
		{
			// todo
		}
		else
		{
			call_destructors(begin() + n, end());
			allocator_type.set_size(n, is_compact());
		}
	}

	void shrink_to_fit()
	{
		// todo
	}

	size_t size() const noexcept
	{
		return size_allocaltor.get_size();
	}

	void swap(compact_vector& x)
	{
		
	}

//private:
public:

	bool is_compact() noexcept
	{
		return size_allocaltor.is_compact();
	}

	void destruct()
	{
		call_destructors(begin(), end());

		if (!is_compact())
			get_allocator().deallocate(full.begin, full.capacity);
		size_allocaltor.set_size(0, 0);
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
		{
			first->~T();
		}
	}

	void grow(size_t new_size)
	{
#ifdef COMPACT_VECTOR_DEBUG
		if (new_size <= capacity())
			throw std::exception(u8"попытка уменьшить размер вектора");
#endif

		auto ptr_begin = get_allocator().allocate(new_size);
		move_data(begin(), end(), ptr_begin);

		call_destructors(begin(), end());
		if (!is_compact())
			get_allocator().deallocate(full.begin, full.capacity);

		full.begin = ptr_begin;
		full.capacity = new_size;
	}

	template<typename InputIterator>
	static void move_data(InputIterator first, InputIterator last, InputIterator target)
	{
		for (; first != last; first++, target++)
		{
			*target = std::move(*first);
		}
	}
};
