#if 0
#define baseShapeData_DEBUG	1
#include <iomanip>
#endif
#include <maya/MIOStream.h>
#include <baseShape/baseShapeData.h>
#include "baseShapeGeometryIterator.h"
#include "api_macros.h"
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MArgList.h>
#define kDblQteChar				"\""
#define kSpaceChar				"	"
#define kWrapString				"\n\t\t"
#define kVertexKeyword			"v"
#define kNormalKeyword			"vn"
#define kTextureKeyword			"vt"
#define kFaceKeyword			"face"
const MTypeId	baseShapeData::id(0x80777 + 1);
const MString baseShapeData::typeName( "baseShapeData" );
baseShapeData::baseShapeData() : fGeometry( NULL )
{
	fGeometry = new baseShapeGeometry;
	static int	i = 0;
	__i = i++;
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::baseShapeData(): __i = " << __i << ", fGeometry -> 0x" << std::setw(10) << std::setfill('0') << fGeometry << endl;
#endif
}
baseShapeData::~baseShapeData()
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::~baseShapeData(): __i = " << __i << ", fGeometry -> 0x" << std::setw(10) << std::setfill('0') << fGeometry << endl;
#endif
	if ( NULL != fGeometry ) {
		delete fGeometry;
		fGeometry = NULL;
	}
}
MStatus baseShapeData::readASCII( const MArgList& argList, unsigned& index )
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::readASCII()" << endl;
#endif
	if ( ! readVerticesASCII(argList,index) ) {
		return MS::kFailure;
	}
	else if ( ! readNormalsASCII(argList,++index) ) {
		return MS::kFailure;
	}
	else if ( ! readFacesASCII(argList,++index) ) {
		return MS::kFailure;
	}
	return MS::kSuccess;
}
MStatus baseShapeData::readBinary( istream& , unsigned  )
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::readBinary()" << endl;
#endif
	return MS::kSuccess;
}
MStatus baseShapeData::writeASCII( ostream& out )
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::writeASCII()" << endl;
#endif
	if ( ! writeVerticesASCII(out) ) {
		return MS::kFailure;
	}
	else if ( ! writeNormalsASCII(out) ) {
		return MS::kFailure;
	}
	else if ( ! writeFacesASCII(out) ) {
		return MS::kFailure;
	}
	return MS::kSuccess;
}
MStatus baseShapeData::writeBinary( ostream&  )
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::writeBinary()" << endl;
#endif
	return MS::kSuccess;
}
void baseShapeData::copy ( const MPxData& other )
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::copy()" << endl;
#endif
	*fGeometry = *(((const baseShapeData &)other).fGeometry);
}
MTypeId baseShapeData::typeId() const
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::typeId()" << endl;
#endif
	return baseShapeData::id;
}
MString baseShapeData::name() const
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::name()" << endl;
#endif
	return baseShapeData::typeName;
}
void * baseShapeData::creator()
{
	return new baseShapeData;
}
MPxGeometryIterator* baseShapeData::iterator( MObjectArray & componentList,
											MObject & component,
											bool useComponents)
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::iterator()" << endl;
#endif
	baseShapeGeometryIterator * result = NULL;
	if ( useComponents ) {
		result = new baseShapeGeometryIterator( fGeometry, componentList );
	}
	else {
		result = new baseShapeGeometryIterator( fGeometry, component );
	}
	return result;
}
MPxGeometryIterator* baseShapeData::iterator( MObjectArray & componentList,
											MObject & component,
											bool useComponents,
											bool ) const
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::iterator(bool world)" << endl;
#endif
	baseShapeGeometryIterator * result = NULL;
	if ( useComponents ) {
		result = new baseShapeGeometryIterator( fGeometry, componentList );
	}
	else {
		result = new baseShapeGeometryIterator( fGeometry, component );
	}
	return result;
}
bool baseShapeData::updateCompleteVertexGroup( MObject & component ) const
{
#ifdef baseShapeData_DEBUG
	cout << "baseShapeData::updateCompleteVertexGroup()" << endl;
#endif
	MStatus stat;
	MFnSingleIndexedComponent fnComponent( component, &stat );
	if ( stat && (NULL != fGeometry) && (fnComponent.isComplete()) ) {
		int maxVerts ;
		fnComponent.getCompleteData( maxVerts );
		int numVertices = fGeometry->vertices.length();
		if ( (numVertices > 0) && (maxVerts != numVertices) ) {
			fnComponent.setCompleteData( numVertices );
			return true;
		}
	}
	return false;
}
MStatus baseShapeData::writeVerticesASCII( ostream& out )
{
	MPoint vertex;
	int vertexCount = fGeometry->vertices.length();
	out << "\n";
	out << kWrapString;
	out << kDblQteChar << kVertexKeyword << kDblQteChar
		<< kSpaceChar << vertexCount;
	for ( int i=0; i<vertexCount; i++ ) {
		vertex = fGeometry->vertices[i];
		out << kWrapString;
		out << vertex[0] << kSpaceChar;
		out << vertex[1] << kSpaceChar;
		out << vertex[2];
	}
	return MS::kSuccess;
}
MStatus baseShapeData::writeNormalsASCII( ostream& out )
{
	MVector normal;
	int normalCount = fGeometry->normals.length();
	out << "\n";
	out << kWrapString;
	out << kDblQteChar << kNormalKeyword << kDblQteChar
		<< kSpaceChar << normalCount;
	for ( int i=0; i<normalCount; i++ ) {
		normal = fGeometry->normals[i];
		out << kWrapString;
		out << normal[0] << kSpaceChar;
		out << normal[1] << kSpaceChar;
		out << normal[2];
	}
	return MS::kSuccess;
}
MStatus baseShapeData::writeFacesASCII( ostream& out )
{
	int numFaces = fGeometry->face_counts.length();
	int vid = 0;
	for ( int f=0; f<numFaces; f++ )
	{
		int faceVertexCount = fGeometry->face_counts[f];
		out << "\n";
		out << kWrapString;
		out << kDblQteChar << kFaceKeyword << kDblQteChar
			<< kSpaceChar << faceVertexCount;
		out << kWrapString;
		for ( int v=0; v<faceVertexCount; v++ )
		{
			out << fGeometry->face_connects[vid] << kSpaceChar;
			vid++;
		}
	}
	return MS::kSuccess;
}
MStatus baseShapeData::readVerticesASCII( const MArgList& argList,
										unsigned& index )
{
	MStatus result;
	MString geomStr;
	MPoint vertex;
	int vertexCount = 0;
	result = argList.get( index, geomStr );
	if ( result && (geomStr == kVertexKeyword) ) {
		result = argList.get( ++index, vertexCount );
		for ( int i=0; i<vertexCount; i++ )
		{
			if ( argList.get( ++index, vertex ) ) {
				fGeometry->vertices.append( vertex );
			}
			else {
				result = MS::kFailure;
			}
		}
	}
	return result;
}
MStatus baseShapeData::readNormalsASCII( const MArgList& argList,
									   unsigned& index )
{
	MStatus result;
	MString geomStr;
	MPoint normal;
	int normalCount = 0;
	result = argList.get( index, geomStr );
	if ( result && (geomStr == kNormalKeyword) ) {
		result = argList.get( ++index, normalCount );
		for ( int i=0; i<normalCount; i++ )
		{
			if ( argList.get( ++index, normal ) ) {
				fGeometry->normals.append( normal );
			}
			else {
				result = MS::kFailure;
			}
		}
	}
	return result;
}
MStatus baseShapeData::readFacesASCII( const MArgList& argList,
									 unsigned& index )
{
	MStatus result = MS::kSuccess;
	MString geomStr;
	int faceCount = 0;
	int vid;
	while( argList.get(index,geomStr) && (geomStr == kFaceKeyword) )
	{
		result = argList.get( ++index, faceCount );
		fGeometry->face_counts.append( faceCount );
		for ( int i=0; i<faceCount; i++ )
		{
			if ( argList.get( ++index, vid ) ) {
				fGeometry->face_connects.append( vid );
			}
			else {
				result = MS::kFailure;
			}
		}
		index++;
	}
	fGeometry->faceCount = fGeometry->face_counts.length();
	return result;
}
