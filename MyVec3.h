#ifndef MyVec3_h
#define MyVec3_h

#include <cmath>

template<typename T>
class MyVec3{
public:
    MyVec3(T _x,T _y,T _z):x(_x),y(_y),z(_z){};
    MyVec3() {x = y = z = 0.0;};
	
	template<typename T>
    friend MyVec3<T> operator+(const MyVec3<T> &a,const MyVec3<T> &b){
        MyVec3<T> t(a.x+b.x,a.y+b.y,a.z+b.z);
        return t;
    };
	
	template<typename T>
    friend MyVec3<T> operator-(const MyVec3<T> &a,const MyVec3<T> &b){
        return MyVec3<T>(a.x-b.x,a.y-b.y,a.z-b.z);
    }
	
	template<typename T>
    friend T operator*(const MyVec3<T> &a,const MyVec3<T> &b){
        return a.dotMultiply(b);
    }
	
	template<typename T>
    friend MyVec3<T> operator^(const MyVec3<T> &a,const MyVec3<T> &b){
        return a.crossMultiply(b);
    }
	
	template<typename T>
    friend MyVec3<T> operator*(const MyVec3<T> &a,const T b){
        return MyVec3<T>(a.x*b,a.y*b,a.z*b);
    }
	
	template<typename T>
    friend MyVec3<T> operator*(const T b, const MyVec3<T> &a){
        return MyVec3<T>(a.x*b,a.y*b,a.z*b);
    }
	
	template<typename T>
    friend MyVec3<T> operator/(const MyVec3<T> &a,const T n){
        return MyVec3<T>(a.x/n,a.y/n,a.z/n);
    }
/*
    friend MyVec3<T> operator=(MyVec3<T> &a, const MyVec3<T> &b){
        a.x = b.x;
        a.y = b.y;
        a.z = b.z;
        return a;
    }
*/
	template<typename T>
    friend bool operator==(const MyVec3<T> &a, const MyVec3<T> &b){
        if(a.x != b.x) return false;
        if(a.y != b.y) return false;
        if(a.z != b.z) return false;
        return true;
    }

    static bool isIntersected(const MyVec3<T> &a1, const MyVec3<T> &b1, const MyVec3<T> &a2, const MyVec3<T> &b2){
        MyVec3<T> t1 = (b1-a1).crossMultiply(a2-a1);
        MyVec3<T> t2 = (b1-a1).crossMultiply(b2-a1);
        if (t1*t2 >= 0) return false;

        MyVec3<T> t3 = (b2-a2).crossMultiply(a1-a2);
                MyVec3<T> t4 = (b2-a2).crossMultiply(b1-a2);
                if (t3*t4 >= 0) return false;

        return true;
    }

    static bool isParallel(const MyVec3<T> &a, const MyVec3<T> &b){
        return a.crossMultiply(b) == MyVec3<T>(0.0,0.0,0.0);
    }

	void set(T _x, T _y, T _z){
		x = _x;
		y = _y;
		z = _z;
	}

    MyVec3<T> operator-() const{
        return MyVec3<T>(-x,-y,-z);
    }

    void operator+=(const MyVec3<T> &other){
        x+=other.x;
        y+=other.y;
        z+=other.z;
    };

    void operator*=(const T n){
        x*=n;
        y*=n;
        z*=n;
    }

    void operator/=(const T n){
        x/=n;
        y/=n;
        z/=n;
    }

	const T& operator[](int i) const{
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			return x;
		}
	}
		
	T& operator[](int i){
		switch(i){
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			return x;
		}
	}

    MyVec3<T> operator=(const MyVec3<T> &other){
                x=other.x;
                y=other.y;
                z=other.z;
        return *this;
        };

    T dotMultiply(const MyVec3<T> &other) const{
        return x*other.x+y*other.y+z*other.z;
    }

    MyVec3<T> crossMultiply(const MyVec3<T> &other) const{
        MyVec3<T> t;
        t.x = y*other.z-z*other.y;
        t.y = -(x*other.z-z*other.x);
        t.z = x*other.y-y*other.x;
        return t;
    }

    void scale(const MyVec3<T> &_size){
        x *= _size.x;
        y *= _size.y;
        z *= _size.z;
    }

    void normalize(){
        T n = norm();
        if(n != 0) (*this)/=n;
    }

    MyVec3<T> normalized() const{
        T n = norm();
        if(n != 0) return (*this)/n;
        return (*this);
    }

    T norm() const{
        return sqrt(squared());
    }

    T squared() const{
        return pow(x,2)+pow(y,2)+pow(z,2);
    }

public:
    T x;
    T y;
    T z;
};

typedef MyVec3<float> MyVec3f;

#endif //MyVec3_h
