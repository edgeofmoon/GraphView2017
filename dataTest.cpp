

#include "MyDenseGraph.h"
#include "MyGraphFilter.h"
#include "MyDataLoader.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyMatrix.h"

float LINKDENSITY = 0.05 ;
int NUMNODE = 74;
int NUMGRAPH = 27;

MyArrayi GetNumberLinkForLobe(const MyGenericGraphf* graph,
							  const MyGenericNode* hierarchy){
	MyArrayi node2lobe(graph->GetNumNodes(), -1);
	for (int i = 0; i<hierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = hierarchy->GetChild(i);
		MyArrayStr nodesStr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
		for (int j = 0; j < nodesStr.size(); j++){
			int node = nodesStr[j].ToInt();
			node2lobe[node] = i;
		}
	}

	for (int i = 0; i < node2lobe.size(); i++){
		assert(node2lobe[i] >= 0);
	}
	int numLobes = hierarchy->GetNumChildren();
	MyArrayi lobeEdges0(numLobes, 0);

	for(int i = 0;i<graph->GetNumEdges();i++){
		MyVec2i edge = graph->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeEdges0[lobe0] ++;
		}
		else{
			lobeEdges0[lobe0] ++;
			lobeEdges0[lobe1] ++;
		}
	}
	return lobeEdges0;
}
MyArrayi GetNumberLinkChangeForLobe(const MyGenericGraphf* graph0, 
							  const MyGenericGraphf* graph1, 
							  const MyGenericNode* hierarchy){
	MyArrayi node2lobe(graph0->GetNumNodes(), -1);
	for (int i = 0; i<hierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = hierarchy->GetChild(i);
		MyArrayStr nodesStr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
		for (int j = 0; j < nodesStr.size(); j++){
			int node = nodesStr[j].ToInt();
			node2lobe[node] = i;
		}
	}

	for (int i = 0; i < node2lobe.size(); i++){
		assert(node2lobe[i] >= 0);
	}
	int numLobes = hierarchy->GetNumChildren();
	MyArray<MyArray2i> lobeEdges0(numLobes);
	MyArray<MyArray2i> lobeEdges1(numLobes);

	for(int i = 0;i<graph0->GetNumEdges();i++){
		MyVec2i edge = graph0->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeEdges0[lobe0] << edge;
		}
		else{
			lobeEdges0[lobe0] << edge;
			lobeEdges0[lobe1] << edge;
		}
	}

	for(int i = 0;i<graph1->GetNumEdges();i++){
		MyVec2i edge = graph1->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeEdges0[lobe0] << edge;
		}
		else{
			lobeEdges1[lobe0] << edge;
			lobeEdges1[lobe1] << edge;
		}
	}

	MyArrayi linkChange;
	for(int i = 0;i<numLobes;i++){
		MyArray2i *commonEdges = MyArray2i::MakeCommonElementArray(lobeEdges0[i],lobeEdges1[i]);
		int change = (lobeEdges0[i].size()-commonEdges->size())
			+(lobeEdges1[i].size()-commonEdges->size());
		delete commonEdges;
		linkChange << change;
	}
	return linkChange;
}

MyArrayi GetLinkNumberChangeForLobe(const MyGenericGraphf* graph0, 
							  const MyGenericGraphf* graph1, 
							  const MyGenericNode* hierarchy){
	MyArrayi node2lobe(graph0->GetNumNodes(), -1);
	for (int i = 0; i<hierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = hierarchy->GetChild(i);
		MyArrayStr nodesStr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
		for (int j = 0; j < nodesStr.size(); j++){
			int node = nodesStr[j].ToInt();
			node2lobe[node] = i;
		}
	}

	for (int i = 0; i < node2lobe.size(); i++){
		assert(node2lobe[i] >= 0);
	}

	MyArrayi lobeNumEdge0(hierarchy->GetNumChildren(), 0);
	MyArrayi lobeNumEdge1(hierarchy->GetNumChildren(), 0);

	for (int i = 0; i < graph0->GetNumEdges(); i++){
		MyVec2i edge = graph0->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeNumEdge0[lobe0] ++;
		}
		else{
			lobeNumEdge0[lobe0] ++;
			lobeNumEdge0[lobe1] ++;
		}
	}

	for (int i = 0; i < graph1->GetNumEdges(); i++){
		MyVec2i edge = graph1->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeNumEdge1[lobe0] ++;
		}
		else{
			lobeNumEdge1[lobe0] ++;
			lobeNumEdge1[lobe1] ++;
		}
	}

	MyArrayi rst;
	for (int i = 0; i < hierarchy->GetNumChildren(); i++){
		int change = lobeNumEdge0[i] - lobeNumEdge1[i];
		rst << change;
	}

	return rst;
}

