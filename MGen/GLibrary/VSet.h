#pragma once

// This is a class for set of vectors

template<typename T>
class VSet
{
public:
	VSet();
	~VSet();

	int Insert(vector<T> const& vec);

	set<vector<T>> s;

};

