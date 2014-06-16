#include <brushTool/CBrushToolCommand.h>
#include <maya/MArgList.h>
#if 0
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#endif
#include <maya/MCommandResult.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#if 1
#include <maya/MUserEventMessage.h>
#endif
#ifdef _DEBUG
#include <assert.h>
#endif
#include <maya/MFnNumericData.h>
#include <maya/MQuaternion.h>
#if 1
#if 1
#include <maya/MFnVectorArrayData.h>
#endif
#include <tentacleShape/kTentacleEnum.h>
#include <baseShape/baseShape.h>
#endif
#if 1
#include <maya/MFnPointArrayData.h>
#endif
#include <float.h>
CBrushToolCommand::BRUSH_TYPE	CBrushToolCommand::BrushType = CBrushToolCommand::BT_INVALID;
void*	CBrushToolCommand::creator()
{
#ifdef move__DEBUG2
	cout << "CBrushToolCommand::creator()" << endl;
#endif
	switch (BrushType) {
#if 0
	case BT_CUT:
		return new CBrushToolCommand_Cut();
		break;
	case BT_SCALE:
		return new CBrushToolCommand_Scale();
		break;
#endif
	case BT_TRANSLATE:
		return new CBrushToolCommand_Translate();
		break;
	case BT_ROTATE:
		return new CBrushToolCommand_Rotate();
		break;
#if 1
#if 0
	case BT_TENTACLE_TWIST:
		return new CBrushToolCommand_Tentacle_Twist();
		break;
	case BT_TENTACLE_SHAPE:
		return new CBrushToolCommand_Tentacle_Shape();
		break;
#endif
#if 1
	case BT_TENTACLE_CREATE:
	case BT_TENTACLE_MOVE:
		return new CBrushToolCommand_Tentacle_Create();
		break;
#endif
#endif
	default:
		break;
	}
#if 1
	return new CBrushToolCommand_Translate();
#else
	return new CBrushToolCommand_Cut();
#endif
}
MStatus	CBrushToolCommand::finalize()
{
	MArgList	command;
	command.addArg(commandString());
	return MPxToolCommand::doFinalize(command);
}
MStatus	CBrushToolCommand::action(int, bool)
{
	return MStatus::kSuccess;
}
#if 0
MStatus	CBrushToolCommand_Cut::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Cut::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	InsideArray_Cut&	insideArray_Cut = __blobCut.insideArray();
	for (int n = 0; n < (int) insideArray_Cut.length(); ++n) {
		DataArray_Cut&	dataArray_Cut = insideArray_Cut(n);
		MFnDependencyNode	fnDepNode(dataArray_Cut.dagPath.node());
		MStatus	ms;
		MPlug	plugArray = fnDepNode.findPlug("seglen", false, &ms);
		for (int k = 0; k < (int) dataArray_Cut.dataArray.length(); ++k) {
			Data_Cut&	data_Cut = dataArray_Cut.dataArray(k);
			MPlug	plug = plugArray.elementByLogicalIndex(data_Cut.uIndex);
			plug.setValue(flag == UNDOIT ? data_Cut.segmentLength__ : data_Cut.segmentLength);
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Cut::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Cut&	insideArray_Cut = __blobCut.insideArray();
	for (int n = 0; n < (int) insideArray_Cut.length(); ++n) {
		DataArray_Cut&	dataArray_Cut = insideArray_Cut(n);
		const MDagPath&	dagPath = dataArray_Cut.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Cut: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Cut.dataArray.length(); ++k) {
			Data_Cut&	data_Cut = dataArray_Cut.dataArray(k);
			int&	uIndex = data_Cut.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Cut: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			MVectorArray&	offsets = activeGuidePtr->offsets;
			for (int i = 1; i < (int) offsets.length(); ++i) {
				MVector	offset = offsets[i];
				offset.normalize();
				offsets[i] = (undo ? data_Cut.segmentLength__ : data_Cut.segmentLength) * offset;
			}
		}
	}
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Scale::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Scale::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	InsideArray_Scale&	insideArray_Scale = __blobScale.insideArray();
	for (int n = 0; n < (int) insideArray_Scale.length(); ++n) {
		DataArray_Scale&	dataArray_Scale = insideArray_Scale(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Scale.dagPath.node());
		MPlug	plugArray = fnDepNode.findPlug("seglen", false, &ms);
		for (int k = 0; k < (int) dataArray_Scale.dataArray.length(); ++k) {
			Data_Scale&	data_Scale = dataArray_Scale.dataArray(k);
			MPlug	plug = plugArray.elementByLogicalIndex(data_Scale.uIndex);
			plug.setValue(flag == UNDOIT ? data_Scale.segmentLength__ : data_Scale.segmentLength);
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Scale::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Scale&	insideArray_Scale = __blobScale.insideArray();
	for (int n = 0; n < (int) insideArray_Scale.length(); ++n) {
		DataArray_Scale&	dataArray_Scale = insideArray_Scale(n);
		const MDagPath&	dagPath = dataArray_Scale.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Scale: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Scale.dataArray.length(); ++k) {
			Data_Scale&	data_Scale = dataArray_Scale.dataArray(k);
			int&	uIndex = data_Scale.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Scale: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			MVectorArray&	offsets = activeGuidePtr->offsets;
			for (int i = 1; i < (int) offsets.length(); ++i) {
				MVector	offset = offsets[i];
				offset.normalize();
				offsets[i] = (undo ? data_Scale.segmentLength__ : data_Scale.segmentLength) * offset;
			}
		}
	}
	return MStatus::kSuccess;
}
#endif
const double	CBrushToolCommand_Translate::TRANSLATE_SCALE_FACTOR	= 0.05;
MStatus	CBrushToolCommand_Translate::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Translate::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	const Arg_Translate&	arg_Translate = __blobTranslate.arg();
	MVector	deltaX = arg_Translate.deltaX;
	InsideArray_Translate&	insideArray_Translate = __blobTranslate.insideArray();
	for (int n = 0; n < (int) insideArray_Translate.length(); ++n) {
		DataArray_Translate&	dataArray_Translate = insideArray_Translate(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Translate.dagPath.node());
#if 1
		MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
#else
		MPlug	plug_cp_array = fnDepNode.findPlug("controlPoints", true, &ms);
#if 1
#if 0
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
#endif
#endif
#endif
		int	numPoints = dataArray_Translate.numPoints;
		MMatrix	worldMatrix = dataArray_Translate.worldMatrix;
		for (int k = 0; k < (int) dataArray_Translate.dataArray.length(); ++k) {
			Data_Translate&	data_Translate = dataArray_Translate.dataArray(k);
			int	index = data_Translate.uIndex;
#if 1
			MVectorArray	before_offsets, after_offsets;
			if (dataArray_Translate.hasSkinCluster) {
				for (unsigned i = 1; i < data_Translate.offsets__.length(); ++i) {
					MVector	offset = data_Translate.offsets__[i] - data_Translate.offsets__[i - 1];
					ms = before_offsets.append(offset);
					ms = after_offsets.append(offset);
				}
			}
#endif
			if (flag == REDOIT) {
				MPoint	basePoint = data_Translate.offsets__[0];
				basePoint *= worldMatrix;
				MPoint	worldPt__ = basePoint;
				MPoint	lastWorldPt__ = basePoint;
				unsigned	bit = 1 << 1;
#if 1
				if (dataArray_Translate.hasSkinCluster) {
					lastWorldPt__ = data_Translate.offsets__[0];
				}
#endif
				for (int i = 1; i < numPoints; ++i, bit <<= 1, lastWorldPt__ = worldPt__) {
					const MVector&	offset__ = data_Translate.offsets__[i];
					double		len__ = offset__.length();
					const MVector	normal__ = offset__.transformAsNormal(worldMatrix);
					worldPt__ += len__ * normal__;
#if 1
					if (dataArray_Translate.hasSkinCluster) {
						worldPt__ = data_Translate.offsets__[i];
					}
#endif
					if (!(data_Translate.vertexBits & bit))
						continue;
					MPoint	P = worldPt__ + TRANSLATE_SCALE_FACTOR * data_Translate.factorArray[i] * deltaX;
					MVector	v = P - lastWorldPt__;
					v.normalize();
					if (abs(v.length() - 1) > DBL_EPSILON) {
						cout << "n = " << n << ": v.length() = " << v.length() << ": skipped" << endl;
						continue;
					}
#if 1
				if (dataArray_Translate.hasSkinCluster) {
					after_offsets[i - 1] = MVector(worldPt__ - lastWorldPt__).length() * v;
				} else {
#endif
					data_Translate.offsets[i] = len__ * v;
#if 1
				}
#endif
#if 1
					MQuaternion	q(data_Translate.offsets__[i], data_Translate.offsets[i]);
					data_Translate.transversals[i - 1] = data_Translate.transversals__[i - 1].rotateBy(q);
					if (i + 1 == numPoints) {
						data_Translate.transversals[i] = data_Translate.transversals__[i].rotateBy(q);
					}
#endif
				}
#if 1
				if (dataArray_Translate.hasSkinCluster) {
					for (unsigned i = 0; i < after_offsets.length(); ++i) {
						data_Translate.offsets[1 + i] = data_Translate.offsets[i] + after_offsets[i];
					}
				}
#endif
			}
#if 1
			MPlug	plug_tentacleArray_element = plug_tentacleArray.elementByPhysicalIndex(index, &ms);
			MFnVectorArrayData	vectorData;
			MObject			valData;
#if 1
		if (dataArray_Translate.hasSkinCluster) {
			MVectorArray	vectorArray = flag == UNDOIT ? before_offsets : after_offsets;
			MPointArray	quaternionArray;
			MVector	v0 = data_Translate.root_wNormal;
			v0 *= vectorArray[0].length();
			ms = vectorArray.insert(v0, 0);
			for (unsigned i = 1; i < vectorArray.length(); ++i) {
				MVector	from = vectorArray[i - 1];
				MVector	to = vectorArray[i];
				MQuaternion	q(from, to);
				if (i == 1)
					cout << q << endl;
				MPoint	p(q.x, q.y, q.z, q.w);
				ms = quaternionArray.append(p);
			}
			MFnPointArrayData	quaternionArrayData;
			valData = quaternionArrayData.create(quaternionArray, &ms);
			MPlug	plug_quaternion = plug_tentacleArray_element.child(TENTACLE_ARRAY_QUATERNION, &ms);
			ms = plug_quaternion.setValue(valData);
		} else {
#endif
			MVectorArray	longitudinals = flag == UNDOIT ? data_Translate.offsets__ : data_Translate.offsets;
			ms = longitudinals.remove(0);
			valData = vectorData.create(longitudinals, &ms);
			MPlug	plug_longitudinal = plug_tentacleArray_element.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			ms = plug_longitudinal.setValue(valData);
#if 1
		}
#endif
			MVectorArray&	transversals = flag == UNDOIT ? data_Translate.transversals__ : data_Translate.transversals;
			valData = vectorData.create(transversals, &ms);
			MPlug	plug_transversal = plug_tentacleArray_element.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
			ms = plug_transversal.setValue(valData);
			MPlug	plug_tentacleDoAction = fnDepNode.findPlug("tentacleDoAction", true, &ms);
			ms = plug_tentacleDoAction.setValue(TentacleDo::None);
#else
			unsigned	bit = 1 << 1;
			for (int i = 1; i < numPoints; ++i, bit <<= 1) {
				if (!(data_Translate.vertexBits & bit))
					continue;
				const MVector&	before = data_Translate.offsets__[i];
				const MVector&	after = data_Translate.offsets[i];
				MPlug	plug_compound = plug_cp_array.elementByLogicalIndex(index * numPoints + i);
				MFnNumericData	offset_val;
				MObject	offset_data = offset_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	offset = flag == UNDOIT ? before : after;
				ms = offset_val.setData(offset.x, offset.y, offset.z);
				ms = plug_compound.setValue(offset_data);
#if 1
#if 0
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i - 1);
				MFnNumericData	transversal_val;
				MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	transversal = flag == UNDOIT ? data_Translate.transversals__[i - 1] : data_Translate.transversals[i - 1];
				ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
				ms = plug_element_transversal.setValue(transversal_data);
				if (i + 1 == numPoints) {
					MPlug	plug_end_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i);
					MFnNumericData	transversal_val;
					MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
					const MVector&	transversal = flag == UNDOIT ? data_Translate.transversals__[i] : data_Translate.transversals[i];
					ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
					ms = plug_end_element_transversal.setValue(transversal_data);
				}
#endif
#endif
			}
#endif
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Translate::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Translate&	insideArray_Translate = __blobTranslate.insideArray();
	for (int n = 0; n < (int) insideArray_Translate.length(); ++n) {
		DataArray_Translate&	dataArray_Translate = insideArray_Translate(n);
		const MDagPath&	dagPath = dataArray_Translate.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Translate: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Translate.dataArray.length(); ++k) {
			Data_Translate&	data_Translate = dataArray_Translate.dataArray(k);
			int&	uIndex = data_Translate.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Translate: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			activeGuidePtr->offsets = undo ? data_Translate.offsets__ : data_Translate.offsets;
#if 1
			activeGuidePtr->transversals = undo ? data_Translate.transversals__ : data_Translate.transversals;
#endif
		}
	}
	return MStatus::kSuccess;
}
const double	CBrushToolCommand_Rotate::ROTATE_NORMALIZATION		= M_PI / 4;
MStatus	CBrushToolCommand_Rotate::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Rotate::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	const Arg_Rotate&	arg_Rotate = __blobRotate.arg();
	InsideArray_Rotate&	insideArray_Rotate = __blobRotate.insideArray();
	for (int n = 0; n < (int) insideArray_Rotate.length(); ++n) {
		DataArray_Rotate&	dataArray_Rotate = insideArray_Rotate(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Rotate.dagPath.node());
#if 1
		MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
#else
		MPlug	plug_cp_array = fnDepNode.findPlug("controlPoints", true, &ms);
#if 1
#if 0
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
#endif
#endif
#endif
		int	numPoints = dataArray_Rotate.numPoints;
		MMatrix	worldMatrix = dataArray_Rotate.worldMatrix;
		MMatrix	inverseWorldMatrix = worldMatrix.inverse();
		for (int k = 0; k < (int) dataArray_Rotate.dataArray.length(); ++k) {
			Data_Rotate&	data_Rotate = dataArray_Rotate.dataArray(k);
			int	index = data_Rotate.uIndex;
#if 1
			MVectorArray	before_offsets, after_offsets;
			if (dataArray_Rotate.hasSkinCluster) {
				for (unsigned i = 1; i < data_Rotate.offsets__.length(); ++i) {
					MVector	offset = data_Rotate.offsets__[i] - data_Rotate.offsets__[i - 1];
					ms = before_offsets.append(offset);
					ms = after_offsets.append(offset);
				}
			}
#endif
			if (flag == REDOIT) {
				unsigned	bit = 1 << 1;
				for (int i = 1; i < numPoints; ++i, bit <<= 1) {
					if (!(data_Rotate.vertexBits & bit))
						continue;
					const MMatrix		rotationMatrix = MQuaternion(data_Rotate.factorArray[i] * arg_Rotate.byAngleAmount, arg_Rotate.axis).asMatrix();
#if 1
				if (dataArray_Rotate.hasSkinCluster) {
					after_offsets[i - 1] = before_offsets[i - 1] * rotationMatrix;
				} else {
#endif
					MMatrix	m = worldMatrix * rotationMatrix * inverseWorldMatrix;
					data_Rotate.offsets[i] = data_Rotate.offsets__[i] * m;
#if 1
				}
#endif
#if 1
					MQuaternion	q(data_Rotate.offsets__[i], data_Rotate.offsets[i]);
					data_Rotate.transversals[i - 1] = data_Rotate.transversals__[i - 1].rotateBy(q);
					if (i + 1 == numPoints) {
						data_Rotate.transversals[i] = data_Rotate.transversals__[i].rotateBy(q);
					}
#endif
				}
#if 1
				if (dataArray_Rotate.hasSkinCluster) {
					for (unsigned i = 0; i < after_offsets.length(); ++i) {
						data_Rotate.offsets[1 + i] = data_Rotate.offsets[i] + after_offsets[i];
					}
				}
#endif
			}
#if 1
			MPlug	plug_tentacleArray_element = plug_tentacleArray.elementByPhysicalIndex(index, &ms);
			MFnVectorArrayData	vectorData;
			MObject			valData;
#if 1
		if (dataArray_Rotate.hasSkinCluster) {
			MVectorArray	vectorArray = flag == UNDOIT ? before_offsets : after_offsets;
			MPointArray	quaternionArray;
			MVector	v0 = data_Rotate.root_wNormal;
			v0 *= vectorArray[0].length();
			ms = vectorArray.insert(v0, 0);
			for (unsigned i = 1; i < vectorArray.length(); ++i) {
				MVector	from = vectorArray[i - 1];
				MVector	to = vectorArray[i];
				MQuaternion	q(from, to);
				if (i == 1)
					cout << q << endl;
				MPoint	p(q.x, q.y, q.z, q.w);
				ms = quaternionArray.append(p);
			}
			MFnPointArrayData	quaternionArrayData;
			valData = quaternionArrayData.create(quaternionArray, &ms);
			MPlug	plug_quaternion = plug_tentacleArray_element.child(TENTACLE_ARRAY_QUATERNION, &ms);
			ms = plug_quaternion.setValue(valData);
		} else {
#endif
			MVectorArray	longitudinals = flag == UNDOIT ? data_Rotate.offsets__ : data_Rotate.offsets;
			ms = longitudinals.remove(0);
			valData = vectorData.create(longitudinals, &ms);
			MPlug	plug_longitudinal = plug_tentacleArray_element.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			ms = plug_longitudinal.setValue(valData);
#if 1
		}
#endif
			MVectorArray&	transversals = flag == UNDOIT ? data_Rotate.transversals__ : data_Rotate.transversals;
			valData = vectorData.create(transversals, &ms);
			MPlug	plug_transversal = plug_tentacleArray_element.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
			ms = plug_transversal.setValue(valData);
			MPlug	plug_tentacleDoAction = fnDepNode.findPlug("tentacleDoAction", true, &ms);
			ms = plug_tentacleDoAction.setValue(TentacleDo::None);
#else
			unsigned	bit = 1 << 1;
			for (int i = 1; i < numPoints; ++i, bit <<= 1) {
				if (!(data_Rotate.vertexBits & bit))
					continue;
				MPlug	plug_compound = plug_cp_array.elementByLogicalIndex(index * numPoints + i);
				MFnNumericData	offset_val;
				MObject	offset_data = offset_val.create(MFnNumericData::k3Double, &ms);
				MVector&	offset = flag == UNDOIT ? data_Rotate.offsets__[i] : data_Rotate.offsets[i];
				ms = offset_val.setData(offset.x, offset.y, offset.z);
				ms = plug_compound.setValue(offset_data);
#if 1
#if 0
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i - 1);
				MFnNumericData	transversal_val;
				MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	transversal = flag == UNDOIT ? data_Rotate.transversals__[i - 1] : data_Rotate.transversals[i - 1];
				ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
				ms = plug_element_transversal.setValue(transversal_data);
				if (i + 1 == numPoints) {
					MPlug	plug_end_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i);
					MFnNumericData	transversal_val;
					MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
					const MVector&	transversal = flag == UNDOIT ? data_Rotate.transversals__[i] : data_Rotate.transversals[i];
					ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
					ms = plug_end_element_transversal.setValue(transversal_data);
				}
#endif
#endif
			}
