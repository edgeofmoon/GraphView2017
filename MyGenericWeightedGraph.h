#include "MyGenericGraph.h"


#define MyEdgeType MyVec<MyNodeType, 2>
#define MyNodeArray MyArray<MyNodeType>
#define MyEdgeArray MyArray<MyEdgeType>
#define MyWeightedGraphType MyGenericWeightedGraph<MyNodeType, WeightType>

template<typename MyNodeType, typename WeightType>
class MyGenericWeightedGraph
	:public MyGenericGraph<MyNodeType>
{
public:
	MyGenericWeightedGraph();
	~MyGenericWeightedGraph();

	virtual void Copy(const MyWeightedGraphType* graph) = 0;

	virtual const WeightType GetEdgeWeight(const MyEdgeType& edge) const = 0;
	virtual const WeightType GetNodeWeight(const MyNodeType& node) const = 0;
	virtual const WeightType GetEdgeWeight(int edgeIdx) const = 0;
	virtual const WeightType GetNodeWeight(int nodeIdx) const = 0;
	
	virtual void SetNodeWeight(WeightType weight, const MyNodeType& node) = 0;
	virtual void SetEdgeWeight(WeightType weight, const MyEdgeType& edge) = 0;
	virtual void SetNodeWeight(WeightType weight, int nodeIdx) const = 0;
	virtual void SetEdgeWeight(WeightType weight, int edgeIdx) const = 0;
};

#undef MyEdgeType
#undef MyNodeArray
#undef MyEdgeArray
#undef MyWeightedGraphType

typedef MyGenericWeightedGraph<std::string, float> MyGenericWeightedGraphf;