MyArrayi GetVariationDistribution(const MyArrayStr& matrixFiles, 
								  const MyString& meanMatricFile,
								  const MyString& hierarchyFile){
	MyMatrixf* meanMat = MyDataLoader::MakeMatrixFromFile(meanMatricFile,NUMNODE, NUMNODE);
	MyDenseGraphf* meanGraph = new MyDenseGraphf;
	meanGraph->SetMatrix(meanMat);
	MyGraphFilter filter;
	filter.SetInputGraph(meanGraph);
	MyGenericGraphf* filteredMeanGraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	delete meanGraph;

	MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFile);

	MyArrayi linkChanges;
	for(int i = 0;i<matrixFiles.size();i++){
		MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE, NUMNODE);
		MyDenseGraphf* graph = new MyDenseGraphf;
		graph->SetMatrix(mat);
		filter.SetInputGraph(graph);
		MyGenericGraphf* filteredgraph
			= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
		//MyArrayi linkChange = GetNumberLinkChangeForLobe(filteredMeanGraph,filteredgraph, hierarchy);
		MyArrayi linkChange = GetLinkNumberChangeForLobe(filteredMeanGraph,filteredgraph, hierarchy);
		linkChanges += linkChange;
		delete filteredgraph;
		delete graph;
	}

	delete hierarchy;
	delete filteredMeanGraph;

	return linkChanges;
}

MyArrayi GetVariationDifferenceDistribution(const MyArrayStr& matrixFiles, 
											const MyString& meanMatricFile,
											const MyString& hierarchyFile){
	MyMatrixf* meanMat = MyDataLoader::MakeMatrixFromFile(meanMatricFile,NUMNODE, NUMNODE);
	MyDenseGraphf* meanGraph = new MyDenseGraphf;
	meanGraph->SetMatrix(meanMat);
	MyGraphFilter filter;
	filter.SetInputGraph(meanGraph);
	MyGenericGraphf* filteredMeanGraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	delete meanGraph;
	
	MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFile);
	
	MyArrayi varDiff;
	for(int i = 0;i<matrixFiles.size();i++){
		MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE, NUMNODE);
		MyDenseGraphf* graph = new MyDenseGraphf;
		graph->SetMatrix(mat);
		filter.SetInputGraph(graph);
		MyGenericGraphf* filteredgraph
			= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
		//MyArrayi linkChange = GetNumberLinkChangeForLobe(filteredMeanGraph,filteredgraph, hierarchy);
		MyArrayi linkChange = GetLinkNumberChangeForLobe(filteredMeanGraph,filteredgraph, hierarchy);
		for( int ifrom = 0;ifrom < linkChange.size();ifrom++){
			int varFrom = linkChange[ifrom];
			if (varFrom < 0) varFrom = -varFrom;
			for( int ito = ifrom +1; ito< linkChange.size(); ito ++){
				int varTo = linkChange[ito];
				if(varTo<0) varTo = -varTo;
				varDiff << (varFrom-varTo);
			}
		}
		delete filteredgraph;
		delete graph;
	}
	delete hierarchy;
	delete filteredMeanGraph;

	return varDiff;
}

