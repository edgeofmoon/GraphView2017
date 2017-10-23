#pragma once

#include <vector>
#include <cmath>

template<typename T>
class My1DSampler{
public:
	My1DSampler();
	~My1DSampler();

	enum MySampleMethod{
		SAMPLE_CLAMP = 0,
		SAMPLE_LOOP = 1
	};

	enum MyInterpolateMethod{
		INTERPOLATE_NEAREST = 0,
		INTERPOLATE_LINEAR = 1,
		INTERPOLATE_GAMMA = 2
	};

	void SetSampleMethod(MySampleMethod method);
	void SetGamma(float gamma);
	void SetInterpolationMethod(MyInterpolateMethod method);

	// must be ascending order
	void SetValues(const std::vector<T>& values);
	void SetValues(const T& start, const T& end);

	const T operator[](float loc) const;
	const T At(float loc) const;
	float Where(const T& value) const;

protected:
	float toDomain(float loc) const;
	T actualSample(float idxf) const;
	std::vector<T> mValues;
	MySampleMethod mSampleMethod;
	MyInterpolateMethod mInterpolateMethod;
	float mGamma;
};

template<typename T>
My1DSampler<T>::My1DSampler(){
	mSampleMethod = SAMPLE_CLAMP;
	mInterpolateMethod = INTERPOLATE_LINEAR;
	mGamma = 1;
};

template<typename T>
My1DSampler<T>::~My1DSampler(){
};

template<typename T>
void My1DSampler<T>::SetSampleMethod(MySampleMethod method){
	mSampleMethod = method;
}

template<typename T>
void My1DSampler<T>::SetGamma(float gamma){
	mGamma = gamma;
}

template<typename T>
void My1DSampler<T>::SetInterpolationMethod(MyInterpolateMethod method){
	mInterpolateMethod = method;
}

template<typename T>
void My1DSampler<T>::SetValues(const std::vector<T>& values){
	mValues = values;
}
	
template<typename T>
void My1DSampler<T>::SetValues(const T& start, const T& end){
	mValues.clear();
	mValues.reserve(2);
	mValues.push_back(start);
	mValues.push_back(end);
}

template<typename T>
const T My1DSampler<T>::operator[](float loc) const{
	return this->At(loc);
}

template<typename T>
const T My1DSampler<T>::At(float loc) const{
	float domainf = this->toDomain(loc);
	return this->actualSample(domainf);
}

template<typename T>
float My1DSampler<T>::toDomain(float loc) const{
	switch(mSampleMethod){
	case SAMPLE_CLAMP:
		return loc>1.0f?1.0f:(loc<0.f?0.f:loc);
	case SAMPLE_LOOP:
	default:
		return fabs(loc)-(int)(fabs(loc));
	}
}

template<typename T>
T My1DSampler<T>::actualSample(float domainf) const{
	float idxf = domainf*(mValues.size()-1); 
	if(mInterpolateMethod == INTERPOLATE_NEAREST){
		int idx = (int)(idxf+0.5f);
		return mValues[idx];
	}
	else if(mInterpolateMethod == INTERPOLATE_LINEAR){
		int idxLow = (int)idxf;
		if(idxLow >= mValues.size()-1){
			return mValues[idxLow];
		}
		int idxHigh = idxLow+1;
		float diff = idxf-(float)idxLow;
		return mValues[idxLow]*(1-diff)+mValues[idxHigh]*diff;
	}
	else{
		// gamma
		int idxLow = (int)idxf;
		if(idxLow >= mValues.size()-1){
			return mValues[idxLow];
		}
		int idxHigh = idxLow+1;
		float diff = idxf-(float)idxLow;
		return pow(pow(mValues[idxLow],mGamma)*(1-diff)+pow(mValues[idxHigh],mGamma)*diff,1/mGamma);
	}
}

template<typename T>
float My1DSampler<T>::Where(const T& value) const{
	if(value <= mValues.front()){
		return 0.f;
	}
	else if(value >= mValues.back()){
		return 1.0f;
	}
	else{
		int idx;
		for(idx = 0;idx<mValues.size();idx++){
			if(mValues[idx+1]>=value){
				break;
			}
		}
		float fraction = (float)(value-mValues[idx])/(mValues[idx+1]-mValues[idx]);
		return (float)idx/(mValues.size()-1) + fraction;
	}
}

typedef My1DSampler<float> My1DSamplerf;
typedef My1DSampler<int> My1DSampleri;
typedef My1DSampler<double> My1DSamplerd;