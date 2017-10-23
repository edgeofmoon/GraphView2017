#include "MyGraphTaskGenerator.h"
#include "VisConfig.h"
#include <fstream>

int main(int argc, char* argv[]){
	MyGraphTaskGenerator generator;
	if(DATASET == 0){
		MyString dropBoxFolder = "data\\";
		MyString matrixFolder = dropBoxFolder + "NBS1.2\\SchizophreniaExample\\matrices\\";
		MyArrayStr matrixFiles;
		for(int i = 1;i<=27;i++){
			MyString filename = matrixFolder;
			filename += "subject";
			if(i<10){
				filename += "0";
			}
			filename += MyString(i);
			filename += ".txt";
			matrixFiles << filename;
		}
		generator.SetMeanMatrixFile(dropBoxFolder + "NBS1.2\\SchizophreniaExample\\matrices\\MeanSubject.txt");
		generator.SetMatrixFiles(matrixFiles);
		generator.SetHierarchyFile(dropBoxFolder+"NBS1.2\\SchizophreniaExample\\nodeHierarchy.txt");
		generator.SetPositionFile(dropBoxFolder+"NBS1.2\\SchizophreniaExample\\COG2.txt");
		generator.SetLabelFile(dropBoxFolder + "NBS1.2\\SchizophreniaExample\\nodeLabels.txt");
		generator.ReadSegmentationFile(dropBoxFolder + "NBS1.2\\SchizophreniaExample\\nodeLobe.txt");
	}
	else if (DATASET == 1){
		MyString dropBoxFolder = "data\\Connection_data_100\\";
		MyString matrixFolder = dropBoxFolder + "contest\\";
		MyArrayStr matrixFiles;
		std::ifstream namefile;
		namefile.open(dropBoxFolder+"matrices_contest_18.txt");
		for(int i = 0;i<18;i++){
			MyString filename;
			namefile >> filename;
			matrixFiles << matrixFolder+filename;
		}
		generator.SetMeanMatrixFile(matrixFolder + "MeanSubject.txt");
		generator.SetMatrixFiles(matrixFiles);
		generator.SetHierarchyFile(dropBoxFolder+"roi_hierarchy.txt");
		generator.SetPositionFile(dropBoxFolder+"fsaverage_ROI_mass_center.txt");
		generator.SetLabelFile(dropBoxFolder + "roi_legend.txt");
		//generator.ReadSegmentationFile(dropBoxFolder + "roi_lobe.txt");
	}
	
	generator.GenerateConfigureFiles();
}