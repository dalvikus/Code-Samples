#pragma once
#define BASE_SHAPE__DATA_NAME	"baseShapeData"
#include <maya/MPxGeometryData.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <baseShape/baseShapeGeometry.h>
class baseShapeData : public MPxGeometryData
{
public:
#if 1
	int	__i;
#endif
	baseShapeData();
	virtual ~baseShapeData();
	virtual MStatus			readASCII( const MArgList& argList, unsigned& idx );
	virtual MStatus			readBinary( istream& in, unsigned length );
	virtual MStatus			writeASCII( ostream& out );
	virtual MStatus			writeBinary( ostream& out );
	virtual	void			copy ( const MPxData& );
	virtual MTypeId         typeId() const;
	virtual MString         name() const;
	virtual MPxGeometryIterator* iterator( MObjectArray & componentList,
											MObject & component,
											bool useComponents);
	virtual MPxGeometryIterator* iterator( MObjectArray & componentList,
											MObject & component,
											bool useComponents,
											bool world) const;
	virtual bool	updateCompleteVertexGroup( MObject & component ) const;
	MStatus					readVerticesASCII( const MArgList&, unsigned& );
	MStatus					readNormalsASCII( const MArgList&, unsigned& );
	MStatus					readFacesASCII( const MArgList&, unsigned& );
	MStatus					writeVerticesASCII( ostream& out );
	MStatus					writeNormalsASCII( ostream& out );
	MStatus					writeFacesASCII( ostream& out );
	static void * creator();
public:
	static const MString typeName;
	static const MTypeId id;
	baseShapeGeometry* fGeometry;
};
