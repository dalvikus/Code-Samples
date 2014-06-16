#include <baseShape/kSkinSmoothBind.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTime.h>
#include <baseShape/kUtil.h>
#include <maya/MAnimUtil.h>
#include <maya/MAnimControl.h>
#include <tentacleShape/kTentacleEnum.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MFnPointArrayData.h>
#include <tentacleShape/kTentacle.h>
#include <assert.h>
#include <maya/MQuaternion.h>
#if 1
#include <maya/MFnNumericData.h>
#endif
MStatus	kSkinSmoothBind::setSkinCluster(const MFnMesh& mesh, MFnSkinCluster& skinCluster)
{
	if (!mesh.dagPath().isValid()) {
		cerr << "WARNING: kSkinSmoothBind::setSkinCluster(): MFnMesh: NOT valid" << endl;
		return MStatus::kFailure;
	}
	MStatus	ms;
	MPlug	inMesh_plus = mesh.findPlug("inMesh", false, &ms);
	MPlugArray	skinCluster_plug_array;
	bool	bFound = inMesh_plus.connectedTo(skinCluster_plug_array, true, false, &ms);
	if (!bFound || skinCluster_plug_array.length() == 0) {
		return MStatus::kFailure;
	}
	if (!skinCluster_plug_array[0].node().hasFn(MFn::kSkinClusterFilter)) {
		cerr << "ERROR: kSkinSmoothBind::setSkinCluster(MFnMesh&): NO kSkinClusterFilter" << endl;
		return MStatus::kFailure;
	}
	ms = skinCluster.setObject(skinCluster_plug_array[0].node(&ms));
	if (!ms) {
		ms.perror("MFnSkinCluster::setObject()");
	}
	return ms;
}
MStatus	kSkinSmoothBind::setSkinCluster(const MDagPath& dagPath, MFnSkinCluster& skinCluster)
{
	MStatus	ms;
	if (!dagPath.isValid(&ms)) {
		cerr << "FATAL: kSkinSmoothBind::setSkinCluster(MDagPath&): MDagPath: NOT valid" << endl;
		return MStatus::kFailure;
	}
	MFnMesh	mesh(dagPath, &ms);
	return kSkinSmoothBind::setSkinCluster(mesh, skinCluster);
}
#if 1
MStatus	kSkinSmoothBind::setGeometryMatrix(const MFnSkinCluster& skinCluster, MMatrix& geometry_matrix)
{
	MStatus	ms;
	MPlug	geometry_matrix_plug = skinCluster.findPlug("geomMatrix", false, &ms);
	if (!ms) {cerr << ms << endl; return ms;}
	MObject	valData;
	ms = geometry_matrix_plug.getValue(valData);
	if (!ms) {cerr << ms << endl; return ms;}
	MFnMatrixData		matrixData;
	ms = matrixData.setObject(valData);
	if (!ms) {cerr << ms << endl; return ms;}
	geometry_matrix = matrixData.matrix(&ms);
	if (!ms) {cerr << ms << endl; return ms;}
	return MStatus::kSuccess;
}
#endif
#if 1
MStatus	kSkinSmoothBind::setInverseBindingMatrixArray(const MFnSkinCluster& skinCluster, kPtrArray<MMatrix>& inverse_binding_matrix_array)
{
	MStatus	ms;
	MPlug	inverse_binding_matrix_plug_array = skinCluster.findPlug("bindPreMatrix", false, &ms);
	if (!inverse_binding_matrix_plug_array.isArray()) {
		return MStatus::kFailure;
	}
	inverse_binding_matrix_array.clear();
	unsigned	numElements = inverse_binding_matrix_plug_array.numElements(&ms);
	for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
		MPlug	inverse_binding_matrix_plug = inverse_binding_matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
		if (!ms) {cerr << ms << endl; return ms;}
		MObject	valData;
		ms = inverse_binding_matrix_plug.getValue(valData);
		if (!ms) {cerr << ms << endl; return ms;}
		MFnMatrixData		matrixData;
		ms = matrixData.setObject(valData);
		if (!ms) {cerr << ms << endl; return ms;}
		MMatrix*	matrixPtr = new MMatrix;
		*matrixPtr = matrixData.matrix(&ms);
		if (!ms) {cerr << ms << endl; return ms;}
		inverse_binding_matrix_array.append(matrixPtr);
	}
	return MStatus::kSuccess;
}
#endif
#if 1
MStatus
kSkinSmoothBind::setWeightsArray(
	const MFnSkinCluster&		skinCluster,
	kPtrArray<MDoubleArray>&	weights_array,
	const MTime*			timePtr
)
{
	if (timePtr) {
		cerr << "WARNING: AS OF 5/4/2010 EST, STATIC WEIGHTS ASSUMED: ";
		cerr << "ALL ARE AT CURRENT TIME" << endl;
	}
	MStatus	ms;
#if 1
#if 0
	MDoubleArray	weights_x;
#endif
#endif
	MPlug	weight_list_compound_plug_array = skinCluster.findPlug("weightList", false, &ms);
	if (!weight_list_compound_plug_array.isArray(&ms) || !ms)
		return MStatus::kFailure;
	unsigned	numElements = weight_list_compound_plug_array.numElements(&ms);
	for (unsigned indexVertex = 0; indexVertex < numElements; ++indexVertex) {
		MDoubleArray*	weightsPtr = new MDoubleArray;
		weights_array.append(weightsPtr);
		MDoubleArray&	weights = *weightsPtr;
		MPlug	weights_compound_plug = weight_list_compound_plug_array.elementByPhysicalIndex(indexVertex, &ms);
		if (!ms) {cerr << ms << endl; return ms;}
		if (weights_compound_plug.isCompound(&ms) && ms) {
			unsigned	numChildren = weights_compound_plug.numChildren(&ms);
			if (!ms) {cerr << ms << endl; return ms;}
			if (numChildren != 1) {
				cerr << "FATAL: kSkinSmoothBind::setWeightsArray(): ONLY one children(.weights) expected" << endl;
				return MStatus::kFailure;
			}
			for (unsigned m = 0; m < numChildren; ++m) {
				MPlug	weight_plug_array = weights_compound_plug.child(m, &ms);
				if (!ms) {cerr << ms << endl; return ms;}
				if (weight_plug_array.isArray(&ms) && ms) {
					unsigned	numElements = weight_plug_array.numElements(&ms);
					for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
						MPlug	weight_plug = weight_plug_array.elementByPhysicalIndex(indexJoint, &ms);
						double	weight;
						ms = weight_plug.getValue(weight);
						weights.append(weight);
					}
				}
			}
		}
	}
	return MStatus::kSuccess;
}
#endif
#if 1
MStatus	kSkinSmoothBind::setJointHierachy(const MFnSkinCluster& skinCluster, MObjectArray& joints)
{
	MStatus	ms;
	MPlug	matrix_plug_array = skinCluster.findPlug("matrix", false, &ms);
	if (!matrix_plug_array.isArray()) {
		return MStatus::kFailure;
	}
	ms = joints.clear();
	unsigned	numElements = matrix_plug_array.numElements(&ms);
	for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
		MPlug	matrix_plug = matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
		MPlugArray	joint_plug_array;
		bool	bFound = matrix_plug.connectedTo(joint_plug_array, true, false, &ms);
		if (!bFound || joint_plug_array.length() == 0)
			return MStatus::kFailure;
		MObject	joint = joint_plug_array[0].node(&ms);
		ms = joints.append(joint);
		MFnDagNode	joint_dagNode(joint, &ms);
		MObject	joint_parent;
	{
		MFnDagNode	joint_parent_dagNode = joint_dagNode.parent(0, &ms); if (!ms) {cerr << ms << endl; break;}
		MDagPath	joint_parent_dagPath;
		ms = joint_parent_dagNode.getPath(joint_parent_dagPath); if (!ms) {cerr << ms << endl; break;}
		if (joint_parent_dagPath.fullPathName() != "")
			joint_parent = joint_parent_dagPath.node(&ms);
	}
		if (indexJoint == 0) {
			if (!joint_parent.isNull()) {
				cerr << "OUT OF ORDER" << endl;
				return MStatus::kFailure;
			}
		} else {
			if (joint_parent != joints[indexJoint - 1]) {
				cerr << "OUT OF ORDER" << endl;
				return MStatus::kFailure;
			}
		}
	}
