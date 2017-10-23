#pragma once

#include "My1DSampler.h"

#include <vector>
#include <cmath>

template<typename DomainType, typename RangeType>
class MyScale
{
public:
	MyScale(void);
	~MyScale(void);

	void SetDomain(const std::vector<DomainType>& domain);
	void SetRange(const std::vector<RangeType>& range);
	void SetDomain(const DomainType& domainStart, const DomainType& domainEnd);
	void SetRange(const RangeType& rangeStart,const RangeType& rangeEnd);
	const RangeType operator[] (const DomainType& key) const;

	const My1DSampler<DomainType>& GetDomain() const {return mDomain;};
	const My1DSampler<RangeType>& GetRange() const {return mRange;};
	My1DSampler<DomainType>& GetDomain(){return mDomain;};
	My1DSampler<RangeType>& GetRange(){return mRange;};

protected:
	My1DSampler<DomainType> mDomain;
	My1DSampler<RangeType> mRange;
};


template<typename DomainType, typename RangeType>
MyScale<typename DomainType, typename RangeType>::MyScale(void){
}

template<typename DomainType, typename RangeType>
MyScale<typename DomainType, typename RangeType>::~MyScale(void){
}

template<typename DomainType, typename RangeType>
void MyScale<typename DomainType, typename RangeType>::SetDomain(const std::vector<DomainType>& domain){
	mDomain.SetValues(domain);
}

template<typename DomainType, typename RangeType>
void MyScale<typename DomainType, typename RangeType>::SetRange(const std::vector<RangeType>& range){
	mRange.SetValues(range);
}

template<typename DomainType, typename RangeType>
void MyScale<typename DomainType, typename RangeType>::SetDomain(const DomainType& domainStart, const DomainType& domainEnd){
	mDomain.SetValues(domainStart,domainEnd);
}
	
template<typename DomainType, typename RangeType>
void MyScale<typename DomainType, typename RangeType>::SetRange(const RangeType& rangeStart,const RangeType& rangeEnd){
	mRange.SetValues(rangeStart,rangeEnd);
}

// use a linear interpolation
template<typename DomainType, typename RangeType>
const RangeType MyScale<typename DomainType, typename RangeType>::operator[] (const DomainType& key) const{
	/*
	int domainIdxi;
	for(domainIdxi = 0;domainIdxi<mDomain.size()-1;domainIdxi++){
		if(key>=mDomain[domainIdxi] && key<=mDomain[domainIdxi+1]){
			break;
		}
	}
	float Ddiff = (key-mDomain[domainIdxi])/(mDomain[domainIdxi+1]-mDomain[domainIdxi]);
	float domainIdxf = (float)domainIdxi+Ddiff;
	float rangeIdxf = (float)domainIdxf/mDomain.size()*mRange.size();
	int floorIdx = floor(rangeIdxf);
	int ceilIdx = ceil(rangeIdxf);
	ceilIdx = ceilIdx>mRange.size()-1?mRange.size()-1:ceilIdx;
	float Rdiff = rangeIdxf-(float)floorIdx;
	return (1-Rdiff)*mRange[floorIdx]+Rdiff*mRange[ceilIdx];
	*/
	float idxf = mDomain.Where(key);
	return mRange[idxf];
}