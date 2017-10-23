
#include "MyString.h"

#include "MyArray.h"
#include "MyKnotLayoutManager.h"
#include "MyDataLoader.h"
#include "MyGraphicsTool.h"
#include "MyGraphKnot.h"
#include "MyView.h"
#include "MyRenderScene.h"
#include "MyGenericEvent.h"
#include "MyGraphFilter.h"
#include "MyDenseGraph.h"
#include "MyUtility.h"

#include "MyGraphLayout.h"
#include "MyGraphMatrixLayout.h"
#include "MyGraphIterativeLayout.h"
#include "MyGraphFruchtermanReingoldLayout.h"
#include "MyGraphLocalOptimizationLayout.h"
#include "MyGraphHierarchicalLayout.h"
#include "MyGraphLodLayout.h"
#include "MyGraphRadialLayout.h"
#include "MyGenericLayoutSyncer.h"

#include "MyGraphEncodingRepresentation.h"
#include "MyGraphAlgorithm.h"
#include "MyGraphMatrixEncodingRepresentation.h"

#include "MyGraphEdgeBundler.h"
#include "MyGraphClusterEdgeBundler.h"
#include "MyGraphGeometryEdgeBundler.h"

MyGraphicsTool helper;
MyView* view;
MyScene* scene;

// suppose only one keyboard or mouse
MyGenericEvent::MyMouseKey lastEventMouseKey = MyGenericEvent::MOUSE_KEY_LEFT;

//#include "gl/glut.h"

MyGenericEvent::MyMouseKey toKey(int k){
	return (k == 0 ?
		MyGenericEvent::MOUSE_KEY_LEFT : (k == 1 ?
		MyGenericEvent::MOUSE_KEY_MIDDLE : MyGenericEvent::MOUSE_KEY_RIGHT));
}

MyGenericEvent::MyKeyState toState(int s){
	return (s == 0 ? MyGenericEvent::KEY_DOWN : MyGenericEvent::KEY_UP);
}

void DisplayFunc(void){
	helper.ClearFrameBuffer();
	view->Show();
	helper.FreshScreen();
}
void ReshapeFunc(int w, int h){
	MyGenericEvent eve = MyGenericEvent::GenerateWindowResizeEvent(w, h);
	view->EventHandler(eve);
}
void KeyReleaseFunc(unsigned char c, int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateKeyboardKeyEvent(c, MyGenericEvent::KEY_UP, x, y);
	helper.Update();
}
void KeyPressFunc(unsigned char c, int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateKeyboardKeyEvent(c, MyGenericEvent::KEY_DOWN, x, y);
	view->EventHandler(eve);
	helper.Update();
}
void MouseKeyFunc(int button, int state, int x, int y){
	lastEventMouseKey = toKey(button);
	MyGenericEvent::MyKeyState keyState = toState(state);
	MyGenericEvent eve = MyGenericEvent::GenerateMouseKeyEvent(lastEventMouseKey, keyState, x, y);
	view->EventHandler(eve);
	helper.Update();
}
void MouseMoveFunc(int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateMouseMoveEvent(
		lastEventMouseKey, MyGenericEvent::KEY_DOWN, x, y);
	view->EventHandler(eve);
	if (eve.DoNeedRedraw()){
		helper.Update();
	}
}

void IdleFunc(){
	MyGenericEvent eve = MyGenericEvent::GenerateTimerEvent();
	//view->EventHandler(eve);
	if (eve.DoNeedRedraw()){
		helper.Update();
	}
}