#if 0
	for (unsigned i = 0; i < joints.length(); ++i) {
		MFnDagNode	dagNode(joints[i], &ms);
		if (i < joints.length() - 2) {
			cout << "joints[" << i << "]: isParentOf? " << (dagNode.isParentOf(joints[i + 2], &ms) ? "True" : "False") << endl;
		}
		if (i > 1) {
			cout << "joints[" << i << "]: isChildOf? " << (dagNode.isChildOf(joints[i - 2], &ms) ? "True" : "False") << endl;
		}
	}
#endif
	return MStatus::kSuccess;
}
MStatus
kSkinSmoothBind::setJointWorldMatrixArray(
#if 1
	const MObjectArray&	joints_hierarchy,
#else
	const MFnSkinCluster&	skinCluster,
#endif
	kPtrArray<MMatrix>&	joint_world_matrix_array,
	const MTime*		timePtr
)
{
	MStatus	ms;
	joint_world_matrix_array.clear();
#if 1
	if (!joints_hierarchy.length()) {
		cerr << "NO JOINTS" << endl;
		return MStatus::kFailure;
	}
#if	0 && defined(_DEBUG)
	if (timePtr)
		cout << "joint world matrices: at t = " << timePtr->value() << endl;
	else {
		MTime	time0 = MAnimControl::currentTime();
		ms = time0.setUnit(MTime::kSeconds);
		cout << "joint world matrices: at t = currentTime(" << time0.value() << ")" << endl;
	}
#endif
	for (unsigned n = 0; n < joints_hierarchy.length(); ++n) {
		MFnDagNode	joint_dagNode(joints_hierarchy[n], &ms);
		MDagPath	joint_dagPath;
		ms = joint_dagNode.getPath(joint_dagPath);
		MMatrix*	joint_world_matrix_ptr = new MMatrix;
		if (timePtr) {
			MMatrix		matrix;
			ms = kUtil::getWorldMatrixAt(joint_dagPath, *timePtr, matrix);
			*joint_world_matrix_ptr = n ? (matrix * joint_world_matrix_array(n - 1)) : matrix;
		} else {
			*joint_world_matrix_ptr = joint_dagPath.inclusiveMatrix(&ms);
		}
#if	0 && defined(_DEBUG)
		cout << "joint[" << n << "]: " << *joint_world_matrix_ptr << endl;
#endif
		joint_world_matrix_array.append(joint_world_matrix_ptr);
	}
#else
	MPlug	matrix_plug_array = skinCluster.findPlug("matrix", false, &ms);
	if (!matrix_plug_array.isArray()) {
		return MStatus::kFailure;
	}
	unsigned	numElements = matrix_plug_array.numElements(&ms);
	cout << "# of joint(s) = " << numElements << endl;
#if 1
if (timePtr == NULL) {
	for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
		MPlug	matrix_plug = matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
		if (!ms) {cerr << ms << endl; return ms;}
		MObject	valData;
		ms = matrix_plug.getValue(valData);
		if (!ms) {cerr << ms << endl; return ms;}
		MFnMatrixData		matrixData;
		ms = matrixData.setObject(valData);
		if (!ms) {cerr << ms << endl; return ms;}
		MMatrix*	joint_world_matrix_ptr = new MMatrix;
		*joint_world_matrix_ptr = matrixData.matrix(&ms);
		joint_world_matrix_array.append(joint_world_matrix_ptr);
		if (!ms) {cerr << ms << endl; return ms;}
	}
	return MStatus::kSuccess;
}
#endif
#if 1
	for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
		MPlug	matrix_plug = matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
{
		MPlugArray	joint_plug_array;
		bool	bFound = matrix_plug.connectedTo(joint_plug_array, true, false, &ms);
		if (bFound && joint_plug_array.length()) {
			MObject	joint = joint_plug_array[0].node(&ms);
			MFnDagNode	joint_dagNode(joint, &ms);
			MDagPath	joint_dagPath;
			ms = joint_dagNode.getPath(joint_dagPath);
#if 1
	MFnDagNode	joint_parent_dagNode = joint_dagNode.parent(0, &ms); if (!ms) {cerr << ms << endl; break;}
	MDagPath	joint_parent_dagPath;
	ms = joint_parent_dagNode.getPath(joint_parent_dagPath); if (!ms) {cerr << ms << endl; break;}
#endif
#if 0
{
	MTime	t;
	for (int i = 0; i < 1; ++i) {
		t.setValue(1 + i);
		cout << "[#" << (1 + i) << "]: t = " << t.value() << endl;
		MMatrix	m;
		ms = kUtil::getWorldMatrixAt(joint_dagPath, t, m);
		if (!ms) {cout << ms << endl;}
		cout << m << endl;
	}
}
#endif
			MMatrix*	joint_world_matrix_ptr = new MMatrix;
			*joint_world_matrix_ptr = joint_dagPath.inclusiveMatrix(&ms);
			joint_world_matrix_array.append(joint_world_matrix_ptr);
		}
}
	}
