#ifndef ONE_SIDED_H
#define ONE_SIDED_H

#include <initializer_list>
#include <iterator>

template <typename T>
struct Link{
	Link();
	Link(T);

	T value;
	Link<T>* next_element;	
};


template <typename T>
class Chain {
public:
	Chain(); // empty														 
	Chain(std::initializer_list<T>);										 
	Chain(const Chain<T>&); // create identical chain copy					 
	Chain(Chain<T>&&); // pass content from chain to new one				 
	Chain(T*, int);															 
	template<int N>
	Chain(T(&)[N]);// initialize from array O_O								 
	~Chain();																 
																			 
	Chain<T>& operator = (Chain<T>&&);										 
	Chain<T>& operator = (const Chain<T>&);									 

	void push_front(T);														 
	void push_back(T);														 
	void insert(T, int);													 
	void insert(Chain<T>& const, int); // inserts a copy
    //void insert(Chain<T>&&, int);
	void erase(int);														 
	void swap(Chain<T>&);													 

	int length() const;														 
	bool is_empty() const;													 

	const T& operator[](int) const;											 

	class iterator;

	iterator begin() const;
	iterator end() const;

	class OutOfRange {};
private:
	using PLink = Link<T>*;

	PLink first_element;
	PLink last_element;
	PLink zero_element; // fictious end-element ~ \0
	int size;

	PLink get_ptr_to(int) const; // get pointer to Link<> by position
	void check_range(int) const; // throws

	void clean_up_to_zero(); // destructive activity: delete chain exc zero								 
	void reduce_to_zero(); // first = last = zero
};

template<typename T>
class Chain<T>::iterator:
	public std::iterator<std::forward_iterator_tag, T>
{	
public:
	T& operator*() const;
	PLink operator->() const;
	iterator& operator++();

	bool operator==(const iterator&) const;
	bool operator!=(const iterator&) const;

	class CantIncrementNullptr {};
	class CantDereferenceNullptr {};
private:
	friend class Chain<T>;
	PLink element;
	iterator(PLink);
};

template <typename T>
Link<T>::Link() :
	Link(T())
{}

template <typename T>
Link<T>::Link(T v) :
	value(v),
	next_element(nullptr)
{}

template<typename T>
inline Chain<T>::Chain() :
	zero_element(new Link<T>())
{
	reduce_to_zero();
}

template<typename T>
inline Chain<T>::Chain(std::initializer_list<T> list) :
	Chain()
{
	for (const T& elem : list)
		push_back(elem);
}

template<typename T>
inline Chain<T>::Chain(const Chain<T>& other):
	Chain()
{
	if (other.is_empty())
		return;

	for (iterator it = other.begin(); it != other.end(); ++it)
		push_back(*it);
}

template<typename T>
inline Chain<T>::Chain(Chain<T>&& other) :
	first_element(other.first_element),
	last_element(other.last_element),
	zero_element(other.zero_element),
	size(other.size)
{
    other.zero_element = new Link<T>();
	other.reduce_to_zero();
}

template<typename T> template<int N>
inline Chain<T>::Chain(T(&arr)[N]) {
	T* iter = arr;
	for (int i = 0; i < N; ++i)
		push_back(*iter++);
}

template<typename T>
inline Chain<T>::~Chain(){
	clean_up_to_zero();
	delete zero_element;
}

template<typename T>
inline Chain<T>& Chain<T>::operator=(Chain<T>&& other){
	clean_up_to_zero();
    reduce_to_zero();

	first_element = other.first_element;
	last_element  = other.last_element;
	last_element->next_element = zero_element;
	
	size = other.size;

	other.reduce_to_zero();

	return *this;
}

template<typename T>
inline Chain<T>& Chain<T>::operator=(const Chain<T>& const other){
    return this* = static_cast<Chain<T>&&>(Chain<T>(other));
}

template<typename T> 
inline Chain<T>::Chain(T* arr, int n):
	Chain()
{
	if (n < 0)
		throw exception("bad range");

	for (int i = 0; i < n; ++i)
		push_back(*arr++);
}

template<typename T>
inline void Chain<T>::push_front(T v){
	PLink other = new Link<T>(v);
	other->next = first_element;
	first_element = other;
	++size;
}

template<typename T>
inline void Chain<T>::push_back(T v){
	PLink other = new Link<T>(v);
	other->next_element = zero_element;

	if (is_empty())
		first_element = last_element = other;
	else {
		last_element->next_element = other;
		last_element = other;
	}

	++size;
}

