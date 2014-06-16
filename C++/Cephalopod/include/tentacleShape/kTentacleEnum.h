#pragma once
#ifndef __TENTACLE_ENUM_H__
#define __TENTACLE_ENUM_H__
#define NUM_RAMP_CHILDREN	3
typedef enum {
	RAMP_POSITION = 0,
	RAMP_FLOAT_VALUE,
	RAMP_INTERP_TYPE,
} RAMP_ENUM;
#define NUM_TENTACLE_PARAM_CHILDREN	5
typedef enum {
	TENTACLE_PARAM_LENGTH = 0,
	TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS,
	TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY,
	TENTACLE_PARAM_CROSS_SECTION_ROTATION,
	TENTACLE_PARAM_CUT,
} TENTACLE_PARAM_ENUM;
#define NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN	3
typedef enum {
	TENTACLE_ATTRIBUTE_EDITOR_SYNC = 0,
	TENTACLE_ATTRIBUTE_EDITOR_VAL,
#if 0
	TENTACLE_ATTRIBUTE_EDITOR_WAKES,
#endif
	TENTACLE_ATTRIBUTE_EDITOR_RAMP,
} TENTACLE_ATTRIBUTE_EDITOR_ENUM;
#define NUM_TENTACLE_LENGTH_ATTRIBUTE_EDITOR_CHILDREN	2
#define NUM_TENTACLE_CUT_ATTRIBUTE_EDITOR_CHILDREN	2
#if 1
#define NUM_TENTACLE_ROOT_CHILDREN	(3 + 3)
#else
#define NUM_TENTACLE_ROOT_CHILDREN	3
#endif
typedef enum {
	TENTACLE_ROOT_POLY_ID = 0,
	TENTACLE_ROOT_POSITION,
	TENTACLE_ROOT_NORMAL,
#if 1
#if 1
	TENTACLE_ROOT_HIT_VERTEX_LIST,
#else
	TENTACLE_ROOT_HIT_TRIANGLE_ID,
#endif
	TENTACLE_ROOT_HIT_BARY1,
	TENTACLE_ROOT_HIT_BARY2,
#endif
} TENTACLE_ROOT_ENUM;
#if 1
#define NUM_TENTACLE_ARRAY_CHILDREN	(13 + 1)
#else
#define NUM_TENTACLE_ARRAY_CHILDREN	13
#endif
typedef enum {
	TENTACLE_ARRAY_ROOT = 0,
#if 1
	TENTACLE_ARRAY_QUATERNION,
#endif
	TENTACLE_ARRAY_LOGITUDINAL,
	TENTACLE_ARRAY_TRANSVERSAL,
	TENTACLE_ARRAY_MAJOR_RADIUS,
	TENTACLE_ARRAY_MAJOR_RADIUS_SCALE,
	TENTACLE_ARRAY_MAJOR_RADIUS_RAMP,
	TENTACLE_ARRAY_ECCENTRICITY,
	TENTACLE_ARRAY_ECCENTRICITY_SCALE,
	TENTACLE_ARRAY_ECCENTRICITY_RAMP,
	TENTACLE_ARRAY_ROTATION,
	TENTACLE_ARRAY_ROTATION_LOCAL,
	TENTACLE_ARRAY_ROTATION_RAMP,
	TENTACLE_ARRAY_CUT,
} TENTACLE_ARRAY_ENUM;
#define NUM_TENTACLE_CUT_VAL_CHILDREN	4
typedef enum {
#if 1
	TENTACLE_CUT_VAL_FRAME = 0,
#else
	TENTACLE_CUT_VAL_HAVE = 0,
#endif
	TENTACLE_CUT_VAL_T,
	TENTACLE_CUT_VAL_POLAR,
	TENTACLE_CUT_VAL_AZIMUTHAL,
} TENTACLE_CUT_VAL_ENUM;
#if 2
#define NUM_TENTACLE_ARRAY_ARRAY_CHILDREN	2
typedef enum {
	TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER	= 0,
	TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT,
};
#endif
struct TentacleDo {
	enum Action {
		Remove = -1,
		None = 0,
		Add = 1,
		Update = 2,
		Move,
		Refresh = 10,
#if 1
		Load = 20,
		Save = 21,
#endif
	};
	friend ostream&	operator<<(ostream& os, const TentacleDo::Action& action)
	{
		return os << (action == Remove ? "Remove" : (action == None ? "None" : (action == Add ? "Add" : (action == Update ? "Update" : (action == Move ? "Move" : (action == Refresh ? "Refresh" : (action == Load ? "Load" : (action == Save ? "Save" : ("Unknown")))))))));
	}
};
struct RampWakes {
	enum Type {
		None = -1,
		MajorRadius = 0,
		Eccentricity,
		Rotation,
	};
	friend ostream&	operator<<(ostream& os, const RampWakes::Type& type)
	{
		return os << (type == None ? "None" : (type == MajorRadius ? "MajorRadius" : (type == Eccentricity ? "Eccentricity" : (type == Rotation ? "Rotation" : ("Unknown")))));
	}
};
struct RampDisplay {
	enum Type {
		None = -1,
		ViewPort = 0,
		CameraFront,
	};
	friend ostream&	operator<<(ostream& os, const RampDisplay::Type& type)
	{
		return os << (type == None ? "None" : (type == ViewPort ? "ViewPort" : (type == CameraFront ? "CameraFront" : ("Unknown"))));
	}
};
struct TentacleParameter {
	enum Type {
		None = -1,
		Length = 0,
		MajorRadius,
		Eccentricity,
		Rotation,
		Cut,
	};
	friend ostream&	operator<<(ostream& os, const TentacleParameter::Type& type)
	{
		return os << (type == None ? "None" : (type == Length ? "Length" : (type == MajorRadius ? "MajorRadius" : (type == Eccentricity ? "Eccentricity" : (type ==  Rotation ? "Rotation" : (type == Cut ? "Cut" : ("None")))))));
	}
};
#if 0
struct AttributeEditor {
	enum Type {
		None = -1,
		Sync = 0,
		Value,
		Wakes,
		Ramp,
	};
};
struct CutEnum {
	enum Type {
		None = -1,
		Have = 0,
		T,
		Polar,
		Azimuthal,
	};
};
#endif
#endif
