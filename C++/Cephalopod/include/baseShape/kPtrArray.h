#pragma once
#include <vector>
template <class T>
class kPtrArray
{
	typedef T*	TPtr;
public:
	kPtrArray();
#if 0
	kPtrArray(
		unsigned	nSize,
		bool		bAllocate
	);
#endif
	virtual ~kPtrArray();
private:
	std::vector<TPtr>	__element;
public:
	unsigned	length() const				{return (unsigned) __element.size();}
	void		append(TPtr p)				{__element.push_back(p);}
	const TPtr&	operator[](unsigned index) const	{return __element.at(index);};
	TPtr&		operator[](unsigned index)		{return __element.at(index);};
	const T&	operator()(unsigned index) const	{return *((TPtr) __element.at(index));}
	T&		operator()(unsigned index)		{return *((TPtr) __element.at(index));}
	void		pop()					{__element.pop_back();}
	void		clear();
#if 0
	void		setLength(unsigned nSize, bool bAllocate);
#endif
};
template <class T>
kPtrArray<T>::kPtrArray()
{
}
#if 0
template <class T>
kPtrArray<T>::kPtrArray(
	unsigned	nSize,
	bool		bAllocate
)
{
	setLength(nSize, bAllocate);
}
template <class T>
void	kPtrArray<T>::setLength(unsigned nSize, bool bAllocate)
{
	size_t	len = __element.size();
	if (nSize == len)
		return;
	if (nSize < len) {
		for (unsigned i = nSize; i < len; ++i) {
			TPtr&	p = __element.at(i);
			if (p) {
				delete p;
				p = NULL;
			}
		}
		__element.resize(nSize);
		return;
	}
	__element.resize(nSize);
	for (size_t i = len; i < nSize; ++i) {
		TPtr&	p = __element.at(i);
		p = bAllocate ? new T : NULL;
	}
}
#endif
template <class T>
void	kPtrArray<T>::clear()
{
	for (unsigned i = 0; i < __element.size(); ++i) {
		TPtr&	p = __element.at(i);
		if (p) {
			delete p;
			p = NULL;
		}
	}
	__element.clear();
}
template <class T>
kPtrArray<T>::~kPtrArray()
{
	clear();
}
