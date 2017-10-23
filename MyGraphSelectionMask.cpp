#include "MyGraphSelectionMask.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphDrawer.h"
#include "MyUtility.h"
#include "MyGraphRadialLayout.h"
#include "MyGraphMatrixLayout.h"
#include "MyGraphMatrixEncodingRepresentation.h"

int MyGraphSelectionMask::mBackground = 0;

MyGraphSelectionMask::MyGraphSelectionMask(void)
{
	 mRepresentation = 0;
	 mHierarchy = 0;
}


MyGraphSelectionMask::~MyGraphSelectionMask(void)
{
}

void MyGraphSelectionMask::SetGraphRepresentation(const MyGraphRepresentation* rep){
	mRepresentation = rep;
}

void MyGraphSelectionMask::SetHierarchy(const MyGenericNode* hierarchy){
	mHierarchy = hierarchy;
}

void MyGraphSelectionMask::Update(){
	mClusterBox.clear();
	for(int iCluster = 0;iCluster<mHierarchy->GetNumChildren();iCluster++){
		MyArrayStr children;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&children,mHierarchy->GetChild(iCluster));
		MyArray<MyBoundingBox> markerBoxes;
		// every node must have the same number of markers
		for(int iMarker = 0;iMarker < mRepresentation->GetNumOfTangibleMarker(0);iMarker ++){
			mRepresentation->GetNodeTangiblePos(children[0].ToInt(), iMarker);
			MyBoundingBox markerBox = mRepresentation->GetNodeTangibleBox(children[0].ToInt(), iMarker);
			for(int iChild = 0;iChild<children.size();iChild++){
				int nodeIdx = children[iChild].ToInt();
				MyBoundingBox box_t = mRepresentation->GetNodeTangibleBox(nodeIdx, iMarker);
				markerBox.Engulf(box_t);
			}
			markerBoxes << markerBox;
		}
		mClusterBox << markerBoxes;
	}

	mMaskerLabelAlignment.clear();
	mMaskerLabelPos.clear();
	const MyGraphLayout* layout = mRepresentation->GetGraphLayout();
	int layoutIdx = 0;
	if (dynamic_cast<const MyGraphRadialLayout*>(layout) != 0){
		layoutIdx = 1;
	}
	else if (dynamic_cast<const MyGraphMatrixLayout*>(layout) != 0){
		layoutIdx = 2;
	}
	/*
	MyVec3f labelPos[3][8] = {
		{ MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f(),
		MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f() },
		{ MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f(),
		MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f() },
		{ MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f(),
		MyVec3f(), MyVec3f(), MyVec3f(), MyVec3f() }
	};

	mMaskerLabelPos.clear();
	for (int i = 0; i < 8; i++){
		mMaskerLabelPos << labelPos[layoutIdx][i];
	}
	*/
	if (layoutIdx == 0){
		for (int i = 0; i < mClusterBox.size(); i++){
			MyBoundingBox box = mClusterBox[i][0];
			int farthestPointIdx = 0;
			float fathestPointDist = 0;
			for (int ip = 0; ip < 4; ip++){
				float dist = box.GetCornerPos(ip).norm();
				if (dist> fathestPointDist){
					fathestPointDist = dist;
					farthestPointIdx = ip;
				}
			}
			mMaskerLabelPos << box.GetCornerPos(farthestPointIdx)+MyVec3f(0,-3,0);
			switch (farthestPointIdx){
				case 0:
					mMaskerLabelAlignment << 2;
					break;
				case 1:
					mMaskerLabelAlignment << 0;
					break;
				case 2:
					mMaskerLabelAlignment << 2;
					break;
				case 3:
				default:
					mMaskerLabelAlignment << 0;
					break;
			}
		}
	}
	else if (layoutIdx == 1){
		// box index is different
		int idx[] = { 0, 1, 3, 2 };
		for (int i = 0; i < mClusterBox.size(); i++){
			MyBoundingBox box = mClusterBox[i][0];
			MyVec2f intersection = box.GetCornerPos(0).toDim<2>();
			MyVec3f center = box.GetCenter();
			MyVec2f center2(center[0], center[1]);
			MyLine2f ray(MyVec2f(0, 0), center2);
			int edgeIdx = 0;
			for (int ie = 0; ie < 4; ie++){
				MyLine2f edge(box.GetCornerPos(idx[ie]).toDim<2>(), box.GetCornerPos((idx[(ie + 1) % 4])).toDim<2>());
				if (MyLine2f::Intersected(ray, edge)){
					intersection = MyLine2f::GetIntersection(ray, edge);
					edgeIdx = ie;
					break;
				}
			}
			mMaskerLabelPos << intersection.toDim<3>(0);
			if (edgeIdx == 3){
				mMaskerLabelAlignment << 2;
			}
			else if (edgeIdx == 1){
				mMaskerLabelAlignment << 0;
			}
			else{
				mMaskerLabelAlignment << 1;
			}
		}
	}
	else{
		for (int i = 0; i < mClusterBox.size(); i++){
			MyBoundingBox box = mClusterBox[i][0];
			MyVec3f center = box.GetCenter();
			center += MyVec3f(box.GetWidth()/2+20, 0, 0);
			MyVec3f pos((box.GetBottom()+box.GetTop())/2, box.GetRight()+28.5,0);
			if(i%2){
				pos += MyVec3f(0,-3,0);
			}
			mMaskerLabelPos << pos;
			mMaskerLabelAlignment << 1;
		}
	}

	MyArrayStr labels;
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		labels << dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(i))->GetObject();
	}
	mLabelObjects.SetStrings(labels);
	mLabelObjects.Build();
}