#endif
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Rotate::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Rotate&	insideArray_Rotate = __blobRotate.insideArray();
	for (int n = 0; n < (int) insideArray_Rotate.length(); ++n) {
		DataArray_Rotate&	dataArray_Rotate = insideArray_Rotate(n);
		const MDagPath&	dagPath = dataArray_Rotate.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Rotate: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Rotate.dataArray.length(); ++k) {
			Data_Rotate&	data_Rotate = dataArray_Rotate.dataArray(k);
			int&	uIndex = data_Rotate.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Rotate: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			activeGuidePtr->offsets = undo ? data_Rotate.offsets__ : data_Rotate.offsets;
#if 1
			activeGuidePtr->transversals = undo ? data_Rotate.transversals__ : data_Rotate.transversals;
#endif
		}
	}
	return MStatus::kSuccess;
}
#if 1
#if 0
const double	CBrushToolCommand_Tentacle_Twist::ROTATE_NORMALIZATION		= M_PI / 4;
MStatus	CBrushToolCommand_Tentacle_Twist::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Tentacle_Twist::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	const Arg_Tentacle_Twist&	arg_Tentacle_Twist = __blobTentacleTwist.arg();
	InsideArray_Tentacle_Twist&	insideArray_Tentacle_Twist = __blobTentacleTwist.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Twist.length(); ++n) {
		DataArray_Tentacle_Twist&	dataArray_Tentacle_Twist = insideArray_Tentacle_Twist(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Tentacle_Twist.dagPath.node());
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
		int	numPoints = dataArray_Tentacle_Twist.numPoints;
		MMatrix	worldMatrix = dataArray_Tentacle_Twist.worldMatrix;
		MMatrix	inverseWorldMatrix = worldMatrix.inverse();
		for (int k = 0; k < (int) dataArray_Tentacle_Twist.dataArray.length(); ++k) {
			Data_Tentacle_Twist&	data_Tentacle_Twist = dataArray_Tentacle_Twist.dataArray(k);
			int	index = data_Tentacle_Twist.uIndex;
			if (flag == REDOIT) {
				unsigned	bit = 1 << 0;
				for (int i = 0; i < numPoints; ++i, bit <<= 1) {
					if (!(data_Tentacle_Twist.vertexBits & bit))
						continue;
					MQuaternion	q(arg_Tentacle_Twist.byAngleAmount, data_Tentacle_Twist.longitudinals[i + 1 == numPoints ? i : i + 1]);
					data_Tentacle_Twist.transversals[i] = data_Tentacle_Twist.transversals__[i].rotateBy(q);
				}
			}
			unsigned	bit = 1 << 0;
			for (int i = 0; i < numPoints; ++i, bit <<= 1) {
				if (!(data_Tentacle_Twist.vertexBits & bit))
					continue;
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i);
				MFnNumericData	transversal_val;
				MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	transversal = flag == UNDOIT ? data_Tentacle_Twist.transversals__[i] : data_Tentacle_Twist.transversals[i];
				ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
				ms = plug_element_transversal.setValue(transversal_data);
			}
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Tentacle_Twist::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Tentacle_Twist&	insideArray_Tentacle_Twist = __blobTentacleTwist.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Twist.length(); ++n) {
		DataArray_Tentacle_Twist&	dataArray_Tentacle_Twist = insideArray_Tentacle_Twist(n);
		const MDagPath&	dagPath = dataArray_Tentacle_Twist.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Tentacle_Twist: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Tentacle_Twist.dataArray.length(); ++k) {
			Data_Tentacle_Twist&	data_Tentacle_Twist = dataArray_Tentacle_Twist.dataArray(k);
			int&	uIndex = data_Tentacle_Twist.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Tentacle_Twist: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			activeGuidePtr->transversals = undo ? data_Tentacle_Twist.transversals__ : data_Tentacle_Twist.transversals;
		}
	}
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Tentacle_Shape::action(int flag, bool byMaya)
{
#ifdef _DEBUG
	cout << "CBrushToolCommand_Tentacle_Shape::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	const Arg_Tentacle_Shape&	arg_Tentacle_Shape = __blobTentacleShape.arg();
	InsideArray_Tentacle_Shape&	insideArray_Tentacle_Shape = __blobTentacleShape.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Shape.length(); ++n) {
		DataArray_Tentacle_Shape&	dataArray_Tentacle_Shape = insideArray_Tentacle_Shape(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Tentacle_Shape.dagPath.node());
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
		int	numPoints = dataArray_Tentacle_Shape.numPoints;
		MMatrix	worldMatrix = dataArray_Tentacle_Shape.worldMatrix;
		MMatrix	inverseWorldMatrix = worldMatrix.inverse();
		for (int k = 0; k < (int) dataArray_Tentacle_Shape.dataArray.length(); ++k) {
			Data_Tentacle_Shape&	data_Tentacle_Shape = dataArray_Tentacle_Shape.dataArray(k);
			int	index = data_Tentacle_Shape.uIndex;
			if (flag == REDOIT) {
				unsigned	bit = 1 << 0;
				for (int i = 0; i < numPoints; ++i, bit <<= 1) {
					if (!(data_Tentacle_Shape.vertexBits & bit))
						continue;
					cout << "factor = " << arg_Tentacle_Shape.factor << endl;
					data_Tentacle_Shape.transversals[i] = arg_Tentacle_Shape.factor * data_Tentacle_Shape.transversals__[i];
				}
			}
			unsigned	bit = 1 << 0;
			for (int i = 0; i < numPoints; ++i, bit <<= 1) {
				if (!(data_Tentacle_Shape.vertexBits & bit))
					continue;
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i);
				MFnNumericData	transversal_val;
				MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	transversal = flag == UNDOIT ? data_Tentacle_Shape.transversals__[i] : data_Tentacle_Shape.transversals[i];
				ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
				ms = plug_element_transversal.setValue(transversal_data);
			}
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
}
MStatus	CBrushToolCommand_Tentacle_Shape::rebuildActiveSelectionList(bool undo)
{
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Tentacle_Shape&	insideArray_Tentacle_Shape = __blobTentacleShape.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Shape.length(); ++n) {
		DataArray_Tentacle_Shape&	dataArray_Tentacle_Shape = insideArray_Tentacle_Shape(n);
		const MDagPath&	dagPath = dataArray_Tentacle_Shape.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Tentacle_Shape: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Tentacle_Shape.dataArray.length(); ++k) {
			Data_Tentacle_Shape&	data_Tentacle_Shape = dataArray_Tentacle_Shape.dataArray(k);
			int&	uIndex = data_Tentacle_Shape.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Tentacle_Shape: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			activeGuidePtr->transversals = undo ? data_Tentacle_Shape.transversals__ : data_Tentacle_Shape.transversals;
		}
	}
	return MStatus::kSuccess;
}
#endif
#if 1
MStatus	CBrushToolCommand_Tentacle_Create::action(int , bool )
{
	return MStatus::kSuccess;
#if 0
#ifdef _DEBUG
	cout << "CBrushToolCommand_Tentacle_Create::action(): ACTION:= " << (flag == UNDOIT ? "UNDO" : (flag == REDOIT ? "REDO" : "DO")) << endl;
#endif
	if (flag != UNDOIT && flag != REDOIT)
		return MStatus::kSuccess;
	const Arg_Tentacle_Create&	arg_Tentacle_Create = __blobTentacleCreate.arg();
	InsideArray_Tentacle_Create&	insideArray_Tentacle_Create = __blobTentacleCreate.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Create.length(); ++n) {
		DataArray_Tentacle_Create&	dataArray_Tentacle_Create = insideArray_Tentacle_Create(n);
		MStatus	ms;
		MFnDependencyNode	fnDepNode(dataArray_Tentacle_Create.dagPath.node());
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
#if 0
		int	numPoints = dataArray_Tentacle_Create.numPoints;
#endif
		MMatrix	worldMatrix = dataArray_Tentacle_Create.worldMatrix;
		MMatrix	inverseWorldMatrix = worldMatrix.inverse();
		for (int k = 0; k < (int) dataArray_Tentacle_Create.dataArray.length(); ++k) {
#if 0
			Data_Tentacle_Create&	data_Tentacle_Create = dataArray_Tentacle_Create.dataArray(k);
			int	index = data_Tentacle_Create.uIndex;
			if (flag == REDOIT) {
				unsigned	bit = 1 << 0;
				for (int i = 0; i < numPoints; ++i, bit <<= 1) {
					if (!(data_Tentacle_Create.vertexBits & bit))
						continue;
					cout << "factor = " << arg_Tentacle_Create.factor << endl;
					data_Tentacle_Create.transversals[i] = arg_Tentacle_Create.factor * data_Tentacle_Create.transversals__[i];
				}
			}
			unsigned	bit = 1 << 0;
			for (int i = 0; i < numPoints; ++i, bit <<= 1) {
				if (!(data_Tentacle_Create.vertexBits & bit))
					continue;
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + i);
				MFnNumericData	transversal_val;
				MObject	transversal_data = transversal_val.create(MFnNumericData::k3Double, &ms);
				const MVector&	transversal = flag == UNDOIT ? data_Tentacle_Create.transversals__[i] : data_Tentacle_Create.transversals[i];
				ms = transversal_val.setData(transversal.x, transversal.y, transversal.z);
				ms = plug_element_transversal.setValue(transversal_data);
			}
#endif
		}
	}
