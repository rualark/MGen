#include "../stdafx.h"
#include "VSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

template<typename T>
VSet<T>::VSet()
{
}

template<typename T>
VSet<T>::~VSet()
{
}

template<typename T>
int VSet<T>::Insert(vector<T> const& v)
{
	pair<set<vector<T>>::iterator, bool> p;
	p = s.insert(v);
	return p.second;
}

// Explicit instantiations
template class VSet<int>;
template class VSet<float>;
template class VSet<char>;
template class VSet<double>;
template class VSet<long>;
template class VSet<long long>;