void MyGraphSelectionMask::Show(){
	bool isMatrix = (dynamic_cast<const MyGraphMatrixLayout*>(mRepresentation->GetGraphLayout())!=0 );
	MyGraphDrawer::EnableAlplaBlending();
	const MyGenericHierarchyOverlayStr* selection = 
		dynamic_cast<const MyGenericHierarchyOverlayStr*>(mRepresentation->GetSelection());
	if(selection){
		if(selection->HasAnyChild()){
			selection = dynamic_cast<const MyGenericHierarchyOverlayStr*>(selection->GetChild(0));
		}
	}
	MyArrayStr selectionName;
	if(selection){
		const MyGenericNode* parent = selection->GetParent();
		for(int i = 0;i<parent->GetNumChildren();i++){
			MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>(parent->GetChild(i))->GetObject();
			selectionName << name;
		}
	}
	for(int i = 0;i<mClusterBox.size();i++){
		MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(i))->GetObject();
		if(!selectionName.HasOne(name)){
			if(!isMatrix){
				MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
				MyGraphDrawer::PushName(i);
			}
			/*
			for(int iface = 0;iface<6;iface++){
				MyVec4i faces = MyBoundingBox::GetFaceIndexSet(iface);
				MyGraphDrawer::DrawQuadAt(mClusterBox[i].GetCornerPos(faces[0]),
					mClusterBox[i].GetCornerPos(faces[1]),
					mClusterBox[i].GetCornerPos(faces[3]),
					mClusterBox[i].GetCornerPos(faces[2]));
			}
			*/
			//MyGraphDrawer::Color(MyColor4f(0.6, 0.6, 0.6, 0.3));
			MyGraphDrawer::Color(MyColor4f(0.8, 0.8, 0.8, 0.3));
			MyGraphDrawer::SetLineWidth(0.5);
			for(int iMarker = 0;iMarker < mRepresentation->GetNumOfTangibleMarker(0);iMarker ++){
				mClusterBox[i][iMarker].Show();
			}
			//MyGraphDrawer::Color(MyColor4f(1, 1, 1, 0.4));
			MyGraphDrawer::Color(MyColor4f(0, 0, 0, 0.4));
			MyGraphDrawer::WirePolygon();
			for(int iMarker = 0;iMarker < mRepresentation->GetNumOfTangibleMarker(0);iMarker ++){
				mClusterBox[i][iMarker].Show();
			}
			MyGraphDrawer::FillPolygon();
			if(!isMatrix){
				MyGraphDrawer::PopName();
				MyGraphDrawer::PopName();
			}
		}
	}
	
	for(int i = 0;i<mClusterBox.size();i++){
		MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(i))->GetObject();
		if(selectionName.HasOne(name)){
			if(!isMatrix){
				MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
				MyGraphDrawer::PushName(i);
			}
			MyGraphDrawer::Color(MyColor4f(0.75,0.75,0.75,0.5));
			MyGraphDrawer::SetLineWidth(0.5);
			for(int iMarker = 0;iMarker < mRepresentation->GetNumOfTangibleMarker(0);iMarker ++){
				mClusterBox[i][iMarker].Show();
			}
			if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::blue());
			else MyPrimitiveDrawer::Color(MyColor4f::yellow());
			MyGraphDrawer::WirePolygon();
			for(int iMarker = 0;iMarker < mRepresentation->GetNumOfTangibleMarker(0);iMarker ++){
				mClusterBox[i][iMarker].Show();
			}
			MyGraphDrawer::FillPolygon();
			if(!isMatrix){
				MyGraphDrawer::PopName();
				MyGraphDrawer::PopName();
			}
		}
	}

	
	for(int i = 0;i<mClusterBox.size();i++){
		MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(i))->GetObject();
		if(selectionName.HasOne(name)){
			if (mStaticHighlight.HasOne(i)){
				if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::orange());
				else MyPrimitiveDrawer::Color(MyColor4f::cyan());
			}
			else{
				if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::blue());
				else MyPrimitiveDrawer::Color(MyColor4f::yellow());
			}
		}
		else if (mStaticHighlight.HasOne(i)){
			MyGraphDrawer::Color(MyColor4f(1,0,0,1));
		}
		else{
			if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::gray());
			else MyPrimitiveDrawer::Color(MyColor4f::black());
		}

		int labelAlignment = 1;
		MyVec3f labelPos = mClusterBox[i][0].GetCornerPos(2);
		if (mMaskerLabelPos.size() == mClusterBox.size()
			&& mMaskerLabelAlignment.size() == mClusterBox.size()){
			labelPos = mMaskerLabelPos[i];
			labelAlignment = mMaskerLabelAlignment[i];
		}
		MyMatrixd mvmat = MyGraphDrawer::GetModelViewMatrix();
		float scale = mvmat.At(1,1);
		int width = MyGraphDrawer::GetBitMapTextLargeWidth(name);
		float widthf = width/scale;
		float heightf = 3;
		if(labelAlignment == 1) labelPos += MyVec3f(-widthf/2,0,0);
		if(labelAlignment == 2) labelPos += MyVec3f(-widthf,0,0);
		MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
		MyGraphDrawer::PushName(i);
		MyGraphDrawer::PushMatrix();
		MyGraphDrawer::Translate(labelPos);
		MyGraphDrawer::Scale(1/scale);
		mLabelObjects.GetTextObject(i).Show();
		MyGraphDrawer::PopMatrix();
		MyGraphDrawer::PopName();
		MyGraphDrawer::PopName();
		
		/*
		const MyGraphLayout* layout = mRepresentation->GetGraphLayout();
		const MyGraphMatrixEncodingRepresentation* matRep = 
			dynamic_cast<const MyGraphMatrixEncodingRepresentation*>(mRepresentation);
		if (matRep != 0
			&& matRep->GetLabelDraw()&MyGraphMatrixEncodingRepresentation::RIGHT){
			MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
			MyGraphDrawer::PushName(i);
			MyGraphDrawer::PushMatrix();
			MyGraphDrawer::Translate(MyVec3f(labelPos[1], labelPos[0]+15,0));
			MyGraphDrawer::Scale(1/scale);
			MyQuarternion qut(-MY_PI/2,0,0,1);
			MyGraphDrawer::MultiplyMatrix(&qut.GetMatrix().Transpose());
			mLabelObjects.GetTextObject(i).Show();
			MyGraphDrawer::PopMatrix();
			MyGraphDrawer::PopName();
			MyGraphDrawer::PopName();
		}
		*/
		/*
		MyGraphDrawer::SetLineWidth(3);
		//MyGraphDrawer::DrawBitMapTextLarge(mClusterBox[i][0].GetCornerPos(3), name);
		MyVec3f labelPos = mClusterBox[i][0].GetCornerPos(2);
		int labelAlignment = 1;
		if (mMaskerLabelPos.size() == mClusterBox.size()
			&& mMaskerLabelAlignment.size() == mClusterBox.size()){
			labelPos = mMaskerLabelPos[i];
			labelAlignment = mMaskerLabelAlignment[i];
		}
		MyGraphDrawer::DrawBitMapTextLarge(labelPos, name, labelAlignment);
		
		MyGraphDrawer::EnableAlplaBlending();

		int width = MyGraphDrawer::GetBitMapTextLargeWidth(name);
		float widthf = width*0.24;
		float heightf = 3;
		if(labelAlignment == 1) labelPos += MyVec3f(-widthf/2,0,0);
		if(labelAlignment == 2) labelPos += MyVec3f(-widthf,0,0);
		MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
		MyGraphDrawer::PushName(i);
		MyGraphDrawer::Color(MyColor4f(0,0,0,0));
		MyGraphicsTool::BeginQuads();
		MyGraphicsTool::Vertex(labelPos);
		MyGraphicsTool::Vertex(labelPos+MyVec3f(widthf,0,0));
		MyGraphicsTool::Vertex(labelPos+MyVec3f(widthf,heightf,0));
		MyGraphicsTool::Vertex(labelPos+MyVec3f(0,heightf,0));
		MyGraphicsTool::EndPrimitive();
		MyGraphDrawer::PopName();
		MyGraphDrawer::PopName();
		MyGraphDrawer::DisableAlplaBlending();
		*/
		
	}
	MyGraphDrawer::SetLineWidth(1);
	MyGraphDrawer::DisableAlplaBlending();
}

MyGenericNode* MyGraphSelectionMask::MakeSelection(int i) const{
	MyGenericNode* sel = mHierarchy->GetChild(i)->MakeCopy();
	sel->DestoryAscendants();
	MyGenericNode* np = new MyGenericHierarchyOverlayStr(
		dynamic_cast<MyGenericHierarchyOverlayStr*>(sel)->GetObject());
	MyGenericNode::Connect(np,sel);
	dynamic_cast<MyGenericHierarchyOverlayStr*>(sel)->SetObject("node");
	return np;
}

void MyGraphSelectionMask::AddStaticHighlight(int i){
	mStaticHighlight << i;
}