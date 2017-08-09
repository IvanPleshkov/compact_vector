#ifndef compact_vector_h__
#define compact_vector_h__

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include <limits>

template <
	class T,
	int compact_max_size = -1,
	typename small_size_type = uint8_t,
	class allocator_type = std::allocator<T>>
class compact_vector
{
public:
	struct full_storage
	{
		size_t capacity = 0;
		T* begin = nullptr;
		T* end = nullptr;

		T* get(size_t i)
		{
			return begin + i;
		}

		const T* get(size_t i) const
		{
			return begin + i;
		}
	};

	static constexpr size_t T_size = sizeof(T);

	static constexpr size_t compact_default_capacity = (sizeof(full_storage) - sizeof(small_size_type)) / T_size;

	static constexpr size_t compact_capacity = compact_max_size < 0 ? compact_default_capacity : compact_max_size;

	struct compact_storage
	{
		small_size_type size = 0;
		uint8_t buffer[compact_capacity * sizeof(T)];

		T* get(size_t i)
		{
			return ((T*)buffer) + i;
		}

		const T* get(size_t i) const
		{
			return ((T*)buffer) + i;
		}
	};

	union
	{
		compact_storage compact;
		full_storage full;
	};

	allocator_type allocator;

public:
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = T*;
	using const_reverse_iterator = const T*;

	/// constructor: default
	/*!
	Constructs an empty container, with no elements.
	*/
	explicit compact_vector(const allocator_type& alloc = allocator_type())
	{

	}

	/// constructor: fill
	/*!
	Constructs a container with n elements.
	*/
	explicit compact_vector(size_t n)
	{

	}

	/// constructor: fill
	/*!
	Constructs a container with n elements. Each element is a copy of val.
	*/
	compact_vector(size_t n, const T& val, const allocator_type& alloc = allocator_type())
	{
	
	}

	/// constructor:: range
	/*!
	Constructs a container with as many elements as the range [first,last), 
	with each element emplace-constructed from its corresponding element in that range, in the same order.
	*/
	template <class InputIterator>
	compact_vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
	{
	
	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x)
	{

	}

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x, const allocator_type& alloc)
	{
	
	}

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved. 
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(compact_vector&& x)
	{
	
	}

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved.
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(compact_vector&& x, const allocator_type& alloc)
	{
	
	}

	/// constructor: initializer list
	/*!
	Constructs a container with a copy of each of the elements in il, in the same order.
	*/
	compact_vector(std::initializer_list<T> il, const allocator_type& alloc = allocator_type())
	{
	
	}

	/// destructor
	~compact_vector()
	{
		clear();
	}

	/// assign: range
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);

	/// assign: fill
	void assign(size_t n, const T& val);

	/// assign: initializer list
	void assign(std::initializer_list<T> il);

	T& at(size_t n);
	const T& at(size_t n) const;

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
	}

	const_iterator begin() const noexcept
	{
	}

	size_t capacity() const noexcept;

	const_iterator cbegin() const noexcept;

	const_iterator cend() const noexcept;

	void clear() noexcept
	{}

	const_reverse_iterator crbegin() const noexcept;

	const_reverse_iterator crend() const noexcept;

	T* data() noexcept;
	const T* data() const noexcept;

	template <class... Args>
	iterator emplace(const_iterator position, Args&&... args);

	template <class... Args>
	void emplace_back(Args&&... args);

	bool empty() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;

	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);

	T& front();
	const T& front() const;

	allocator_type get_allocator() const noexcept;

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

	size_t max_size() const noexcept;

	// operator=, copy
	compact_vector& operator= (const compact_vector& x);

	// operator=, move
	compact_vector& operator= (compact_vector&& x);

	// operator=, initializer list
	compact_vector& operator= (std::initializer_list<T> il);

	T& operator[] (size_t n);
	const T& operator[] (size_t n) const;

	void pop_back();

	void push_back(const T& val);
	void push_back(T&& val);

	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;

	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;

	void reserve(size_t n);

	void resize(size_t n);
	void resize(size_t n, const T& val);

	void shrink_to_fit();

	size_t size() const noexcept;

	void swap(compact_vector& x);

//private:
public:

	bool is_compact()
	{
		return ((uint8_t*)this)[0] & uint8_t(128);
	}

	void set_compact_flag()
	{
		((uint8_t*)this)[0] = ((uint8_t*)this)[0] | uint8_t(128);
	}
};

#endif // compact_vector_h__