int main(int argc, char* argv[]){
	
	helper.Init(&argc,argv);
	scene = new MyRenderScene;
	view = new MyView;

	MyString dropBoxFolder = "C:\\Users\\GuohaoZhang\\Dropbox";
	//string dropBoxFolder = "C:\\Users\\Guohao Zhang\\Desktop\\dropbox";
	
	MyGenericNode* hierarchy
		= MyDataLoader::LoadHierarchyOverlay(dropBoxFolder + "\\Connection_data\\fake\\nodeHierarchy.txt");
	MyArray3f* pos
		= MyDataLoader::MakeArrayVecFromFile(dropBoxFolder + "\\Connection_data\\fake\\COG.txt", 12);
	MyArrayStr* labels
		= MyDataLoader::MakeArrayStrFromFile(dropBoxFolder + "\\Connection_data\\fake\\nodeLabels.txt");
	MyGraphKnot* tmp = 0;

	//MyArray3f* pos;
	//MyArrayStr* labels;
	//MyMatrixf* mat;
	//MyDataLoader::MakeGraphFromXmlFile(dropBoxFolder+"\\data\\\connectome\\network_res83.graphml",
	//	mat, pos, labels);
	//MyGenericNode* hierarchy
	//	= MyDataLoader::LoadHierarchyOverlay(dropBoxFolder + "\\data\\\connectome\\nodeHierarchy.txt");

	for(int i = 1;i<=1;i++){
		MyDenseGraphf* graph = new MyDenseGraphf;
		MyString idxStr(1);
		MyString subjectName;
		if(i<10){
			subjectName = MyString("subject0")+idxStr+".txt";
		}
		else{
			subjectName = MyString("subject")+idxStr+".txt";
		}
		MyMatrixf* mat
			= MyDataLoader::MakeMatrixFromFile(dropBoxFolder+"\\Connection_data\\fake\\mat88.txt", 12, 12);
		graph->SetMatrix(mat);
		MyGraphFilter* graphFilter = new MyGraphFilter;
		graphFilter->SetInputGraph(graph);
		MyDenseGraphf* newGraph = dynamic_cast<MyDenseGraphf*>(graphFilter->MakeFilteringByEdgeWeightRatio(.5));
		//SafeFreeObject(graph);

		// node link diagram
		MyGraphKnot* graphKnot = new MyGraphKnot;

		if(i == 1){
			tmp = graphKnot;
		}
		else{
			MyGraphKnot::Connect(tmp, graphKnot);
			tmp = graphKnot;
		}
		// load data level info
		graphKnot->SetGraph(newGraph);
		graphKnot->SetGraphNodePos(pos);
		graphKnot->SetGraphNodeLabel(labels);
		graphKnot->SetHierarchy(hierarchy);
		MyGraphLayout* layout = new MyGraphLayout;
		layout->SetEdgeBundler(new MyGraphGeometryEdgeBundler(4,4));
		layout->SetEdgeBundle();
		MyGraphEncodingRepresentation* representation = new MyGraphEncodingRepresentation;
		//MyGraphMatrixLayout* layout = new MyGraphMatrixLayout;
		//layout->OrderByHierarchy(hierarchy);
		//MyGraphEncodingRepresentation* representation = new MyGraphMatrixEncodingRepresentation;
		//MyGraphLayout* layout = new MyGraphRadialLayout;
		//MyGraphClusterEdgeBundler * bundler = new MyGraphClusterEdgeBundler;
		//bundler->SetHierarchy(hierarchy);
		//layout->SetEdgeBundler(bundler);
		//layout->SetEdgeBundle();
		//MyGraphEncodingRepresentation* representation = new MyGraphEncodingRepresentation;
		MyGraphAlgorithm algor;
		algor.SetGraph(newGraph);
		algor.FloydWarshallWithPathReconstruction();
		MyArrayf values = algor.GetBetweennessCentrality();
		if (representation){
			representation->SetNodeValuef(values);
			representation->SetNodeEncodingMethods((MyVis::MyVisualElement)1);
		}
		
		//MyGraphSelectionMask* mask = new MyGraphSelectionMask;
		//mask->SetGraphRepresentation(representation);
		//mask->SetHierarchy(hierarchy);

		//graphKnot->SetSelectionMask(mask);
		graphKnot->SetLayout(layout);
		graphKnot->SetRepresentation(representation);

		// systhesis info
		graphKnot->Build();
		scene->AddKnot(graphKnot);
		/*
		// matrix 
		MyGraphKnot* graphKnot2 = new MyGraphKnot;
		MyGraphKnot::Connect(tmp, graphKnot2);
		tmp = graphKnot2;

		// load data level info
		graphKnot2->SetGraph(newGraph);
		graphKnot2->SetGraphNodePos(pos);
		graphKnot2->SetGraphNodeLabel(labels);
		graphKnot2->SetHierarchy(hierarchy);

		MyGraphLayout* layout2
			= new MyGraphMatrixLayout;
		MyGenericLayoutSyncer::Connect(layout2,layoutSyncer2);
		MyGraphRepresentation* representation2
			= new MyGraphMatrixRepresentation;
		graphKnot2->SetLayout(layout2);
		graphKnot2->SetRepresentation(representation2);

		// systhesis info
		graphKnot2->Build();
		scene->AddKnot(graphKnot2);
		*/
	}
	view->SetScene(scene);
	scene->SetView(view);
	scene->Build();
	view->Build();
	helper.RegisterDisplayFunction(DisplayFunc);
	helper.RegisterReshapeFunction(ReshapeFunc);
	helper.RegisterKeyReleaseFunction(KeyReleaseFunc);
	helper.RegisterKeyPressFunction(KeyPressFunc);
	helper.RegisterMouseKeyFunction(MouseKeyFunc);
	helper.RegisterMouseMoveFunction(MouseMoveFunc);
	helper.RegisterIdleFunction(IdleFunc);
	helper.Start();

	//SafeFreeObject(hierarchy);
	SafeFreeObject(pos);
	SafeFreeObject(labels);

	return 1;
}