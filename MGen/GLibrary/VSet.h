#pragma once

// This is a class for set of vectors

template<typename T>
class VSet
{
public:
	VSet() {}
	~VSet() {}

	int Insert(vector<T> const& v) {
		pair<set<vector<T>>::iterator, bool> p;
		p = s.insert(v);
		return p.second;
	}

	int size() { return s.size(); }
	typename set<vector<T>>::iterator begin() { return s.begin(); }
	typename set<vector<T>>::iterator end() { return s.end(); }
	void clear() { s.clear(); }

	set<vector<T>> s;
	typename set<vector<T>>::iterator i;

};