MyMatrixi GetInterLobeLinks(const MyGenericGraphf* graph, 
						   const MyGenericNode* hierarchy){
	int numNode = graph->GetNumNodes();
	int numLobe = hierarchy->GetNumChildren();
	MyArrayi node2cluster(numNode,-1);
	for(int i = 0;i<numLobe;i++){
		MyArrayStr nodeIdxStrs;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeIdxStrs,hierarchy->GetChild(i));
		for(int inode = 0;inode<nodeIdxStrs.size();inode++){
			int nodeIdx = nodeIdxStrs[inode].ToInt();
			node2cluster[nodeIdx] = i;
		}
	}
	for (int i = 0; i < node2cluster.size(); i++){
		assert(node2cluster[i] >= 0);
	}

	MyMatrixi rst (numLobe,numLobe, 0);
	for(int iedge = 0;iedge <graph->GetNumEdges();iedge++){
		MyVec2i edge = graph->GetEdge(iedge);
		MyVec2i edgeCluster(node2cluster[edge[0]],node2cluster[edge[1]]);
		rst.At(edgeCluster[0],edgeCluster[1]) ++;
		rst.At(edgeCluster[1],edgeCluster[0]) ++;
	}

	return rst;
}

MyArrayi GetInterLobeLinkChangeDist(const MyArrayStr& matrixFiles, 
								  const MyString& meanMatricFile,
								  const MyString& hierarchyFile){
	MyMatrixf* meanMat = MyDataLoader::MakeMatrixFromFile(meanMatricFile,NUMNODE, NUMNODE);
	MyDenseGraphf* meanGraph = new MyDenseGraphf;
	meanGraph->SetMatrix(meanMat);
	MyGraphFilter filter;
	filter.SetInputGraph(meanGraph);
	MyGenericGraphf* filteredMeanGraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	delete meanGraph;
	MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFile);
	MyMatrixi meanInterLobeLinks = GetInterLobeLinks(filteredMeanGraph,hierarchy);

	MyArrayi linkChanges;
	for(int i = 0;i<matrixFiles.size();i++){
		MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE, NUMNODE);
		MyDenseGraphf* graph = new MyDenseGraphf;
		graph->SetMatrix(mat);
		filter.SetInputGraph(graph);
		MyGenericGraphf* filteredgraph
			= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
		MyMatrixi interLinks = GetInterLobeLinks(filteredgraph, hierarchy);
		
		MyMatrixi interLobeLinkChange = meanInterLobeLinks - interLinks;
		for(int ir = 0;ir<meanInterLobeLinks.GetNumRows();ir++){
			for(int ic = ir+1;ic<meanInterLobeLinks.GetNumCols();ic++){
				linkChanges << interLobeLinkChange.At(ir,ic);
			}
		}
		delete filteredgraph;
		delete graph;
	}

	delete hierarchy;
	delete filteredMeanGraph;

	return linkChanges;
}

MyArrayi GetLinkNumForLobeDist(const MyArrayStr& matrixFiles, 
							   const MyString& hierarchyFile){
	MyArrayi rst;
	MyGraphFilter filter;
	MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFile);
	for(int i = 0;i<matrixFiles.size();i++){
		MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE, NUMNODE);
		MyDenseGraphf* graph = new MyDenseGraphf;
		graph->SetMatrix(mat);
		filter.SetInputGraph(graph);
		MyGenericGraphf* filteredgraph
			= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
		MyArrayi lnkForLobe = GetNumberLinkForLobe(filteredgraph, hierarchy);
		rst.PushBack(&lnkForLobe);
		delete filteredgraph;
		delete graph;
	}
	delete hierarchy;
	return rst;
}

void WriteArrayi(const MyArrayi& arr, const MyString& filename){
	std::ofstream outfile;
	outfile.open(filename);
	for(int i = 0;i<arr.size();i++){
		outfile << arr[i] << std::endl;
	}
	outfile.close();
}

void WriteArrayf(const MyArrayf& arr, const MyString& filename){
	std::ofstream outfile;
	outfile.open(filename);
	for(int i = 0;i<arr.size();i++){
		outfile << arr[i] << std::endl;
	}
	outfile.close();
}

