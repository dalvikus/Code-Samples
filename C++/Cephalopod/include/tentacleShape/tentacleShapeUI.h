#pragma once
#ifndef __TENTACLE_SHAPE_UI__
#define __TENTACLE_SHAPE_UI__
#include <baseShape/baseShapeUI.h>
struct kTentacle;
struct RampTuple;
class tentacleShapeUI : public baseShapeUI
{
public:
	tentacleShapeUI();
	virtual ~tentacleShapeUI();
	static	void*	creator();
#if 1
	virtual void	getDrawRequests(const MDrawInfo& info, bool objectAndActiveOnly, MDrawRequestQueue& requests);
	virtual void	draw(const MDrawRequest& request, M3dView& view) const;
	virtual bool	select(MSelectInfo& selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const;
#endif
#if 1
	void	drawVertices0(
		const kPtrArray<kTentacle>&	tentacleArray,
		const MDrawRequest&	request,
		M3dView&	view
	) const;
#if 2
	void	drawStrands(const MDrawRequest& , M3dView& view) const;
#endif
	bool	selectVertices0(
		const kPtrArray<kTentacle>&	tentacleArray,
		MSelectInfo&	selectInfo,
		MSelectionList&	selectionList,
		MPointArray&	worldSpaceSelectPts
	) const;
#if 2
	bool	selectStrands(
		MSelectInfo&	selectInfo,
		MSelectionList&	selectionList,
		MPointArray&	worldSpaceSelectPts
	) const;
#endif
	void	drawBoundingBox(const MDrawRequest& request, M3dView& view) const;
#endif
#if 1
	static void	DrawCreatePoint(M3dView& view, const MPoint& p);
#if 1
	static void	DrawWakes(M3dView& view, const MPointArray& strandSamplePoints, const MPointArray& tentacleSamplePoints, const kTentacle& tentacle);
#else
	static void	DrawWakes(M3dView& view, const MMatrix& mInvT, const MPointArray& worldPts, const kTentacle& tentacle);
#endif
	static void	DrawRampInViewPort(M3dView& view, const kPtrArray<RampTuple>& rampTupleArray);
	static void	DrawRampInCameraFront(M3dView& view, const kPtrArray<RampTuple>& rampTupleArray);
#endif
};
#endif