template<typename T>
Link<T>* Chain<T>::get_ptr_to(int pos) const{	
	PLink iter = first_element;

	for (int i = 0; i < pos; ++i)
		iter = iter->next_element;

	return iter;
}

template<typename T>
inline void Chain<T>::check_range(int pos) const
{
	if (pos < 0 || pos > size-1)
		throw OutOfRange();
}

template<typename T>
inline void Chain<T>::clean_up_to_zero(){
	if (is_empty())
		return;
	else
		while (first_element != zero_element) {
			PLink killer = first_element;
            if (!killer)
                break;
			first_element = killer->next_element;
				delete killer;
		}
	reduce_to_zero();
}

template<typename T>
inline void Chain<T>::reduce_to_zero(){
	first_element = last_element = zero_element;
	size = 0;
}

template<typename T>
inline void Chain<T>::insert(T v, int pos)
{
	PLink other = new Link<T>(v);

	if (pos == 0) 
	{
		other->next_element = first_element;
		first_element = other;
	}
	else
	if (0 < pos && pos <= size) 
	{
		PLink iter = get_ptr_to(pos - 1);
		other->next_element = iter->next_element;
		iter->next_element = other;
	}
	else
		throw OutOfRange();

	++size;
}

template<typename T>
inline void Chain<T>::insert(Chain<T>& const other, int pos){
	if (other.is_empty())
		return;

	Chain<T> other_copy(other);

	if (pos == 0) {
		other_copy.last_element->next_element = first_element;
		first_element = other_copy.first_element;
	}
	else if (0 < pos && pos <= size) {
		PLink iter = get_ptr_to(pos - 1);
		other_copy.last_element->next_element = iter->next_element;
		iter->next_element = other_copy.first_element;
	}
	else
		throw OutOfRange();

    other_copy.reduce_to_zero();
	
	size += other_copy.size;
}

//template<typename T>
//inline void Chain<T>::insert(Chain<T>&& other, int pos){
//    if (other.is_empty())
//        return;
//
//    if (pos == 0) {
//        other.last_element->next_element = first_element;
//        first_element = other.first_element;
//    }
//    else if (0 < pos && pos <= size) {
//        PLink iter = get_ptr_to(pos - 1);
//        other.last_element->next_element = iter->next_element;
//        iter->next_element = other.first_element;
//    }
//    else
//        throw OutOfRange();
//
//    other_copy.reduce_to_zero();
//
//    size += other_copy.size;
//}

template<typename T>
inline void Chain<T>::erase(int pos){
	if (is_empty())
		return;

	check_range(pos);

	PLink killer;
	if (pos == 0) {
		killer = first_element;
		first_element = first_element->next_element;
	}
	else {
		PLink prekil = get_ptr_to(pos - 1);
		killer = prekil->next_element;
		prekil->next_element = killer->next_element;
	}
	delete killer;

	--size;
    if (is_empty())
        reduce_to_zero();
}

template<typename T>
inline void Chain<T>::swap(Chain<T>& another){
	Chain temp = std::move(another);
	another = std::move(*this);
	*this = std::move(temp);
}

template<typename T>
inline int Chain<T>::length() const{
	return size;
}

template<typename T>
inline bool Chain<T>::is_empty() const{
	return size == 0;
}

template<typename T>
inline const T& Chain<T>::operator[](int pos)const{
	check_range(pos);
	PLink iter = get_ptr_to(pos);
	return iter->value;
}

template<typename T>
inline typename Chain<T>::iterator Chain<T>::begin() const{
	return iterator(first_element);
}

template<typename T>
inline typename Chain<T>::iterator Chain<T>::end() const{
	return iterator(zero_element);
}

template<typename T>
inline Chain<T>::iterator::iterator(PLink other):
	element(other)
{}

template<typename T>
inline typename T& Chain<T>::iterator::operator*() const
{
	if (!element)
		throw CantDereferenceNullptr();
	return element->value;
}

template<typename T>
Link<T>* Chain<T>::iterator::operator->() const
{
	return element;
}

template<typename T>
inline bool Chain<T>::iterator::operator==(const iterator& other) const
{
	return element == other.element;
}

template<typename T>
inline bool Chain<T>::iterator::operator!=(const iterator& other) const
{
	return element != other.element;
}

template<typename T>
inline typename Chain<T>::iterator& Chain<T>::iterator::operator++()
{
	if (!element)
		throw CantIncrementNullptr();
	element = element->next_element;
	return *this;
}
#endif