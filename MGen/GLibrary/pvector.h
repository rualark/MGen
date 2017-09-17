#pragma once

// Standard vector class encapsulation
template<typename T>
class svector 
{
public:
	svector() {}
	svector(size_t size) { v.resize(size); }
	~svector() {}
	void push_back(T value) { v.push_back(value); }
	void resize(size_t size) { v.resize(size); }
	size_t size(T value) { return v.size(); }
	void clear(T value) { v.clear(); }
	T& operator[](size_t index)	{	return v[index]; }

	vector<T> v;
};

// Protected vector class with access counting
template<typename T>
class pvector : public svector<T>
{
public:
	pvector(size_t size) : svector(size) { v_access.resize(size); }
	void push_back(T value) { v_access.push_back(0);  v.push_back(value); }
	T& operator[](size_t index) { 
		v_access[index] = 1; 
		return v[index]; 
	}

	vector<int> v_access;
};

