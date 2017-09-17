#pragma once

// Set debug level or comment to disable debug
#define DEBUG_PVECTOR 2

// Protected vector class with access analysis
template<typename T>
class pvector
{
public:
	pvector() {}
	pvector(size_t size) { 
		v.resize(size); 
#ifdef DEBUG_PVECTOR
		v_accessed.resize(size); 
#endif
	}

	size_t size() { return v.size(); }

	void push_back(T value) {
#if DEBUG_PVECTOR == 2
		v_resized = 1;
		v_accessed.push_back(0);
#endif
		v.push_back(value);
	}

	void resize(size_t size) { 
#ifdef DEBUG_PVECTOR
		v_resized = 1;
		v_accessed.resize(size);
#endif
		v.resize(size);
	}

	void resize(size_t size, T value) { 
#ifdef DEBUG_PVECTOR
		v_resized = 1;
		v_accessed.resize(size);
#endif
		v.resize(size, value);
	}

	void clear() { 
#ifdef DEBUG_PVECTOR
		v_resized = 1;
		v_accessed.clear();
#endif
		v.clear();
	}

	T& operator[](size_t index) {
#if DEBUG_PVECTOR == 2
		v_accessed[index] = 1;
#endif
		return v[index];
	}

	void fill(T value) {
#if DEBUG_PVECTOR == 2
		std::fill(v_accessed.begin(), v_accessed.end(), 1);
#endif
		std::fill(v.begin(), v.end(), value);
	}

	T accumulate() {
#if DEBUG_PVECTOR == 2
		std::fill(v_accessed.begin(), v_accessed.end(), 1);
#endif
		return std::accumulate(v.begin(), v.end(), (T)0);
	}

	bool operator==(const pvector<T>& other) {
#if DEBUG_PVECTOR == 2
		std::fill(v_accessed.begin(), v_accessed.end(), 1);
#endif
		return v == other.v;
	}

	bool operator!=(const pvector<T>& other) {
		return !(*this == other);
	}

	void reset_accessed(int x1, int x2) {
		for (int x = x1; x < x2; ++x) {
			v_accessed[x] = 0;
		}
	}

	bool was_accessed(int x1, int x2) {
		for (int x = x1; x < x2; ++x) {
			if (v_accessed[x]) return TRUE;
		}
	}

	vector<T> v;
#ifdef DEBUG_PVECTOR
	// Which elements of vector were accessed
	vector<int> v_accessed;
	// If number of vector elements was changed
	int v_resized = 0;
#endif
};

