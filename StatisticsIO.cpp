#include <fstream>
#include "MyArray.h"

class Row{
public:
	MyString pIdx;
	MyString trialIdx;
	MyString task;
	MyString layout;
	MyString encoding;
	MyString density;
	MyString UserAns;
	MyString CorrectAns;
	MyString TimeUsed;
	MyString logTime;
	MyString IsCorrect;

	friend std::istream& operator>>(std::istream& in, Row& row){
		return in >> row.pIdx >> row.trialIdx >> row.task
			>> row.layout >> row.encoding >> row.density
			>> row.UserAns >> row.CorrectAns >> row.TimeUsed
			>> row.logTime >> row.IsCorrect;
	}

	friend std::ostream& operator<<(std::ostream& in, Row& row){
		return out << row.pIdx << row.trialIdx << row.task
			<< row.layout << row.encoding << row.density
			<< row.UserAns << row.CorrectAns << row.TimeUsed
			<< row.logTime << row.IsCorrect;
	}

	MyString& operator[](int i){
		return *((&pIdx) +i);
	}
};

bool IsRowFirst(MyArray<Row>& table, int rowIdx, int colIdx){
	if(rowIdx==0) return true;
	return (table[rowIdx][colIdx] != table[rowIdx][colIdx-1]);
}

bool IsRowLast(MyArray<Row>& table, int rowIdx, int colIdx){
	if(rowIdx==table.size()-1) return true;
	return (table[rowIdx][colIdx] != table[rowIdx][colIdx+1]);
}

void RemoveCorrectnessOutlier(MyArray<Row>& table){
	int correctColIdx = 10;
	for(int r = 0;r<table.size();r++){
		if(IsRowFirst(table,r,correctColIdx)){

		}
	}
}

int main(int argc, char* argv[]){
	std::ifstream rstFile;
	rstFile.open("C:\\Users\\GuohaoZhang\\Dropbox\\SAS\\connection\\results_itr5.txt");
	char tmp[1000];
	//get header
	rstFile.getline(tmp,1000,'\n');
	MyArray<Row> table;
	while (!rstFile.eof()){
		Row row;
		rstFile >> row;
		table << row;
	}
}