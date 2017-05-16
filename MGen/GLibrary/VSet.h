#pragma once

// This is a class for set of vectors

template<typename T>
class VSet
{
public:
	VSet();
	~VSet();

	int Insert(vector<T> const& vec);
	int size() { return s.size(); }
	typename set<vector<T>>::iterator begin() { return s.begin(); }
	typename set<vector<T>>::iterator end() { return s.end(); }

	set<vector<T>> s;
	typename set<vector<T>>::iterator i;

};