void WriteVariationDifference(const MyString& filename){
	std::ifstream infile;
	infile.open(filename);
	MyArrayi variations;
	while (!infile.eof()){
		int t;
		infile >>t;
		variations << t;
	}

	MyArrayi variationDiffs;
	for(int ib = 0;ib<variations.size()/8;ib++){
		for(int ia0 = 0;ia0<8;ia0++){
			for(int ia1 = ia0+1;ia1<8;ia1++){
				int change = variations[ib*8+ia0]-variations[ib*8+ia1];
				variationDiffs << change;
			}
		}
	}

	WriteArrayi(variationDiffs,"varDiff.txt");
}

void WritePartialMatrix(const MyString& matfile, const MyString& outfile,
						const MyArrayi& set){
	MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matfile,NUMNODE, NUMNODE);
	MyMatrixf colMat = mat->GetCols(set);
	MyMatrixf rst = colMat.GetRows(set);
	delete mat;
	std::ofstream ofs;
	ofs.open(outfile);
	for(int i = 0;i<rst.GetNumRows();i++){
		for(int j = 0;j<rst.GetNumCols();j++){
			ofs << rst.At(i,j) << '\t';
		}
		ofs << std::endl;
	}
	ofs.close();
}

MyArrayi GetDegrees(const MyString& matfile){
	MyGraphFilter filter;
	MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matfile,NUMNODE, NUMNODE);
	MyDenseGraphf* graph = new MyDenseGraphf;
	graph->SetMatrix(mat);
	filter.SetInputGraph(graph);
	MyGenericGraphf* filteredgraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	MyArrayi deg;
	for(int i = 0;i<filteredgraph->GetNumNodes();i++){
		deg << filteredgraph->GetNeighbors(i).size();
	}
	delete graph;
	delete filteredgraph;
	return deg;
}

MyArrayi GetDegreeDiff(const MyString& meanMatFile, const MyString& matFile){
	MyArrayi meanDeg = GetDegrees(meanMatFile);
	MyArrayi deg = GetDegrees(matFile);
	MyArrayi diff;
	for(int i = 0;i<deg.size();i++){
		diff << meanDeg[i] - deg[i];
	}
	return diff;
}

MyArrayf GetBetweennessDist(const MyString& matFile){
	MyGraphFilter filter;
	MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matFile,NUMNODE, NUMNODE);
	MyDenseGraphf* graph = new MyDenseGraphf;
	graph->SetMatrix(mat);
	filter.SetInputGraph(graph);
	MyGenericGraphf* filteredgraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	MyGraphAlgorithm algor;
	algor.SetGraph(filteredgraph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf rst = algor.GetBetweennessCentrality();
	delete filteredgraph;
	delete graph;
	return rst;
}

