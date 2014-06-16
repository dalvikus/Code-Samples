#include <assert.h>
#include <maya/MGlobal.h>
#if 3
#include <baseShape/kUtil.h>
#include <maya/MPlugArray.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#endif
#if 2
#include <maya/MQuaternion.h>
#endif
#if 0
#include <maya/MFnMatrixData.h>
#include <maya/MFnTypedAttribute.h>
#endif
#if 0
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#endif
#if 1
#endif
#include <math.h>
#include <maya/MIOStream.h>
#include <baseShape/baseShapeCreator.h>
#include <baseShape/baseShapeData.h>
#include "api_macros.h"
#include <maya/MFnMesh.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923
#endif
MObject baseShapeCreator::size;
MObject baseShapeCreator::shapeType;
MObject baseShapeCreator::inputMesh;
MObject baseShapeCreator::outputSurface;
#if 0
MObject	baseShapeCreator::__externalWorldMatrix;
#endif
MTypeId	baseShapeCreator::id(0x80089 + 1);
baseShapeCreator::baseShapeCreator()
{
#ifdef baseShapeCreator_DEBUG
	cout << "baseShapeCreator::baseShapeCreator()" << endl;
#endif
}
baseShapeCreator::~baseShapeCreator()
{
#ifdef baseShapeCreator_DEBUG
	cout << "baseShapeCreator::~baseShapeCreator()" << endl;
#endif
}
#if 1
#endif
MStatus
baseShapeCreator::__compute(
	const MPlug&	plug,
	MDataBlock&	datablock
)
{
	if (plug != outputSurface) {
		return MS::kUnknownParameter;
	}
#ifdef baseShapeCreator_DEBUG
	cout << "baseShapeCreator::compute" << endl;
#endif
	MStatus	stat;
	MFnPluginData	fnDataCreator;
	MTypeId		tmpid(baseShapeData::id);
#ifdef baseShapeData_DEBUG
cout << "<MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
#if 1
#endif
	MObject	newDataObject = fnDataCreator.create(tmpid, &stat);
	MCHECKERROR(stat, "compute : error creating baseShapeData")
#ifdef baseShapeData_DEBUG
cout << "</MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
	baseShapeData*	newData = (baseShapeData*) fnDataCreator.data(&stat);
	MCHECKERROR(stat, "compute : error gettin at proxy baseShapeData object")
	baseShapeGeometry*	geometryPtr = newData->fGeometry;
	bool	hasHistory = computeInputMesh(
		plug,
		datablock,
		geometryPtr->vertices,
		geometryPtr->face_counts,
		geometryPtr->face_connects,
		geometryPtr->normals
	);
	if (!hasHistory) {
		MDataHandle	sizeHandle = datablock.inputValue(size);
		double	shape_size = sizeHandle.asDouble();
		MDataHandle	typeHandle = datablock.inputValue(shapeType);
		short	shape_type = typeHandle.asShort();
		switch (shape_type) {
		case 0:
			buildCube(
				shape_size,
				geometryPtr->vertices,
				geometryPtr->face_counts,
				geometryPtr->face_connects,
				geometryPtr->normals
			);
			break;
		case 1:
			buildSphere(
				shape_size, 32,
				geometryPtr->vertices,
				geometryPtr->face_counts,
				geometryPtr->face_connects,
				geometryPtr->normals
			);
			break;
		}
	}
#if 2
	geometryPtr->wakes.clear();
	for (unsigned i = 0; i < geometryPtr->vertices.length(); ++i) {
		MQuaternion	q(MVector(0, 0, 1), geometryPtr->normals[i]);
		double	radius = 1.;
		double	factor = 2. * M_PI / WAKE_POINTS;
		for (int n = 0; n < WAKE_POINTS; ++n) {
			double	angle = factor * n;
			MVector	v(radius * cos(angle), radius * sin(angle), 0.);
			MPoint	p(v.rotateBy(q));
			geometryPtr->wakes.append(p + geometryPtr->vertices[i]);
		}
	}
#endif
	geometryPtr->faceCount = geometryPtr->face_counts.length();
	MDataHandle	outHandle = datablock.outputValue(outputSurface);
#if 1
#endif
#ifdef baseShapeData_DEBUG
cout << "<MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	outHandle.set(newData);
#ifdef baseShapeData_DEBUG
cout << "</MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	datablock.setClean(plug);
	return MS::kSuccess;
}
MStatus
baseShapeCreator::compute(
	const MPlug&	plug,
	MDataBlock&	datablock
)
{
	MStatus	ms = __compute(plug, datablock);
	return ms;
}
#if 0
MStatus	baseShapeCreator::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
{
	if (asSrc == false && plug == inputMesh) {
		MStatus	ms;
		MObject	otherNodeShape = otherPlug.node(&ms); if (!ms) cerr << ms << endl;
		MFnDagNode	otherDagNodeShape(otherNodeShape, &ms); if (!ms) cerr << ms << endl;
		MDagPath	otherDagPathShape;
		ms = otherDagNodeShape.getPath(otherDagPathShape); if (!ms) cerr << ms << endl;
		MMatrix	m = otherDagPathShape.inclusiveMatrix(&ms);
		MFnMatrixData	matrixData;
		MObject	matrixObj = matrixData.create(m, &ms);
		MPlug	plug_externalWorldMatrix(thisMObject(), __externalWorldMatrix);
		ms = plug_externalWorldMatrix.setValue(matrixObj);
	}
	return MStatus::kSuccess;
}
MStatus	baseShapeCreator::connectionBroken(const MPlug& plug, const MPlug& , bool asSrc)
{
	if (asSrc == false && plug == inputMesh) {
		MStatus	ms;
		MMatrix	m;
		MFnMatrixData	matrixData;
		MObject	matrixObj = matrixData.create(m, &ms);
		MPlug	plug_externalWorldMatrix(thisMObject(), __externalWorldMatrix);
		ms = plug_externalWorldMatrix.setValue(matrixObj);
	}
	return MStatus::kSuccess;
}
#endif
void	baseShapeCreator::buildCube(
	double	cube_size,
	MPointArray&	pa,
	MIntArray&	faceCounts,
	MIntArray&	faceConnects,
	MVectorArray&	normals
)
{
	const int	num_faces		= 6;
	const int	num_face_connects	= 24;
	const double	normal_value		= 0.5775;
	pa.clear(); faceCounts.clear(); faceConnects.clear();
	pa.append( MPoint( -cube_size, -cube_size, -cube_size ) );
	pa.append( MPoint(  cube_size, -cube_size, -cube_size ) );
	pa.append( MPoint(  cube_size, -cube_size,  cube_size ) );
	pa.append( MPoint( -cube_size, -cube_size,  cube_size ) );
	pa.append( MPoint( -cube_size,  cube_size, -cube_size ) );
	pa.append( MPoint( -cube_size,  cube_size,  cube_size ) );
	pa.append( MPoint(  cube_size,  cube_size,  cube_size ) );
	pa.append( MPoint(  cube_size,  cube_size, -cube_size ) );
	normals.append( MVector( -normal_value, -normal_value, -normal_value ) );
	normals.append( MVector(  normal_value, -normal_value, -normal_value ) );
	normals.append( MVector(  normal_value, -normal_value,  normal_value ) );
	normals.append( MVector( -normal_value, -normal_value,  normal_value ) );
	normals.append( MVector( -normal_value,  normal_value, -normal_value ) );
	normals.append( MVector( -normal_value,  normal_value,  normal_value ) );
	normals.append( MVector(  normal_value,  normal_value,  normal_value ) );
	normals.append( MVector(  normal_value,  normal_value, -normal_value ) );
	int face_counts[num_faces] = { 4, 4, 4, 4, 4, 4 };
	int i;
	for ( i=0; i<num_faces; i++ )
	{
		faceCounts.append( face_counts[i] );
	}
	int face_connects[ num_face_connects ] = {	0, 1, 2, 3,
												4, 5, 6, 7,
												3, 2, 6, 5,
												0, 3, 5, 4,
												0, 4, 7, 1,
												1, 7, 6, 2	};
	for ( i=0; i<num_face_connects; i++ )
	{
		faceConnects.append( face_connects[i] );
	}
}
void	baseShapeCreator::buildSphere(
	double	rad,
	int	div,
	MPointArray&	vertices,
	MIntArray&	counts,
	MIntArray&	connects,
	MVectorArray&	normals
)
{
	double u = -M_PI_2;
	double v = -M_PI;
	double u_delta = M_PI / ((double)div);
	double v_delta = 2 * M_PI / ((double)div);
	MPoint topPole( 0.0, rad, 0.0 );
	MPoint botPole( 0.0, -rad, 0.0 );
	vertices.append( botPole );
	normals.append( botPole-MPoint::origin );
	int i;
	for ( i=0; i<(div-1); i++ )
	{
		u += u_delta;
		v = -M_PI;
		for ( int j=0; j<div; j++ )
		{
			double x = rad * cos(u) * cos(v);
			double y = rad * sin(u);
			double z = rad * cos(u) * sin(v) ;
			MPoint pnt( x, y, z );
			vertices.append( pnt );
			normals.append( pnt-MPoint::origin );
			v += v_delta;
		}
	}
	vertices.append( topPole );
	normals.append( topPole-MPoint::origin );
	int vid = 1;
	int numV = 0;
	for ( i=0; i<div; i++ )
	{
		for ( int j=0; j<div; j++ )
		{
			if ( i==0 ) {
				counts.append( 3 );
				connects.append( 0 );
				connects.append( j+vid );
				connects.append( (j==(div-1)) ? vid : j+vid+1 );
			}
			else if ( i==(div-1) ) {
				counts.append( 3 );
				connects.append( j+vid+1-div );
				connects.append( vid+1 );
				connects.append( j==(div-1) ? vid+1-div : j+vid+2-div );
			}
			else {
				counts.append( 4 );
				connects.append( j + vid+1-div );
				connects.append( j + vid+1 );
				connects.append( j == (div-1) ? vid+1 : j+vid+2 );
				connects.append( j == (div-1) ? vid+1-div : j+vid+2-div );
			}
			numV++;
		}
		vid = numV;
	}
}
MStatus
baseShapeCreator::computeInputMesh(
	const MPlug&	plug,
	MDataBlock&	datablock,
	MPointArray&	vertices,
	MIntArray&	counts,
	MIntArray&	connects,
	MVectorArray&	normals
)
{
	MStatus	stat;
	MDataHandle	inputData = datablock.inputValue(inputMesh, &stat);
	MCHECKERROR(stat, "compute get inputMesh")
	MObject	surf = inputData.asMesh();
	MObject	thisObj = thisMObject();
	MPlug	surfPlug(thisObj, inputMesh);
	if (!surfPlug.isConnected()) {
		stat = datablock.setClean(plug);
		MCHECKERROR(stat, "compute setClean")
		return MS::kFailure;
	}
	MFnMesh	surfFn(surf, &stat);
	MCHECKERROR(stat, "compute - MFnMesh error")
	stat = surfFn.getPoints(vertices, MSpace::kObject);
	if (vertices.length() > 20) {
		MGlobal::displayWarning("# of vertices > 20: set to 20");
		vertices.setLength(20);
	}
	MCHECKERROR(stat, "compute getPoints")
	for (int i = 0; i < surfFn.numPolygons(); ++i) {
		MIntArray	polyVerts;
		surfFn.getPolygonVertices(i, polyVerts);
		int	pvc = polyVerts.length();
		counts.append(pvc);
		for (int v = 0; v < pvc; ++v) {
			connects.append(polyVerts[v]);
		}
	}
#if 3
	{
#if 1
		MStatus	ms;
		MDagPath	dagPathShape;
#if 1
		ms = findExternalDagPath2(plug.node(), dagPathShape);
#else
		ms = findExternalDagPath2(plug, dagPathShape);
#endif
		if (ms != MStatus::kSuccess) {
			cerr << "Info: computeInputMesh: " << endl;
			cerr << "findExternalDagPath2(|" << plug.info() << "|) NOT return MStatus::kSuccess: ";
			cerr << ms << endl;
		} else {
			MObject	transform = dagPathShape.transform(&ms); if (!ms) cout << ms << endl;
			MFnTransform	fnTransform(transform, &ms); if (!ms) cout << ms << endl;
			MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cout << ms << endl;
#if 3
			__matrix = xformMatrix.asMatrix();
#endif
		}
#else
		MStatus	ms;
		MObject	node = plug.node(&ms); if (!ms) cout << ms << endl;
		MFnDependencyNode	depNode(node, &ms); if (!ms) cout << ms << endl;
		bool	bFound;
				MPlug	plug_inputMesh = depNode.findPlug("inputMesh", false, &ms); if (!ms) cout << ms << endl;
				MPlugArray	connectedToPlug_inputMesh;
				bFound = plug_inputMesh.connectedTo(connectedToPlug_inputMesh, true, false, &ms);
				if (!ms || !bFound)
					cout << ms << endl;
				for (unsigned k = 0; k < 1 && k < connectedToPlug_inputMesh.length(); ++k) {
					MPlug&	plug_worldMeshArray = connectedToPlug_inputMesh[k];
					bool	isElement = plug_worldMeshArray.isElement(&ms);
					if (!isElement || !ms)
						continue;
					MPlug	plug_worldMesh = plug_worldMeshArray.array(&ms); if (!ms) cout << ms << endl;
					MString	partialName_useLongNames = plug_worldMesh.partialName(false, false, false, false, false, true, &ms); if (!ms) cout << ms << endl;
					if (partialName_useLongNames != "worldMesh")
						continue;
					MObject	mesh = plug_worldMesh.node(&ms); if (!ms) cout << ms << endl;
					if (!mesh.hasFn(MFn::kMesh) || !mesh.hasFn(MFn::kShape))
						continue;
					MFnDagNode	dagNode(mesh, &ms); if (!ms) cout << ms << endl;
					MDagPath	dagPath;
					ms = dagNode.getPath(dagPath); if (!ms) cout << ms << endl;
					MObject	transform = dagPath.transform(&ms); if (!ms) cout << ms << endl;
					MFnTransform	fnTransform(transform, &ms); if (!ms) cout << ms << endl;
					MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cout << ms << endl;
					__matrix = xformMatrix.asMatrix();
					cout << "__matrix: " << __matrix << endl;
				}
#endif
	}
#endif
	for (int n = 0; n < (int) vertices.length(); ++n) {
		MStatus	ms;
		MVector	normal;
		ms = surfFn.getVertexNormal(n, normal);
#if 3
		ms = normals.append(normal);
#else
		ms = normals.append(normal);
#endif
	}
#if 0
	{
		MStatus	ms;
		cout << "# of UV Sets = " << surfFn.numUVSets(&ms) << endl;
		cout << ms << endl;
		MStringArray	setNames;
		ms = surfFn.getUVSetNames(setNames);
		cout << "# of UV Set Names = " << setNames.length() << endl;
		for (unsigned i = 0; i < setNames.length(); ++i)
			cout << "UV Set[" << i << "]:= |" << setNames[i] << "|" << endl;
		MFloatArray	uArray, vArray;
		ms = surfFn.getUVs(uArray, vArray);
		cout << "# of Us = " << uArray.length() << endl;
		for (unsigned i = 0; i < uArray.length(); ++i)
			cout << "u[" << i << "] = " << uArray[i] << endl;
		cout << "# of Vs = " << vArray.length() << endl;
		for (unsigned i = 0; i < vArray.length(); ++i)
			cout << "v[" << i << "] = " << vArray[i] << endl;
		MIntArray	uvCounts;
		MIntArray	uvIds;
		ms = surfFn.getAssignedUVs(uvCounts, uvIds);
		cout << "# of uvCounts = " << uvCounts.length() << endl;
		for (unsigned i = 0; i < uvCounts.length(); ++i)
			cout << "uvCounts[" << i << "] = " << uvCounts[i] << endl;
		cout << "# of uvIds = " << uvIds.length() << endl;
		for (unsigned i = 0; i < uvIds.length(); ++i)
			cout << "uvIds[" << i << "] = " << uvIds[i] << endl;
		MFloatVectorArray	tangents;
		ms = surfFn.getTangents(tangents);
		cout << "# of element(s) in tangents = " << tangents.length() << endl;
		for (unsigned i = 0; i < tangents.length(); ++i) {
			cout << "tangents[" << i << "]: " << tangents[i] << endl;
		}
		MFloatVectorArray	binormals;
		ms = surfFn.getTangents(binormals);
		cout << "# of element(s) in binormals = " << binormals.length() << endl;
		for (unsigned i = 0; i < binormals.length(); ++i) {
			cout << "binormals[" << i << "]: " << binormals[i] << endl;
		}
	}
#endif
	return MS::kSuccess;
}
void* baseShapeCreator::creator()
{
	return new baseShapeCreator();
}
MStatus baseShapeCreator::initialize()
{
	MStatus				stat;
    MFnTypedAttribute	typedAttr;
	MFnEnumAttribute	enumAttr;
	MAKE_NUMERIC_ATTR(	size, "size", "sz",
						MFnNumericData::kDouble, 1,
						false, false, true );
	shapeType = enumAttr.create( "shapeType", "st", 0, &stat );
	MCHECKERROR( stat, "create shapeType attribute" );
	stat = enumAttr.addField( "cube", 0 );
	MCHECKERROR( stat, "add enum type cube" );
	stat = enumAttr.addField( "sphere", 1 );
	MCHECKERROR( stat, "add enum type sphere" );
    CHECK_MSTATUS( enumAttr.setHidden( false ) );
    CHECK_MSTATUS( enumAttr.setKeyable( true ) );
	ADD_ATTRIBUTE( shapeType );
	MAKE_TYPED_ATTR( inputMesh, "inputMesh", "im", MFnData::kMesh, NULL );
    outputSurface = typedAttr.create( "outputSurface", "os",
									  baseShapeData::id,
									  MObject::kNullObj, &stat );
    MCHECKERROR( stat, "create outputSurface attribute" )
	typedAttr.setWritable( false );
    ADD_ATTRIBUTE( outputSurface );
    ATTRIBUTE_AFFECTS( inputMesh, outputSurface );
    ATTRIBUTE_AFFECTS( size, outputSurface );
    ATTRIBUTE_AFFECTS( shapeType, outputSurface );
#if 0
	MStatus	ms;
	MFnTypedAttribute	externalWorldMatrxi_attr;
	MMatrix		externalWorldMatrxi_default;
	MFnMatrixData	externalWorldMatrxi_data;
	__externalWorldMatrix = externalWorldMatrxi_attr.create(
		"externalWorldMatrix", "eWM", MFnData::kMatrix,
		externalWorldMatrxi_data.create(externalWorldMatrxi_default, &ms),
		&ms
	);
	ms = addAttribute(__externalWorldMatrix);
#endif
	return MS::kSuccess;
}