#endif
#endif
#if 0
	if (timePtr)
		cout << "joint's world matrix list for given time, t = " << timePtr->value() << endl;
	else
		cout << "joint's world matrix list for current time(t = " << MAnimControl::currentTime().value() << ")" << endl;
	for (unsigned n = 0; n < joint_world_matrix_array.length(); ++n) {
		cout << "W[" << n << "]: " << joint_world_matrix_array(n) << endl;
	}
#endif
	return MStatus::kSuccess;
}
#endif
#if 1
kSkinSmoothBind::kSkinSmoothBind()
{
	__hasSkinCluster = false;
	__inverse_binding_matrix_array.clear();
	__weights_array.clear();
	__joint_world_matrix_array.clear();
}
MStatus	kSkinSmoothBind::setSkinCluster(const MFnMesh& mesh)
{
	MStatus	ms = kSkinSmoothBind::setSkinCluster(mesh, __skinCluster);
	__hasSkinCluster = ms == MStatus::kSuccess;
	__dagShape = mesh.dagPath(&ms);
	return ms;
}
MStatus	kSkinSmoothBind::setSkinCluster(const MDagPath& dagPath)
{
	MStatus	ms = kSkinSmoothBind::setSkinCluster(dagPath, __skinCluster);
	__hasSkinCluster = ms == MStatus::kSuccess;
	__dagShape = dagPath;
	return ms;
}
MStatus	kSkinSmoothBind::setGeometryMatrix()
{
	if (!__hasSkinCluster) {
		cerr << "ERROR: kSkinSmoothBind::setGeometryMatrix(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	return kSkinSmoothBind::setGeometryMatrix(__skinCluster, __geometry_matrix);
}
MStatus	kSkinSmoothBind::setInverseBindingMatrixArray()
{
	if (!__hasSkinCluster) {
		cerr << "ERROR: kSkinSmoothBind::setInverseBindingMatrixArray(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	return kSkinSmoothBind::setInverseBindingMatrixArray(__skinCluster, __inverse_binding_matrix_array);
}
MStatus	kSkinSmoothBind::setWeightsArray(const MTime* timePtr)
{
	if (!__hasSkinCluster) {
		cerr << "ERROR: kSkinSmoothBind::setWeightsArray(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	return kSkinSmoothBind::setWeightsArray(__skinCluster, __weights_array, timePtr);
}
MStatus	kSkinSmoothBind::setJointHierachy()
{
	if (!__hasSkinCluster) {
		cerr << "ERROR: kSkinSmoothBind::setJointHierachy(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	return kSkinSmoothBind::setJointHierachy(__skinCluster, __joints);
}
MStatus	kSkinSmoothBind::setJointWorldMatrixArray(const MTime* timePtr)
{
	if (!__hasSkinCluster) {
		cerr << "ERROR: kSkinSmoothBind::setJointWorldMatrixArray(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	return kSkinSmoothBind::setJointWorldMatrixArray(
#if 1
		__joints,
#else
		__skinCluster,
#endif
		__joint_world_matrix_array, timePtr
	);
}
#endif
#if 1
MStatus
kSkinSmoothBind::getTransformationMatrix(
	int	indexVertex,
	const kPtrArray<MMatrix>&	joint_world_matrix_array,
	MMatrix&	matrix
) const
{
	if (indexVertex < 0 || indexVertex >= (int) __weights_array.length()) {
		cerr << "ERROR: kSkinSmoothBind::getTransformationMatrix(indexVertex = " << indexVertex << ")";
		if (__weights_array.length() == 0) {
			cerr << ": No weights_array: Call kSkinSmoothBind::setWeightsArray first" << endl;
		} else {
			cerr << ": INVALID (# of vertices = " << __weights_array.length() << ")" << endl;
		}
		return MStatus::kFailure;
	}
	const MDoubleArray&	weights = __weights_array(indexVertex);
	if (__inverse_binding_matrix_array.length() == 0) {
		cerr << "ERROR: kSkinSmoothBind::getTransformationMatrix(indexVertex = " << indexVertex << ")";
		cerr << ": No __inverse_binding_matrix_array: Call kSkinSmoothBind::setInverseBindingMatrixArray first" << endl;
		return MStatus::kFailure;
	}
	if (joint_world_matrix_array.length() == 0) {
		cerr << "ERROR: kSkinSmoothBind::getTransformationMatrix(indexVertex = " << indexVertex << ")";
		cerr << ": No joint_world_matrix_array: Call kSkinSmoothBind::setJointWorldMatrixArray first" << endl;
		return MStatus::kFailure;
	}
	if (
		weights.length() != __inverse_binding_matrix_array.length() ||
		weights.length() != joint_world_matrix_array.length()
	) {
		cerr << "ERROR: kSkinSmoothBind::getTransformationMatrix(indexVertex = " << indexVertex << ")";
		cerr << ": # of joints MISMATCH";
		cerr << ": # of weights = " << weights.length();
		cerr << ", # of __inverse_binding_matrix = " << __inverse_binding_matrix_array.length();
		cerr << ", # of joint_world_matrix = " << joint_world_matrix_array.length() << endl;
		return MStatus::kFailure;
	}
	matrix = weights[0] * (__inverse_binding_matrix_array(0) * joint_world_matrix_array(0));
	for (unsigned i = 1; i < weights.length(); ++i) {
		matrix += weights[i] * (__inverse_binding_matrix_array(i) * joint_world_matrix_array(i));
	}
	return MStatus::kSuccess;
}
MStatus	kSkinSmoothBind::setCache(const MPlug& plug_tentacleArray)
{
	MStatus	ms;
	__polyId_vertexIdList_dic.clear();
	__vertexId_position_dic.clear();
	__tentacleId_vertexId_list.clear();
	__tentacleId_stringRootInput_list.clear();
	if (!__hasSkinCluster) {
		cerr << "FATAL: kSkinSmoothBind::setCache(): NO skinCluster" << endl;
		return MStatus::kFailure;
	}
	if (!__dagShape.isValid(&ms)) {
		cerr << "FATAL: kSkinSmoothBind::setCache(): MDagPath: NOT valid" << endl;
		return MStatus::kFailure;
	}
	MFnMesh	mesh(__dagShape, &ms);
	if (!ms) {
		ms.perror("ERROR: kSkinSmoothBind::setCache(): MFnMesh::MFnMesh(MDagPath)");
		return ms;
	}
	MMatrix	inclusiveMatrix = __dagShape.inclusiveMatrix(&ms);
	if (!ms) {
		ms.perror("FATAL: kSkinSmoothBind::setCache(): MDagPath::inclusiveMatrix()");
		return ms;
	}
	MMatrix	inverseInclusiveMatrix = inclusiveMatrix.inverse();
	MItMeshPolygon	polyIter(__dagShape, MObject::kNullObj, &ms);
	if (!ms) {
		ms.perror("ERROR: kSkinSmoothBind::setCache(): MItMeshPolygon(MDagPath&)");
		return ms;
	}
	kPtrArray<MMatrix>	joint_world_matrix_array;
	ms = setJointWorldMatrixArray(__joints, joint_world_matrix_array, NULL );
	for (unsigned tentacleId = 0; tentacleId < plug_tentacleArray.numElements(); ++tentacleId) {
		MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(tentacleId, &ms);
		StringRootInput	sri;
		MPlug	plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
		MPlug	plug_root_polyId = plug_root.child(TENTACLE_ROOT_POLY_ID, &ms);
		int&	polyId = sri.polyId;
		ms = plug_root_polyId.getValue(polyId);
#if 1
		MPlug	plug_root_hitVertexList = plug_root.child(TENTACLE_ROOT_HIT_VERTEX_LIST, &ms);
		MObject	data;
		ms = plug_root_hitVertexList.getValue(data);
		if (!ms)
			return ms;
		MFnNumericData	val(data, &ms);
		if (!ms)
			return ms;
		int	hitVertexId0, hitVertexId1, hitVertexId2;
		ms = val.getData(hitVertexId0, hitVertexId1, hitVertexId2);
		cout << "hitVertexList: [" << hitVertexId0 << ", " << hitVertexId1 << ", " << hitVertexId2 << "]" << endl;
#else
		MPlug	plug_root_hitTriangleId = plug_root.child(TENTACLE_ROOT_HIT_TRIANGLE_ID, &ms);
		int	hitTriangleId = -1;
		ms = plug_root_hitTriangleId.getValue(hitTriangleId);
#endif
		MPlug	plug_root_hit_bary1 = plug_root.child(TENTACLE_ROOT_HIT_BARY1, &ms);
		float&	hitBary1 = sri.hitBary1;
		ms = plug_root_hit_bary1.getValue(hitBary1);
		MPlug	plug_root_hit_bary2 = plug_root.child(TENTACLE_ROOT_HIT_BARY2, &ms);
		float&	hitBary2 = sri.hitBary2;
		ms = plug_root_hit_bary2.getValue(hitBary2);
		__tentacleId_stringRootInput_list.push_back(sri);
		int	prevIndex;
		ms = polyIter.setIndex(polyId, prevIndex);
		if (!ms) {
			ms.perror("ERROR: kSkinSmoothBind::setCache(): MItMeshPolygon::setIndex()");
			return ms;
		}
		POLYID_VERTEXIDLIST_DIC::const_iterator	it = __polyId_vertexIdList_dic.find(polyId);
		if (it == __polyId_vertexIdList_dic.end()) {
			MIntArray	vertexIdList;
			int	numTriangles = 0;
			ms = polyIter.numTriangles(numTriangles);
			for (int triangleId = 0; triangleId < numTriangles; ++triangleId) {
#if 0
				int	vertexList[3];
				ms = mesh.getPolygonTriangleVertices(polyId, triangleId, vertexList);
				ms = vertexIdList.append(vertexList[0]);
				ms = vertexIdList.append(vertexList[1]);
				ms = vertexIdList.append(vertexList[2]);
#endif
#if 1
		MPointArray	wPoints;
		MIntArray	vertexList;
		ms = polyIter.getTriangle(triangleId, wPoints, vertexList, MSpace::kWorld);
		for (unsigned i = 0; i < vertexList.length(); ++i) {
			int	vertexIndex = vertexList[i];
			ms = vertexIdList.append(vertexIndex);
			MMatrix	m;
			ms = getTransformationMatrix(vertexIndex, joint_world_matrix_array, m);
			if (!ms) {
				ms.perror("FATAL: kSkinSmoothBind::setCache(): kSkinSmoothBind::getTransformationMatrix()");
				return ms;
			}
			MPoint	point = wPoints[i] * inverseInclusiveMatrix * __geometry_matrix * m.inverse() * __geometry_matrix.inverse();
			VERTEXID_POSITION_DIC::const_iterator	it = __vertexId_position_dic.find(vertexIndex);
			if (it == __vertexId_position_dic.end()) {
				__vertexId_position_dic[vertexIndex] = point;
			}
		}
#endif
			}
			__polyId_vertexIdList_dic[polyId] = vertexIdList;
		}
#if 1
		ms = __tentacleId_vertexId_list.append(hitVertexId0);
		ms = __tentacleId_vertexId_list.append(hitVertexId1);
		ms = __tentacleId_vertexId_list.append(hitVertexId2);
#else
		MPointArray	wPoints;
		MIntArray	vertexList;
		ms = polyIter.getTriangle(hitTriangleId, wPoints, vertexList, MSpace::kWorld);
		for (unsigned i = 0; i < vertexList.length(); ++i) {
			int	vertexIndex = vertexList[i];
			ms = __tentacleId_vertexId_list.append(vertexIndex);
#if 0
			MMatrix	m;
			ms = getTransformationMatrix(vertexIndex, joint_world_matrix_array, m);
			if (!ms) {
				ms.perror("FATAL: kSkinSmoothBind::setCache(): kSkinSmoothBind::getTransformationMatrix()");
				return ms;
			}
			MPoint	point = wPoints[i] * inverseInclusiveMatrix * __geometry_matrix * m.inverse() * __geometry_matrix.inverse();
			VERTEXID_POSITION_DIC::const_iterator	it = __vertexId_position_dic.find(vertexIndex);
			if (it == __vertexId_position_dic.end()) {
				__vertexId_position_dic[vertexIndex] = point;
			}
#endif
		}
#endif
	}
#if	0 && defined(_DEBUG)
	for (unsigned i = 0; i < __tentacleId_vertexId_list.length(); i += 3) {
		cout << "__tentacleId_vertexId_list[" << i / 3 << "]: [";
		cout << __tentacleId_vertexId_list[i + 0] << ", ";
		cout << __tentacleId_vertexId_list[i + 1] << ", ";
		cout << __tentacleId_vertexId_list[i + 2] << "]" << endl;
	}
	for (POLYID_VERTEXIDLIST_DIC::const_iterator it = __polyId_vertexIdList_dic.begin(); it != __polyId_vertexIdList_dic.end(); ++it) {
		cout << "__polyId_vertexIdList_dic[" << it->first << "]: " << it->second << endl;
	}
	for (VERTEXID_POSITION_DIC::const_iterator it = __vertexId_position_dic.begin(); it != __vertexId_position_dic.end(); ++it) {
		cout << "__vertexId_position_dic[" << it->first << "]: " << it->second << endl;
	}
#endif
	return MStatus::kSuccess;
}
MStatus	kSkinSmoothBind::set(const MDagPath& dagShape, const MPlug& plug_tentacleArray)
{
	MStatus	ms;
	if (!dagShape.isValid(&ms)) {
		cerr << "FATAL: kSkinSmoothBind::set: dagShape: NOT valid" << endl;
		return MStatus::kFailure;
	}
	ms = setSkinCluster(dagShape);
	ms = setGeometryMatrix();
	ms = setInverseBindingMatrixArray();
	ms = setWeightsArray();
	ms = setJointHierachy();
#if 0
	ms = __skinSmoothBind.setJointWorldMatrixArray();
	MTime	t;
	for (int i = 0; i < 24; ++i) {
		t.setValue(1 + i);
		cout << "[#" << (1 + i) << "]: t = " << t.value() << endl;
		ms = __skinSmoothBind.setJointWorldMatrixArray(&t);
	}
#endif
	ms = setCache(plug_tentacleArray);
	return MStatus::kSuccess;
}
MStatus	kSkinSmoothBind::getPointAndNormal(
	int	tentacleId,
#if 1
	MPoint&		root_wPoint,
	MVector&	root_wNormal,
#else
	float	hitBary1,
	float	hitBary2,
	MPoint&		root_wPoint,
	int	polyId,
	MVector&	root_wNormal,
#endif
	const MTime*	timePtr
) const
{
	MStatus	ms;
	if (!__dagShape.isValid(&ms)) {
		cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(): MDagPath: NOT valid" << endl;
		return MStatus::kFailure;
	}
	MMatrix	inclusiveMatrix;
	if (timePtr) {
		ms = kUtil::getWorldMatrixAt(__dagShape, *timePtr, inclusiveMatrix);
	} else {
		inclusiveMatrix = __dagShape.inclusiveMatrix(&ms);
	}
	if (!ms) {
		ms.perror("FATAL: kSkinSmoothBind::getPointAndNormal(): MDagPath::inclusiveMatrix()");
		return ms;
	}
#if 1
	if (tentacleId < 0 || tentacleId >= (int) __tentacleId_stringRootInput_list.size()) {
		cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << "): OUT OF RANGE";
		cerr << " (# of tentacles = " << __tentacleId_stringRootInput_list.size() << ")" << endl;
		return MStatus::kFailure;
	}
	StringRootInput	sri = __tentacleId_stringRootInput_list.at(tentacleId);
	int&	polyId = sri.polyId;
	float&	hitBary1 = sri.hitBary1;
	float&	hitBary2 = sri.hitBary2;
#endif
	if ((int) __tentacleId_vertexId_list.length() < 3 * (tentacleId + 1)) {
		cerr << "ERROR: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << "): OUT OF RANGE";
		cerr << " (# of tentacles = " << __tentacleId_vertexId_list.length() / 3 << ")" << endl;
		return MStatus::kFailure;
	}
	kPtrArray<MMatrix>	joint_world_matrix_array;
	ms = setJointWorldMatrixArray(__joints, joint_world_matrix_array, timePtr);
#if 1
	int	vertexId[3];
	for (int i = 0; i < 3; ++i)
		vertexId[i] = __tentacleId_vertexId_list[3 * tentacleId + i];
	MPoint	vertexPosition0[3];
	for (int i = 0; i < 3; ++i) {
		VERTEXID_POSITION_DIC::const_iterator	it = __vertexId_position_dic.find(vertexId[i]);
		if (it == __vertexId_position_dic.end()) {
			cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << ")";
			cerr << ": NO vertex position in MSpace::kObject before skinning for ";
			cerr << (i == 0 ? "first" : (i == 1 ? "second" : "third")) << " vertex(id = " << vertexId[i] << ")" << endl;
			return MStatus::kFailure;
		}
		vertexPosition0[i] = it->second;
	}
	MMatrix	m[3];
	for (int i = 0; i < 3; ++i) {
		ms = getTransformationMatrix(vertexId[i], joint_world_matrix_array, m[i]);
	}
	MPoint	vertexPosition[3];
	for (int i = 0; i < 3; ++i) {
		vertexPosition[i] = vertexPosition0[i] * __geometry_matrix * m[i] * __geometry_matrix.inverse() * inclusiveMatrix;
	}
#endif
	MVector	v0(vertexPosition[0]), v1(vertexPosition[1]), v2(vertexPosition[2]);
	MVector	v = hitBary1 * v0 + hitBary2 * v1 + (1 - hitBary1 - hitBary2) * v2;
	root_wPoint.x = v.x; root_wPoint.y = v.y; root_wPoint.z = v.z;
#if	0 && defined(_DEBUG)
	if (timePtr) {
		cout << "t = " << timePtr->value() << endl;
	} else {
		MTime	t = MAnimControl::currentTime();
		double	v = t.value();
		t.setUnit(MTime::kSeconds);
		cout << "t = currentTime(" << v << ", " << t.value() << ")" << endl;
	}
	cout << "root_wPoint: " << root_wPoint << endl;
#endif
	POLYID_VERTEXIDLIST_DIC::const_iterator	it = __polyId_vertexIdList_dic.find(polyId);
	if (it == __polyId_vertexIdList_dic.end()) {
		cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << ")";
		cerr << ": NO vertexIdList for polyId = " << polyId << endl;
		return MStatus::kFailure;
	}
	MIntArray	vertexIdList = it->second;
	if (vertexIdList.length() == 0 || vertexIdList.length() % 3 != 0) {
		cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << ")";
		cerr << ": INVALID vertexIdList" << endl;
		return MStatus::kFailure;
	}
	root_wNormal = MVector::zero;
	for (int triangleId = 0; triangleId < (int) vertexIdList.length() / 3; ++triangleId) {
#if 1
		int	vertexId[3];
		for (int k = 0; k < 3; ++k)
			vertexId[k] = vertexIdList[3 * triangleId + k];
		MPoint	vertexPosition0[3];
		for (int i = 0; i < 3; ++i) {
			VERTEXID_POSITION_DIC::const_iterator	it = __vertexId_position_dic.find(vertexId[i]);
			if (it == __vertexId_position_dic.end()) {
				cerr << "FATAL: kSkinSmoothBind::getPointAndNormal(tentacleId = " << tentacleId << ")";
				cerr << ": NO vertex position in MSpace::kObject before skinning for ";
				cerr << (i == 0 ? "first" : (i == 1 ? "second" : "third")) << " vertex(id = " << vertexId[i] << ")" << endl;
				return MStatus::kFailure;
			}
			vertexPosition0[i] = it->second;
		}
		MMatrix	m[3];
		for (int i = 0; i < 3; ++i) {
			ms = getTransformationMatrix(vertexId[i], joint_world_matrix_array, m[i]);
		}
		MPoint	vertexPosition[3];
		for (int i = 0; i < 3; ++i) {
			vertexPosition[i] = vertexPosition0[i] * __geometry_matrix * m[i] * __geometry_matrix.inverse() * inclusiveMatrix;
		}
#endif
		MVector	v1 = vertexPosition[1] - vertexPosition[0]; v1.normalize();
		MVector	v2 = vertexPosition[2] - vertexPosition[0]; v2.normalize();
		MVector	v = v1 ^ v2; v.normalize();
		if (triangleId && root_wNormal * v < 0) {
			cerr << "WARN: normal[" << triangleId << "]: reversed" << endl;
			v = -v;
		}
		root_wNormal += v;
	}
	root_wNormal.normalize();
	return MStatus::kSuccess;
}
MStatus	kSkinSmoothBind::getPoints(
	int	tentacleId,
	const MPlug&	plug_tentacle,
	float	segment_length,
	MVector&		root_wNormal,
	MFloatVectorArray&	string_wPoints,
	const MTime*	timePtr
) const
{
	MStatus	ms;
#if 0
	MPlug	plug_root = plug_tentacle.child(TENTACLE_ARRAY_ROOT, &ms);
	MPlug	plug_root_polyId = plug_root.child(TENTACLE_ROOT_POLY_ID, &ms);
	int	polyId = -1;
	ms = plug_root_polyId.getValue(polyId);
	MPlug	plug_root_hit_bary1 = plug_root.child(TENTACLE_ROOT_HIT_BARY1, &ms);
	float	hitBary1;
	ms = plug_root_hit_bary1.getValue(hitBary1);
	MPlug	plug_root_hit_bary2 = plug_root.child(TENTACLE_ROOT_HIT_BARY2, &ms);
	float	hitBary2;
	ms = plug_root_hit_bary2.getValue(hitBary2);
#endif
	MPoint	root_wPoint;
#if 1
	ms = getPointAndNormal(tentacleId, root_wPoint, root_wNormal, timePtr);
#else
	ms = getPointAndNormal(tentacleId, hitBary1, hitBary2, root_wPoint, polyId, root_wNormal);
#endif
	if (!ms) {
		ms.perror("ERROR: kSkinSmoothBind::getPoints: kSkinSmoothBind::getPointAndNormal()");
		return ms;
	}
	MPlug	plug_quaternion = plug_tentacle.child(TENTACLE_ARRAY_QUATERNION, &ms);
	MObject	valData;
	ms = plug_quaternion.getValue(valData);
	MFnPointArrayData	pointArrayData;
	ms = pointArrayData.setObject(valData);
	MPointArray	pointArray = pointArrayData.array(&ms);
	assert(pointArray.length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
	ms = string_wPoints.clear();
	MPoint	wP = root_wPoint;
	ms = string_wPoints.append(wP);
	MVector	wN = root_wNormal;
	for (unsigned i = 0; i < pointArray.length(); ++i) {
		MPoint	p = pointArray[i];
		MQuaternion	q; q.x = p.x; q.y = p.y; q.z = p.z; q.w = p.w;
		MVector	v = wN.rotateBy(q);
		wN = v;
		v *= segment_length;
		wP += v;
		ms = string_wPoints.append(wP);
	}
	return MStatus::kSuccess;
}
#endif
