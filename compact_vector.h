#ifndef compact_vector_h__
#define compact_vector_h__

#include <cstddef>		// std::size_t, std::ptrdiff_t
#include <memory>		// std::allocator
#include <utility>		// std::move

template < 
	class T,
	size_t compact_max_count = -1,
	class allocator_type = std::allocator<T> >
class compact_vector
{
public:
	typename T& reference;
	typename const T& const_reference;

	/// constructor: default
	/*!
	Constructs an empty container, with no elements.
	*/
	explicit compact_vector(const allocator_type& alloc = allocator_type());

	/// constructor: fill
	/*!
	Constructs a container with n elements.
	*/
	explicit compact_vector(size_t n);

	/// constructor: fill
	/*!
	Constructs a container with n elements. Each element is a copy of val.
	*/
	compact_vector(size_t n, const_reference val, const allocator_type& alloc = allocator_type());

	/// constructor:: range
	/*!
	Constructs a container with as many elements as the range [first,last), 
	with each element emplace-constructed from its corresponding element in that range, in the same order.
	*/
	template <class InputIterator>
	compact_vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type());

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x);

	/// constructor: copy
	/*!
	Constructs a container with a copy of each of the elements in x, in the same order.
	*/
	compact_vector(const compact_vector& x, const allocator_type& alloc);

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved. 
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(compact_vector&& x);

	/// constructor: move
	/*!
	Constructs a container that acquires the elements of x.
	If alloc is specified and is different from x's allocator, the elements are moved.
	Otherwise, no elements are constructed (their ownership is directly transferred).
	x is left in an unspecified but valid state.
	*/
	compact_vector(compact_vector&& x, const allocator_type& alloc);

	/// constructor: initializer list
	/*!
	Constructs a container with a copy of each of the elements in il, in the same order.
	*/
	compact_vector(std::initializer_list<T> il, const allocator_type& alloc = allocator_type());

	/// destructor
	~compact_vector();

	/// assign: range
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);

	/// assign: fill
	void assign(size_t n, const T& val);

	/// assign: initializer list
	void assign(std::initializer_list<T> il);

	reference at(size_t n);
	const_reference at(size_t n) const;

	reference back();
	const_reference back() const;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;

	size_t capacity() const noexcept;

	const_iterator cbegin() const noexcept;

	const_iterator cend() const noexcept;

	void clear() noexcept;

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

	reference front();
	const_reference front() const;

	allocator_type get_allocator() const noexcept;

	/// insert: single element
	iterator insert(const_iterator position, const_reference val);

	/// insert: fill
	iterator insert(const_iterator position, size_type n, const_reference val);

	/// insert: range
	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last);

	/// insert: move
	iterator insert(const_iterator position, T&& val);

	/// initializer list
	iterator insert(const_iterator position, initializer_list<T> il);

	size_type max_size() const noexcept;

	// operator=, copy
	compact_vector& operator= (const compact_vector& x);

	// operator=, move
	compact_vector& operator= (compact_vector&& x);

	// operator=, initializer list
	compact_vector& operator= (initializer_list<T> il);

	reference operator[] (size_type n);
	const_reference operator[] (size_type n) const;

	void pop_back();

	void push_back(const_reference val);
	void push_back(T&& val);

	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;

	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;

	void reserve(size_type n);

	void resize(size_type n);
	void resize(size_type n, const_reference val);

	void shrink_to_fit();

	size_type size() const noexcept;

	void swap(compact_vector& x);

private:
};

#endif // compact_vector_h__