#if 1
	if (byMaya)
		return rebuildActiveSelectionList(flag == UNDOIT);
#endif
	return MStatus::kSuccess;
#endif
}
MStatus	CBrushToolCommand_Tentacle_Create::rebuildActiveSelectionList(bool )
{
	return MStatus::kSuccess;
#if 0
	if (!__activeSelectionArrayPtr) {
		cerr << "Error: __activeSelectionArrayPtr is nil: Call setActiveSelectionArrayPtr first!" << endl;
		return MStatus::kFailure;
	}
	SelectionArray&	activeSelectionArray = *__activeSelectionArrayPtr;
	InsideArray_Tentacle_Create&	insideArray_Tentacle_Create = __blobTentacleCreate.insideArray();
	for (int n = 0; n < (int) insideArray_Tentacle_Create.length(); ++n) {
		DataArray_Tentacle_Create&	dataArray_Tentacle_Create = insideArray_Tentacle_Create(n);
		const MDagPath&	dagPath = dataArray_Tentacle_Create.dagPath;
		SelectionPtr	activeSelectionPtr = 0;
		for (int i = 0; i < (int) activeSelectionArray.length(); ++i) {
			SelectionPtr	selectionPtr = activeSelectionArray[i];
			if (dagPath == selectionPtr->dagPath) {
				activeSelectionPtr = selectionPtr;
				break;
			}
		}
		if (!activeSelectionPtr) {
			cout << "Info: rebuildActiveSelectionList_Tentacle_Create: |" << dagPath.fullPathName() << "|: Not selected" << endl;
			continue;
		}
		for (int k = 0; k < (int) dataArray_Tentacle_Create.dataArray.length(); ++k) {
#if 0
			Data_Tentacle_Create&	data_Tentacle_Create = dataArray_Tentacle_Create.dataArray(k);
			int&	uIndex = data_Tentacle_Create.uIndex;
			SelectedGuidePtr	activeGuidePtr = 0;
			for (int i = 0; i < (int) activeSelectionPtr->guideArray.length(); ++i) {
				SelectedGuidePtr	guidePtr = activeSelectionPtr->guideArray[i];
				if (uIndex == guidePtr->uIndex) {
					activeGuidePtr = guidePtr;
					break;
				}
			}
			if (!activeGuidePtr) {
				cout << "Info: rebuildActiveSelectionList_Tentacle_Create: |" << dagPath.fullPathName() << "|.guide[" << k << "]: Not selected" << endl;
				continue;
			}
			activeGuidePtr->transversals = undo ? data_Tentacle_Create.transversals__ : data_Tentacle_Create.transversals;
#endif
		}
	}
	return MStatus::kSuccess;
#endif
}
void	CBrushToolCommand_Tentacle_Create::doPress(const MDagPath& nodeDagPath) const
{
	bool		hitted = __arg_Tentacle_Create.hitted;
#if 1
	int	polyId = __arg_Tentacle_Create.polyId;
	const MPoint&	position = __arg_Tentacle_Create.position;
	const MVector&	normal = __arg_Tentacle_Create.normal;
#if 1
#if 1
	const int*	hitVertexList = __arg_Tentacle_Create.hitVertexList;
#else
	int	hitTriangleId = __arg_Tentacle_Create.hitTriangleId;
#endif
	float	hitBary1 = __arg_Tentacle_Create.hitBary1;
	float	hitBary2 = __arg_Tentacle_Create.hitBary2;
#endif
#else
	const MFloatPoint&	hitPoint = __arg_Tentacle_Create.hitPoint;
#endif
#if 0
	cout << "CBrushToolCommand_Tentacle_Create::doPress: hitted? " << (hitted ? "True" : "False");
	if (hitted)
		cout << ": " << hitPoint << endl;
	else
		cout << endl;
#endif
	if (!hitted)
		return;
	MStatus	ms;
	MFnDependencyNode	depNode(nodeDagPath.node(), &ms);
	if (!ms) {
		cerr << "CBrushToolCommand_Tentacle_Create::doPress: MFnDependencyNode::MFnDependencyNode(MObject&): Failure: |" << ms << "|" << endl;
		return;
	}
	MPlug	plug_hitted = depNode.findPlug("drawTentacleRoot", true, &ms);
	ms = plug_hitted.setValue(true);
#if 1
	MPlug	plug_hit = depNode.findPlug("tentacleRoot", true, &ms);
	MPlug	plug_hit_polyId = plug_hit.child(TENTACLE_ROOT_POLY_ID, &ms);
	plug_hit_polyId.setValue(polyId);
	MPlug	plug_hit_position = plug_hit.child(TENTACLE_ROOT_POSITION, &ms);
	MFnNumericData	hit_position_val;
	MObject	hit_position_data = hit_position_val.create(MFnNumericData::k3Double, &ms);
	ms = hit_position_val.setData(position.x, position.y, position.z);
	ms = plug_hit_position.setValue(hit_position_data);
	MPlug	plug_hit_normal = plug_hit.child(TENTACLE_ROOT_NORMAL, &ms);
	MFnNumericData	hit_normal_val;
	MObject	hit_normal_data = hit_normal_val.create(MFnNumericData::k3Double, &ms);
	ms = hit_normal_val.setData(normal.x, normal.y, normal.z);
	ms = plug_hit_normal.setValue(hit_normal_data);
#if 1
#if 1
	MPlug	plug_hit_vertexList = plug_hit.child(TENTACLE_ROOT_HIT_VERTEX_LIST, &ms);
	MFnNumericData	hit_vertexList_val;
	MObject	hit_vertexList_data = hit_vertexList_val.create(MFnNumericData::k3Int, &ms);
	ms = hit_vertexList_val.setData(hitVertexList[0], hitVertexList[1], hitVertexList[2]);
	ms = plug_hit_vertexList.setValue(hit_vertexList_data);
#else
	MPlug	plug_hit_triangleId = plug_hit.child(TENTACLE_ROOT_HIT_TRIANGLE_ID, &ms);
	plug_hit_triangleId.setValue(hitTriangleId);
#endif
	MPlug	plug_hit_bary1 = plug_hit.child(TENTACLE_ROOT_HIT_BARY1, &ms);
	plug_hit_bary1.setValue(hitBary1);
	MPlug	plug_hit_bary2 = plug_hit.child(TENTACLE_ROOT_HIT_BARY2, &ms);
	plug_hit_bary2.setValue(hitBary2);
#endif
#else
	MPlug	plug_hitPoint = depNode.findPlug("tentacleRoot", true, &ms);
	MFnNumericData	hitPoint_val;
	MObject	hitPoint_data = hitPoint_val.create(MFnNumericData::k3Float, &ms);
	ms = hitPoint_val.setData(hitPoint.x, hitPoint.y, hitPoint.z);
	ms = plug_hitPoint.setValue(hitPoint_data);
#endif
}
void	CBrushToolCommand_Tentacle_Create::doDrag(const MDagPath& nodeDagPath) const
{
	bool		hitted = __arg_Tentacle_Create.hitted;
#if 1
	int	polyId = __arg_Tentacle_Create.polyId;
	const MPoint&	position = __arg_Tentacle_Create.position;
	const MVector&	normal = __arg_Tentacle_Create.normal;
#if 1
#if 1
	const int*	hitVertexList = __arg_Tentacle_Create.hitVertexList;
#else
	int	hitTriangleId = __arg_Tentacle_Create.hitTriangleId;
#endif
	float	hitBary1 = __arg_Tentacle_Create.hitBary1;
	float	hitBary2 = __arg_Tentacle_Create.hitBary2;
#endif
#else
	const MFloatPoint&	hitPoint = __arg_Tentacle_Create.hitPoint;
#endif
#if 0
	cout << "CBrushToolCommand_Tentacle_Create::doDrag: hitted? " << (hitted ? "True" : "False");
	if (hitted)
		cout << ": " << hitPoint << endl;
	else
		cout << endl;
#endif
	if (!hitted)
		return;
	MStatus	ms;
	MFnDependencyNode	depNode(nodeDagPath.node(), &ms);
	if (!ms) {
		cerr << "CBrushToolCommand_Tentacle_Create::doDrag: MFnDependencyNode::MFnDependencyNode(MObject&): Failure: |" << ms << "|" << endl;
		return;
	}
	MPlug	plug_hitted = depNode.findPlug("drawTentacleRoot", true, &ms);
	ms = plug_hitted.setValue(true);
#if 1
	MPlug	plug_hit = depNode.findPlug("tentacleRoot", true, &ms);
	MPlug	plug_hit_polyId = plug_hit.child(TENTACLE_ROOT_POLY_ID, &ms);
	plug_hit_polyId.setValue(polyId);
	MPlug	plug_hit_position = plug_hit.child(TENTACLE_ROOT_POSITION, &ms);
	MFnNumericData	hit_position_val;
	MObject	hit_position_data = hit_position_val.create(MFnNumericData::k3Double, &ms);
	ms = hit_position_val.setData(position.x, position.y, position.z);
	ms = plug_hit_position.setValue(hit_position_data);
	MPlug	plug_hit_normal = plug_hit.child(TENTACLE_ROOT_NORMAL, &ms);
	MFnNumericData	hit_normal_val;
	MObject	hit_normal_data = hit_normal_val.create(MFnNumericData::k3Double, &ms);
	ms = hit_normal_val.setData(normal.x, normal.y, normal.z);
	ms = plug_hit_normal.setValue(hit_normal_data);
#if 1
#if 1
	MPlug	plug_hit_vertexList = plug_hit.child(TENTACLE_ROOT_HIT_VERTEX_LIST, &ms);
	MFnNumericData	hit_vertexList_val;
	MObject	hit_vertexList_data = hit_vertexList_val.create(MFnNumericData::k3Int, &ms);
	ms = hit_vertexList_val.setData(hitVertexList[0], hitVertexList[1], hitVertexList[2]);
	ms = plug_hit_vertexList.setValue(hit_vertexList_data);
#else
	MPlug	plug_hit_triangleId = plug_hit.child(TENTACLE_ROOT_HIT_TRIANGLE_ID, &ms);
	plug_hit_triangleId.setValue(hitTriangleId);
#endif
	MPlug	plug_hit_bary1 = plug_hit.child(TENTACLE_ROOT_HIT_BARY1, &ms);
	plug_hit_bary1.setValue(hitBary1);
	MPlug	plug_hit_bary2 = plug_hit.child(TENTACLE_ROOT_HIT_BARY2, &ms);
	plug_hit_bary2.setValue(hitBary2);
#endif
#else
	MPlug	plug_hitPoint = depNode.findPlug("tentacleRoot", true, &ms);
	MFnNumericData	hitPoint_val;
	MObject	hitPoint_data = hitPoint_val.create(MFnNumericData::k3Float, &ms);
	ms = hitPoint_val.setData(hitPoint.x, hitPoint.y, hitPoint.z);
	ms = plug_hitPoint.setValue(hitPoint_data);
#endif
}
void	CBrushToolCommand_Tentacle_Create::doRelease(const MDagPath& nodeDagPath) const
{
	bool		hitted = __arg_Tentacle_Create.hitted;
#if 0
	const MFloatPoint&	hitPoint = __arg_Tentacle_Create.hitPoint;
	cout << "CBrushToolCommand_Tentacle_Create::doRelease: hitted? " << (hitted ? "True" : "False");
	if (hitted)
		cout << ": " << hitPoint << endl;
	else
		cout << endl;
#endif
	if (!hitted)
		return;
	MStatus	ms;
	MFnDependencyNode	depNode(nodeDagPath.node(), &ms);
	if (!ms) {
		cerr << "CBrushToolCommand_Tentacle_Create::doRelease: MFnDependencyNode::MFnDependencyNode(MObject&): Failure: |" << ms << "|" << endl;
		return;
	}
	MPlug	plug_hitted = depNode.findPlug("drawTentacleRoot", true, &ms);
	ms = plug_hitted.setValue(false);
	MPlug	plug_tentacleDoAction = depNode.findPlug("tentacleDoAction", true, &ms);
	ms = plug_tentacleDoAction.setValue(BrushType == BT_TENTACLE_CREATE ? TentacleDo::Add : TentacleDo::Move);
}
#endif
#endif
