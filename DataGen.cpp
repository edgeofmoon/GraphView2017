#include "MyDenseGraph.h"
#include "MyGraphFilter.h"
#include "MyDataLoader.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyMatrix.h"
#include "VisConfig.h"
#include "MyGraphTaskGenerator.h"

#include <algorithm>
#include <fstream>

#define NODELEFT 100

bool buildTask = false;

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

void WriteParialLineFile(const MyString& infile, const MyString& outfile,
						const MyArrayi& set){
	std::ifstream ifs;
	ifs.open(infile);
	std::ofstream ofs;
	ofs.open(outfile);
	int i = 0;
	while(!ifs.eof()){
		MyString line;
		std::getline(ifs, line, '\n');
		if(set.HasOne(i)){
			ofs << line << '\n';
		}
		i++;
	}
	ifs.close();
	ofs.close();
}

void WriteParialHierarchyFile(const MyString& infile, const MyString& outfile,
						const MyArrayi& set){
	MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(infile);
	std::ofstream ofs;
	ofs.open(outfile);
	
	MyArrayi oldToNewIdx(NUMNODE, -1);
	for(int i = 0;i<set.size();i++){
		oldToNewIdx[set[i]] = i;
	}

	MyGenericHierarchyOverlayStr* hierarchyStr 
		= dynamic_cast<MyGenericHierarchyOverlayStr*>(hierarchy);
	ofs << hierarchyStr->GetObject() << std::endl;
	for(int i = 0;i<hierarchy->GetNumChildren();i++){
		MyGenericHierarchyOverlayStr* clusterStr 
			= dynamic_cast<MyGenericHierarchyOverlayStr*>(hierarchy->GetChild(i));
		ofs << clusterStr->GetObject() << std::endl;
		for(int j = 0;j<clusterStr->GetNumChildren();j++){
			MyGenericHierarchyOverlayStr* leaveStr 
				= dynamic_cast<MyGenericHierarchyOverlayStr*>(clusterStr->GetChild(j));
			int newNodeIdx = oldToNewIdx[leaveStr->GetObject().ToInt()];
			if(newNodeIdx >= 0){
				ofs << newNodeIdx << ' ';
			}
		}
		ofs << std::endl;
	}
	ofs.close();
	hierarchy->DestoryAll();
	delete hierarchy;
}

MyString GetNonSideName(const MyString& name){
	MyString rst = name;
	rst.erase(rst.end()-1);
	return rst;
}

MyArray2i GetNodePairs(MyString labelFile){
	MyArrayStr* label = MyDataLoader::MakeArrayStrFromFile(labelFile);
	MyArray2i rst;
	/*
	MyArrayb taken(label->size(), false);
	for(int i = 0;i<label->size();i++){
		if(taken[i]) continue;
		MyString oneLabel = GetNonSideName(label->at(i));
		for(int j = i+1;j<label->size();j++){
			if(taken[j]) continue;
			MyString deLabel = GetNonSideName(label->at(j));
			if(deLabel == oneLabel){
				rst << MyVec2i(i,j);
				taken[i] = true;
				taken[j] = true;
			}
		}
	}
	delete label;
	return rst;
	*/
	
	for(int i = 0;i<label->size()/2;i++){
		rst << MyVec2i(i, i+label->size()/2);
	}
	return rst;
}

MyVec3f transform(const MyVec3f& a){
	MyVec3f rst = a;
	rst[1] = a[2];
	rst[2] = a[1];
	// so called flat
	rst[2] = 0;
	return rst;
}

MyMatrixf GetPairWiseDistance(MyString coordfile){
	MyArray3f* coord = MyDataLoader::MakeArrayVecFromFile(coordfile);
	int n = coord->size();
	MyMatrixf dist(n, n);
	for(int i = 0;i<n;i++){
		MyVec3f a = transform(coord->at(i));
		for(int j = 0;j<n;j++){
			MyVec3f b = transform(coord->at(j));
			dist.At(i,j) = (a-b).norm();
		}
	}
	delete coord;
	return dist;
}

