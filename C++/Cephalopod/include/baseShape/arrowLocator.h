#include <maya/MPxLocatorNode.h>
#define SIMULATION_PARAMETER_ARROW_LOCATOR_NAME	"CTSimulationParameterArrowLocator"
class arrowLocator : public MPxLocatorNode
{
public:
	arrowLocator();
	virtual ~arrowLocator();
	virtual MStatus	compute(const MPlug& plug, MDataBlock& data);
	virtual void	draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
	virtual void	postConstructor();
	virtual bool	isBounded() const;
	virtual MBoundingBox	boundingBox() const;
	static void*	creator();
	static MStatus	initialize();
	static  MObject         point;
	static  MObject         origin;
	static  MObject         to;
	static  MObject         len;
	static  MObject         dir;
public:
	static MTypeId	id;
};