int main(int argc, char* argv[]){

	MyString dropBoxFolder = "data\\";
	MyString matrixFolder = dropBoxFolder + "NBS1.2\\SchizophreniaExample\\matrices\\";
	MyString meanMatFile = matrixFolder+"MeanSubject.txt";
	MyString hierarchyFile = dropBoxFolder + "NBS1.2\\SchizophreniaExample\\nodeHierarchy.txt";
	MyArrayStr matrixFiles;
	for(int i = 1;i<=NUMGRAPH;i++){
		MyString filename = matrixFolder;
		filename += "subject";
		if(i<10){
			filename += "0";
		}
		filename += MyString(i);
		filename += ".txt";
		matrixFiles << filename;
	}
	
	MyArrayf density;
	density << 0.05 << 0.10;
	MyArrayStr nameAffix;
	nameAffix << "74005.txt" << "74010.txt";
	for(int di = 0;di<density.size();di++){
		LINKDENSITY = density[di];
		MyString affix = nameAffix[di];

		// get degree distribution
		MyArrayi degDist;
		for(int i = 0;i<NUMGRAPH;i++){
			MyArrayi deg = GetDegrees(matrixFiles[i]);
			degDist.PushBack(&deg);
		}
		WriteArrayi(degDist, "DegDist"+affix);

		// get degree difference distribution
		MyArrayi degDiffDist;
		for(int i = 0;i<NUMGRAPH;i++){
			MyArrayi deg = GetDegreeDiff(meanMatFile, matrixFiles[i]);
			degDiffDist.PushBack(&deg);
		}
		WriteArrayi(degDiffDist, "DegDiffDist"+affix);

		// get variation difference distribution
		MyArrayi varDiffDist;
		varDiffDist = GetVariationDifferenceDistribution(matrixFiles, meanMatFile, hierarchyFile);
		WriteArrayi(varDiffDist, "varDiffDist"+affix);

		// get betweenness distribution
		MyArrayf betnDist = GetBetweennessDist(meanMatFile);
		WriteArrayf(betnDist, "betnDist"+affix);

		// get inter-lobe link change difference distribution
		MyArrayi intLobLnkNumChgDiffDist = GetInterLobeLinkChangeDist(matrixFiles, meanMatFile, hierarchyFile);
		WriteArrayi(intLobLnkNumChgDiffDist, "intLobLnkNumChgDiffDist"+affix);
	}


	//WriteVariationDifference("var_dist.txt");
	//return 1;
	/*
	MyString dropBoxFolder = "data\\Connection_data_100\\";
	MyString matrixFolder = dropBoxFolder + "contest\\";
	MyString meanMatFile = matrixFolder+"MeanSubject.txt";
	MyString hierarchyFile = dropBoxFolder + "roi_hierarchy.txt";
	MyArrayStr matrixFiles;
	std::ifstream namefile;
	namefile.open(dropBoxFolder + "matrices_contest_18.txt");
	for (int i = 0; i<NUMGRAPH; i++){
		MyString filename;
		namefile >> filename;
		matrixFiles << matrixFolder + filename;
	}
	namefile.close();
	*/
	//MyArrayi varDist = GetVariationDistribution(matrixFiles,meanMatFile,hierarchyFile);
	//WriteVariationDifference("var_dist2_74.txt");
	
	/*
	MyString dropBoxFolder2 = "data2\\Connection_data\\";
	MyArrayi lefts = MyArrayi::GetSequence(0,147);
	namefile.open(dropBoxFolder + "matrices_contest_18.txt");
	for (int i = 0; i<NUMGRAPH; i++){
		MyString filename;
		namefile >> filename;
		WritePartialMatrix(matrixFiles[i], dropBoxFolder2 + "contest\\"+filename, lefts);
	}
	WritePartialMatrix(meanMatFile, dropBoxFolder2 + "MeanSubject.txt", lefts);
	namefile.close();
	*/

	//MyArrayi interLobeLinkChangDist = GetInterLobeLinkChangeDist(matrixFiles,meanMatFile,hierarchyFile);
	//WriteArrayi(interLobeLinkChangDist,"interLobeLinkChangeDist167.txt");
	//WriteArrayi(varDist, "var_dist2_74.txt");

	//MyArrayi lobeLinks = GetLinkNumForLobeDist(matrixFiles, hierarchyFile);
	//WriteArrayi(lobeLinks, "lobe_link_dist100.txt");



	// get variation distribution
	/*
	// get degree centrality distribution for mean graph
	// or get betweenness centrality for mean graph
	MyMatrixf meanMat(NUMNODE,NUMNODE, 0);
	for(int i = 0;i<NUMGRAPH;i++){
		MyMatrixf* thisMat = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE,NUMNODE);
		meanMat = meanMat+(*thisMat);
	}

	MyDenseGraphf meanGraph;
	meanGraph.SetMatrix(new MyMatrixf(meanMat));
	MyGraphFilter filter;
	filter.SetInputGraph(&meanGraph);
	MyGenericGraphf* filteredMeanGraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	MyGraphAlgorithm algor;
	algor.SetGraph(filteredMeanGraph);
	
	MyArrayf dc = algor.GetDegreeCentrality();
	//algor.FloydWarshallWithPathReconstruction();
	//MyArrayf dc = algor.GetBetweennessCentrality();

	std::ofstream dcfile;
	dcfile.open("dx.txt");
	for(int i = 0;i<dc.size();i++){
		dcfile << dc[i] << std:: endl;
	}
	dcfile.close();
	delete filteredMeanGraph;
	*/
	/*
	// get node difference
	MyMatrixf* mats[NUMGRAPH];
	MyMatrixf meanMat(NUMNODE,NUMNODE, 0);
	for(int i = 0;i<NUMGRAPH;i++){
		mats[i] = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE,NUMNODE);
		meanMat = meanMat+(*mats[i]);
	}
	MyDenseGraphf meanGraph;
	meanGraph.SetMatrix(new MyMatrixf(meanMat));
	MyGraphFilter filter;
	filter.SetInputGraph(&meanGraph);
	MyGenericGraphf* filteredMeanGraph = filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	MyGraphAlgorithm algor;
	algor.SetGraph(filteredMeanGraph);
	MyArrayf dc_mean = algor.GetDegreeCentrality();
	
	std::ofstream dcfile;
	dcfile.open("dx.txt");
	for(int i = 0;i<NUMGRAPH;i++){
		MyDenseGraphf* thisGraph = 
			MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matrixFiles[i],NUMNODE,NUMNODE,LINKDENSITY);
		algor.SetGraph(thisGraph);
		MyArrayf dc = algor.GetDegreeCentrality();
		for(int ip = 0;ip<NUMNODE; ip++){
			dcfile << dc[ip]-dc_mean[ip] << std::endl;
		}
	}
	dcfile.close();
	delete filteredMeanGraph;
	*/
	/*
	MyMatrixf* mats[NUMGRAPH];
	MyMatrixf meanMat(NUMNODE,NUMNODE, 0);
	for(int i = 0;i<NUMGRAPH;i++){
		mats[i] = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE,NUMNODE);
		meanMat = meanMat+(*mats[i]);
	}
	MyDenseGraphf meanGraph;
	meanGraph.SetMatrix(new MyMatrixf(meanMat));
	MyGraphFilter filter;
	filter.SetInputGraph(&meanGraph);
	MyGenericGraphf* filteredMeanGraph = filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	MyGraphAlgorithm algor;
	algor.SetGraph(filteredMeanGraph);

	MyArrayi numCns;
	for(int i = 0;i<NUMNODE;i++){
		MyArrayi neighbor0 = filteredMeanGraph->GetNeighbors(i);
		for(int j = i+1;j<NUMNODE;j++){
			MyArrayi neighbor1 = filteredMeanGraph->GetNeighbors(j);
			MyArrayi* cns = MyArrayi::MakeCommonElementArray(neighbor0, neighbor1);
			numCns << cns->size();
			delete cns;
		}
	}
	
	std::ofstream dcfile;
	dcfile.open("dx.txt");
	for(int i = 0;i<numCns.size();i++){
		if(numCns[i]<=0) continue;
		dcfile << numCns[i] << std::endl;
	}
	dcfile.close();
	delete filteredMeanGraph;
	*/
	
	/*
	MyMatrixf* mats[NUMGRAPH];
	MyMatrixf meanMat(NUMNODE,NUMNODE, 0);
	for(int i = 0;i<NUMGRAPH;i++){
		mats[i] = MyDataLoader::MakeMatrixFromFile(matrixFiles[i],NUMNODE,NUMNODE);
		meanMat = meanMat+(*mats[i]);
	}
	meanMat = meanMat*(1.f/NUMGRAPH);
	
	std::ofstream dcfile;
	dcfile.open("MeanSubject.txt");
	for(int i = 0;i<meanMat.GetNumRows();i++){
		for(int j = 0;j<meanMat.GetNumCols();j++){
			dcfile << meanMat.At(i,j) << "\t";
		}
		dcfile << std::endl;
	}
	dcfile.close();
	*/
}