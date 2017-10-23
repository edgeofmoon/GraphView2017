#pragma once

template<typename T, int d>
class MyIndex{
public:
	MyIndex(){
	};
	MyIndex(const MyIndex& idx){
		this->operator=(idx);
	};
	~MyIndex(){
	}

	T& operator[] (int i) {
		return _d[i];
	};
	const T& operator[] (int i) const{
		return _d[i];
	};

	MyIndex& operator= (const MyIndex& idx) {
		for(int i = 0;i<d;i++)
			_d[i] = idx[i];
		return *this;
	};

protected:
	T _d[d];
};

typedef MyIndex<int,2> MyIndex2i;
typedef MyIndex<int,3> MyIndex3i;
typedef MyIndex<int,4> MyIndex4i;