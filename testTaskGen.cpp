#include <fstream>
#include "MyString.h"
#include "MyArray.h"
#include "MyGraphTaskGenerator.h"
using namespace std;

#define NUMNODE 12
int main(int argc, char* argv[]){

	MyString dropBoxFolder = "C:\\Users\\GuohaoZhang\\Dropbox\\Connection_data\\fake\\";
	MyString meanMatFile = dropBoxFolder + "mat88.txt";
	MyString hierarchyFile = dropBoxFolder+"nodeHierarchy.txt";
	MyString positionFile = dropBoxFolder+"COG.txt";
	MyString labelFile = dropBoxFolder + "nodeLabels.txt";

	float density = 0.5f;
	MyArrayStr geoValues;
	geoValues << "layout_default" << "layout_treering" << "layout_matrix";
	MyArrayStr topoValues;
	topoValues << "bundler_GEB" << "bundler_HEB" << "bundler_none";
	MyArrayStr repValues;
	repValues << "representation_default" << "representation_default" << "representation_matrix";
	MyArrayi encValues = MyArrayi::GetSequence(1,9);
	MyString task = "task_hub";
	MyString mDecimer = "\t";
	MyArrayi indices(1,0);

	for(int i = 0;i<geoValues.size();i++){
		MyString geo = geoValues[i];
		MyString topo = topoValues[i];
		MyString rep = repValues[i];
		for(int j = 0;j<encValues.size();j++){
			MyString encoding = encValues[j];
			ofstream out;
			out.open("test3\\"+MyString(i*(int)encValues.size()+j)+".txt");

			
			out << "[task_name]" << mDecimer << task << endl;
			out << "[extr_info]";
			if(indices.size() > 1){
				out << mDecimer << indices.size();
			}
			for(int i = 0;i<indices.size();i++){
				out << mDecimer << indices[i];
			}
			out << endl;
			out << "[num_nodes]" << mDecimer << NUMNODE << endl;
			out << "[hier_file]" << mDecimer << hierarchyFile << endl
				<< "[pos_file]" << mDecimer << positionFile << endl
				<< "[label_file]" << mDecimer << labelFile << endl;
			int numGraph = 1;
			out << "[num_graph]" << mDecimer << numGraph << endl;
			for(int i = 0;i<numGraph;i++){
				out << "[matrix_file]" << mDecimer << meanMatFile << endl
					<< "[density]" << mDecimer << density << endl
					<< "[geometry]" << mDecimer << geo << endl
					<< "[topology]" << mDecimer << topo << endl
					<< "[encoding]" << mDecimer << encoding << endl
					<< "[represent]" << mDecimer << rep << endl;
			}
			out.close();
		}
	}
}