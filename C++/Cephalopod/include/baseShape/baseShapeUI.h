#pragma once
#if 0
#define TENTACLE
#endif
#include <maya/MPxSurfaceShapeUI.h>
#include <baseShape/kPtrArray.h>
class baseShapeUI : public MPxSurfaceShapeUI
{
public:
	baseShapeUI();
	virtual ~baseShapeUI();
	virtual void	getDrawRequests(const MDrawInfo& info, bool objectAndActiveOnly, MDrawRequestQueue& requests);
	virtual void	draw(const MDrawRequest& request, M3dView& view) const;
	virtual bool	select(MSelectInfo& selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const;
#ifdef TENTACLE
	void	drawVertices0(
		const kPtrArray<kTentacle>&	tentacleArray,
		const MDrawRequest&	request,
		M3dView&	view
	) const;
	bool	selectVertices0(
		const kPtrArray<kTentacle>&	tentacleArray,
		MSelectInfo&	selectInfo,
		MSelectionList&	selectionList,
		MPointArray&	worldSpaceSelectPts
	) const;
	void	drawBoundingBox(const MDrawRequest& request, M3dView& view) const;
#else
	void	drawWireframe( const MDrawRequest & request, M3dView & view ) const;
	void	drawShaded( const MDrawRequest & request, M3dView & view ) const;
#if 1
	MStatus	getSelectBy(int& iSelectBy) const;
#endif
#if 1
	void	drawVertices0(const MDrawRequest& request, M3dView& view) const;
	void	drawVertices2(const MDrawRequest& request, M3dView& view) const;
#endif
	void	drawVertices(const MDrawRequest& request, M3dView& view) const;
	void	drawBoundingBox( const MDrawRequest & request, M3dView & view ) const;
#if 1
	bool	selectVertices0(MSelectInfo &selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const;
	bool	selectVertices2(MSelectInfo &selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const;
#endif
	bool	selectVertices(MSelectInfo &selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const;
#endif
	static  void *      creator();
protected:
	enum {
		kDrawVertices,
		kDrawWireframe,
		kDrawWireframeOnShaded,
		kDrawSmoothShaded,
		kDrawFlatShaded,
		kDrawBoundingBox,
		kLastToken
	};
#if 0
public:
	void	drawHair() const;
#endif
};