MyArrayi GetSparserNodes(const MyMatrixf& dist, const MyArray2i& pairs, int nodeLeft){
	MyMatrixf meanDist = dist.GetMeanOfCol();
	MyArrayf pairDist;
	for(int i = 0;i<pairs.size();i++){
		pairDist << meanDist.At(1, pairs[i][0])+meanDist.At(1, pairs[i][1]);
	}
	MyArrayi* sort = pairDist.MakeSortResultArray();
	MyArrayi rst;
	for(int i = 0;i<nodeLeft/2;i++){
		// the the ones with most distance
		int idx = sort->at(sort->size()-1-i);
		rst << pairs[idx][0] << pairs[idx][1];
	}
	std::sort(rst.begin(), rst.end());
	delete sort;
	return rst;
}

int main(int argc, char* argv[]){
	

	MyString dropBoxFolder = "data\\Connection_data_148\\";
	MyString matrixFolder = dropBoxFolder + "contest\\";
	MyString meanMatFile = matrixFolder+"MeanSubject.txt";
	MyString hierarchyFile = dropBoxFolder + "roi_hierarchy.txt";
	MyString labelFile = dropBoxFolder + "roi_legend.txt";
	MyString coordFile = dropBoxFolder + "fsaverage_ROI_mass_center.txt";
	MyArrayStr matrixFiles;
	std::ifstream namefile;
	namefile.open(dropBoxFolder + "matrices_contest_18.txt");
	for (int i = 0; i<NUMGRAPH; i++){
		MyString filename;
		namefile >> filename;
		matrixFiles << matrixFolder + filename;
	}
	namefile.close();
	

	MyMatrixf* meanMat = MyDataLoader::MakeMatrixFromFile(meanMatFile,NUMNODE, NUMNODE);
	MyDenseGraphf* meanGraph = new MyDenseGraphf;
	meanGraph->SetMatrix(meanMat);
	MyGraphFilter filter;
	filter.SetInputGraph(meanGraph);
	MyGenericGraphf* filteredMeanGraph
		= filter.MakeFilteringByEdgeWeightRatio(LINKDENSITY);
	delete meanGraph;

	//MyGraphAlgorithm algor;
	//algor.SetGraph(filteredMeanGraph);
	//MyArrayi nodes = algor.GetTopDegreeNodes(NODELEFT);
	//std::sort(nodes.begin(), nodes.end());
	
	MyArray2i pairs = GetNodePairs(labelFile);
	MyMatrixf dist = GetPairWiseDistance(coordFile);
	MyArrayi nodes = GetSparserNodes(dist, pairs, NODELEFT);

	// write the files
	// get new file names
	MyString newFolder = "data\\Connection_data_100\\";
	MyString newMatrixFolder = newFolder + "contest\\";
	MyString newMeanMatFile = newMatrixFolder+"MeanSubject.txt";
	MyString newHierarchyFile = newFolder + "roi_hierarchy.txt";
	MyString newLabelFile = newFolder + "roi_legend.txt";
	MyString newCoordFile = newFolder + "fsaverage_ROI_mass_center.txt";
	MyArrayStr newMatrixFiles;
	namefile.open(dropBoxFolder + "matrices_contest_18.txt");
	for (int i = 0; i<NUMGRAPH; i++){
		MyString filename;
		namefile >> filename;
		newMatrixFiles << newMatrixFolder + filename;
	}
	namefile.close();

	// write new matrices
	for(int i = 0;i<newMatrixFiles.size();i++){
		WritePartialMatrix(matrixFiles[i], newMatrixFiles[i], nodes);
	}

	// write mean matrix file
	WritePartialMatrix(meanMatFile, newMeanMatFile, nodes);

	// write label file
	WriteParialLineFile(labelFile, newLabelFile, nodes);

	// write coordinate file
	WriteParialLineFile(coordFile, newCoordFile, nodes);

	// write hierarchy file
	WriteParialHierarchyFile(hierarchyFile, newHierarchyFile, nodes);

	if(buildTask){
		MyGraphTaskGenerator generator;
		generator.SetMeanMatrixFile(newMeanMatFile);
		generator.SetMatrixFiles(newMatrixFiles);
		generator.SetHierarchyFile(newHierarchyFile);
		generator.SetPositionFile(newCoordFile);
		generator.SetLabelFile(newLabelFile);
		//generator.ReadSegmentationFile(dropBoxFolder + "roi_lobe.txt");
		generator.GenerateConfigureFiles();
	}
}