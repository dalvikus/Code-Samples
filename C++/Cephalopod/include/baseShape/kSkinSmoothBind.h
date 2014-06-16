#ifndef __SKIN_SMOOTH_BIND_H__
#define __SKIN_SMOOTH_BIND_H__
#if 0
{
	MMatrix	m;
#if 1
	if (__numJoints) {
		int	indexTestVertex = 0;
		assert(indexTestVertex < (int) __weights_array.length());
		const MDoubleArray&	weights = __weights_array(indexTestVertex);
		m = weights[0] * (__inverse_binding_matrix_array(0) * joint_world_matrix_array(0));
		for (unsigned i = 1; i < __numJoints; ++i) {
			m += weights[i] * (__inverse_binding_matrix_array(i) * joint_world_matrix_array(i));
		}
	}
	cout << "m(\\sum w_i B^-1_i W_i): " << m << endl;
#else
	m = weights_x[0] * (inverse_binding_matrix[0] * joint_world_matrix[0]);
	for (int i = 1; i < 5; ++i) {
		m += weights_x[i] * (inverse_binding_matrix[i] * joint_world_matrix[i]);
	}
#endif
#if 0
	cout << "MSpace::kObject in geometry: " << V_prime_kWorld * __externalInclusiveMatrix.inverse() * geometry_matrix * m.inverse() * geometry_matrix.inverse() << endl;
#endif
}
#endif
#include <baseShape/kPtrArray.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MMatrix.h>
#include <maya/MDoubleArray.h>
#include <maya/MStatus.h>
#include <maya/MObjectArray.h>
#include <map>
#include <maya/MPoint.h>
#include <maya/MIntArray.h>
#include <maya/MDagPath.h>
#include <vector>
class MFnMesh;
class MMatrix;
class MTime;
class MPlug;
class MFloatVectorArray;
class kSkinSmoothBind
{
#if 1
public:
	static MStatus	setSkinCluster(const MFnMesh& mesh, MFnSkinCluster& skinCluster);
	static MStatus	setSkinCluster(const MDagPath& dagPath, MFnSkinCluster& skinCluster);
public:
	static MStatus	setGeometryMatrix(const MFnSkinCluster& skinCluster, MMatrix& geometry_matrix);
public:
	static MStatus	setInverseBindingMatrixArray(const MFnSkinCluster& skinCluser, kPtrArray<MMatrix>& inverse_binding_matrix_array);
public:
	static MStatus	setWeightsArray(
		const MFnSkinCluster&		skinCluster,
		kPtrArray<MDoubleArray>&	weights_array,
		const MTime*			timePtr = NULL
	);
public:
	static MStatus	setJointHierachy(const MFnSkinCluster& skinCluster, MObjectArray& joints);
public:
	static MStatus	setJointWorldMatrixArray(
#if 1
		const MObjectArray&	joints_hierarchy,
#else
		const MFnSkinCluster&	skinCluster,
#endif
		kPtrArray<MMatrix>&	joint_world_matrix_array,
		const MTime*		timePtr = NULL
	);
#endif
#if 1
public:
	kSkinSmoothBind();
#if 1
private:
	bool			__hasSkinCluster;
	MFnSkinCluster		__skinCluster;
	MDagPath		__dagShape;
public:
	const MFnSkinCluster&	skinCluster() const	{return __skinCluster;}
	MFnSkinCluster&	skinCluster()			{return __skinCluster;}
public:
	bool	hasSkinCluster() const		{return __hasSkinCluster;}
private:
	MObjectArray	__joints;
public:
	const MObjectArray&	joints() const	{return __joints;}
	MObjectArray&		joints()	{return __joints;}
#if 1
private:
	MMatrix			__geometry_matrix;
public:
	const MMatrix&	geometry_matrix() const		{return __geometry_matrix;}
	MMatrix&	geometry_matrix()		{return __geometry_matrix;}
#endif
#if 1
private:
	kPtrArray<MMatrix>	__inverse_binding_matrix_array;
public:
	const kPtrArray<MMatrix>&	inverse_binding_matrix_array() const	{return __inverse_binding_matrix_array;}
	kPtrArray<MMatrix>&		inverse_binding_matrix_array()		{return __inverse_binding_matrix_array;}
#endif
#if 1
private:
	kPtrArray<MDoubleArray>	__weights_array;
public:
	const kPtrArray<MDoubleArray>&	weights_array() const	{return __weights_array;}
	kPtrArray<MDoubleArray>&	weights_array()		{return __weights_array;}
#endif
#if 1
private:
	kPtrArray<MMatrix>	__joint_world_matrix_array;
public:
	const kPtrArray<MMatrix>&	joint_world_matrix_array() const	{return __joint_world_matrix_array;}
	kPtrArray<MMatrix>&		joint_world_matrix_array()		{return __joint_world_matrix_array;}
#endif
#endif
#if 1
public:
	MStatus	setSkinCluster(const MFnMesh& mesh);
	MStatus	setSkinCluster(const MDagPath& dagPath);
	MStatus	setGeometryMatrix();
	MStatus	setInverseBindingMatrixArray();
	MStatus	setWeightsArray(const MTime* timePtr = NULL);
	MStatus	setJointHierachy();
	MStatus	setJointWorldMatrixArray(const MTime* timePtr = NULL);
#endif
#endif
#if 1
public:
	MStatus	getTransformationMatrix(
		int	indexVertex,
		const kPtrArray<MMatrix>&	joint_world_matrix_array,
		MMatrix&	matrix
	) const;
#if 1
private:
	typedef
	std::map<
		int,
		MIntArray
	>	POLYID_VERTEXIDLIST_DIC;
	typedef
	std::map<
		int,
		MPoint
	>	VERTEXID_POSITION_DIC;
#if 1
	struct StringRootInput
	{
		float	hitBary1, hitBary2;
		int	polyId;
	};
#endif
private:
	POLYID_VERTEXIDLIST_DIC	__polyId_vertexIdList_dic;
	VERTEXID_POSITION_DIC	__vertexId_position_dic;
	MIntArray	__tentacleId_vertexId_list;
#if 1
	std::vector<StringRootInput>	__tentacleId_stringRootInput_list;
#endif
#if 0
public:
	const POLYID_VERTEXIDLIST_DIC&	polyId_vertexIdList_dic() const	{return __polyId_vertexIdList_dic;}
	POLYID_VERTEXIDLIST_DIC&	polyId_vertexIdList_dic()	{return __polyId_vertexIdList_dic;}
	const VERTEXID_POSITION_DIC&	vertexId_position_dic() const	{return __vertexId_position_dic;}
	VERTEXID_POSITION_DIC&		vertexId_position_dic()		{return __vertexId_position_dic;}
	const MIntArray&	tentacleId_vertexId_list() const	{return __tentacleId_vertexId_list;}
	MIntArray&		tentacleId_vertexId_list()		{return __tentacleId_vertexId_list;}
#endif
public:
	MStatus	setCache(const MPlug& plug_tentacleArray);
#endif
public:
	MStatus	set(const MDagPath& dagShape, const MPlug& plug_tentacleArray);
public:
	MStatus	getPointAndNormal(
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
		const MTime*	timePtr = NULL
	) const;
	MStatus	getPoints(
		int	tentacleId,
		const MPlug&	plug_tentacle,
		float	segment_length,
		MVector&		root_wNormal,
		MFloatVectorArray&	string_wPoints,
		const MTime*	timePtr = NULL
	) const;
#endif
};
#endif
