#include <brushTool/CSelection.h>
CSelection::CSelection(const CSelection& other)
{
	*this = other;
}
CSelection&	CSelection::operator=(const CSelection& other)
{
	if (this != &other) {
#ifdef _DEBUG4
		this->__typeName = other.__typeName;
#endif
		this->dagPath = other.dagPath;
#if 1
		this->hasSkinCluster = other.hasSkinCluster;
#endif
		this->selectBy = other.selectBy;
		this->numPoints = other.numPoints;
		this->worldMatrix = other.worldMatrix;
		for (unsigned n = 0; n < other.guideArray.length(); ++n) {
			SelectedGuidePtr	p = new SelectedGuide(other.guideArray(n));
			this->guideArray.append(p);
		}
	}
	return *this;
}
void	listActiveSelectionArray(
	const char*	typeName,
	SelectionArray&	selectionArray,
	const char*	callerName
)
{
	MString	caller(callerName);
	cout << "################################################################################" << endl;
	cout << caller << "# of " << typeName << " = " << selectionArray.length() << endl;
	for (unsigned i = 0; i < selectionArray.length(); ++i) {
		CSelection&	selection = selectionArray(i);
		cout << caller << typeName << "[" << i << "]" << endl;
		cout << caller << "\t" << "selectBy = " << selection.selectBy << endl;
		cout << caller << "\t" << "MDagPath: \"" << selection.dagPath.fullPathName() << "\"" << endl;
		cout << caller << "\t" << "numPoints = " << selection.numPoints << endl;
		const MMatrix&	worldMatrix = selection.worldMatrix;
		cout << caller << "\t" << "worldMatrix: " << worldMatrix << endl;
		cout << caller << "\t" << "guideArray: ";
		cout << "# of guideArray = " << selection.guideArray.length() << endl;
		for (unsigned k = 0; k < selection.guideArray.length(); ++k) {
			cout << caller << "\t" << "guideArray[" << k << "](0x" << selection.guideArray[k] << ")" << endl;
			const SelectedGuide&	guide = selection.guideArray(k);
			cout << caller << "\t\t" << "uIndex = " << guide.uIndex << endl;
			const unsigned&	vertexBits = guide.vertexBits;
#if 1
			(void) printf("%s\t\tvertexBits = 0x%08X\n", caller.asChar(), vertexBits);
#else
			cout << caller << "\t\t" << "vertexBits = 0x";
std::ios_base::fmtflags	flags_ = cout.setf(std::ios_base::hex | std::ios_base::uppercase);
std::streamsize	width_ = cout.width(8);
char	fillChar = cout.fill('0');
			cout << vertexBits;
cout.flags(flags_);
cout.width(width_);
cout.fill(fillChar);
			cout << "..." << vertexBits;
			cout << endl;
#endif
			unsigned	bit = 1 << 0;
			cout << caller << "\t\t" << "worldPts: ";
			cout << "# of worldPts = " << guide.offsets.length() << endl;
			bool	isDotDone = false;
			MPoint	basePoint = guide.offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (unsigned n = 0; n < guide.offsets.length(); ++n, bit <<= 1) {
				if (n > 1) {
					if (n < guide.offsets.length() - 2) {
						if (!isDotDone) {
							cout << caller << "\t\t\t......" << endl;
							isDotDone = true;
						}
						continue;
					}
				}
				if (n > 0) {
					const MVector&	offset = guide.offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
				cout << caller << "\t\t\t" << "worldPts[" << n << "]: " << worldPt << ": ";
				cout << (vertexBits & bit ? "True" : "False") << endl;
			}
		}
	}
	cout << "################################################################################" << endl;
}
