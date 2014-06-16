#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_18 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__CHUNK_TYPE__RS ER__RS__VISIBILITY ER__VAR_SCOPE__ALL */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'O':
    if (memEQ(name, "ER__VAR_SCOPE__ALL", 18)) {
    /*                         ^              */
#ifdef ER__VAR_SCOPE__ALL
      *iv_return = ER__VAR_SCOPE__ALL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'S':
    if (memEQ(name, "ER__RS__VISIBILITY", 18)) {
    /*                         ^              */
#ifdef ER__RS__VISIBILITY
      *iv_return = ER__RS__VISIBILITY;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "ER__CHUNK_TYPE__RS", 18)) {
    /*                         ^              */
#ifdef ER__CHUNK_TYPE__RS
      *iv_return = ER__CHUNK_TYPE__RS;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_19 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__CHUNK_TYPE__SHD ER__RS__CAST_SHADOW ER__RS__RECV_SHADOW */
  /* Offset 11 gives the best switch position.  */
  switch (name[11]) {
  case 'T':
    if (memEQ(name, "ER__RS__CAST_SHADOW", 19)) {
    /*                          ^              */
#ifdef ER__RS__CAST_SHADOW
      *iv_return = ER__RS__CAST_SHADOW;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'V':
    if (memEQ(name, "ER__RS__RECV_SHADOW", 19)) {
    /*                          ^              */
#ifdef ER__RS__RECV_SHADOW
      *iv_return = ER__RS__RECV_SHADOW;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'Y':
    if (memEQ(name, "ER__CHUNK_TYPE__SHD", 19)) {
    /*                          ^              */
#ifdef ER__CHUNK_TYPE__SHD
      *iv_return = ER__CHUNK_TYPE__SHD;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__DECLARE__NO_FRAME ER__STACK_TYPE__FRAME ER__VAR_SCOPE__GLOBAL */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'E':
    if (memEQ(name, "ER__DECLARE__NO_FRAME", 21)) {
    /*                         ^                 */
#ifdef ER__DECLARE__NO_FRAME
      *iv_return = ER__DECLARE__NO_FRAME;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'O':
    if (memEQ(name, "ER__VAR_SCOPE__GLOBAL", 21)) {
    /*                         ^                 */
#ifdef ER__VAR_SCOPE__GLOBAL
      *iv_return = ER__VAR_SCOPE__GLOBAL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "ER__STACK_TYPE__FRAME", 21)) {
    /*                         ^                 */
#ifdef ER__STACK_TYPE__FRAME
      *iv_return = ER__STACK_TYPE__FRAME;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_24 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__CONTENT_FATE__DELETE ER__DECLARE__FRAME_BLOCK ER__SHADER_TYPE__SURFACE
     ER__VAR_SCOPE__JOB_CHUNK */
  /* Offset 5 gives the best switch position.  */
  switch (name[5]) {
  case 'A':
    if (memEQ(name, "ER__VAR_SCOPE__JOB_CHUNK", 24)) {
    /*                    ^                         */
#ifdef ER__VAR_SCOPE__JOB_CHUNK
      *iv_return = ER__VAR_SCOPE__JOB_CHUNK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'E':
    if (memEQ(name, "ER__DECLARE__FRAME_BLOCK", 24)) {
    /*                    ^                         */
#ifdef ER__DECLARE__FRAME_BLOCK
      *iv_return = ER__DECLARE__FRAME_BLOCK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'H':
    if (memEQ(name, "ER__SHADER_TYPE__SURFACE", 24)) {
    /*                    ^                         */
#ifdef ER__SHADER_TYPE__SURFACE
      *iv_return = ER__SHADER_TYPE__SURFACE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'O':
    if (memEQ(name, "ER__CONTENT_FATE__DELETE", 24)) {
    /*                    ^                         */
#ifdef ER__CONTENT_FATE__DELETE
      *iv_return = ER__CONTENT_FATE__DELETE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_25 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__CONTENT_STATE__DELETE ER__STACK_TYPE__ATTRIBUTE
     ER__STACK_TYPE__TRANSFORM */
  /* Offset 21 gives the best switch position.  */
  switch (name[21]) {
  case 'B':
    if (memEQ(name, "ER__STACK_TYPE__ATTRIBUTE", 25)) {
    /*                                    ^          */
#ifdef ER__STACK_TYPE__ATTRIBUTE
      *iv_return = ER__STACK_TYPE__ATTRIBUTE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'F':
    if (memEQ(name, "ER__STACK_TYPE__TRANSFORM", 25)) {
    /*                                    ^          */
#ifdef ER__STACK_TYPE__TRANSFORM
      *iv_return = ER__STACK_TYPE__TRANSFORM;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "ER__CONTENT_STATE__DELETE", 25)) {
    /*                                    ^          */
#ifdef ER__CONTENT_STATE__DELETE
      *iv_return = ER__CONTENT_STATE__DELETE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_28 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__GET_RIB_DIRECTIVE__ERROR ER__GET_RIB_DIRECTIVE__VALID
     ER__SHADER_TYPE__LIGHTSOURCE */
  /* Offset 26 gives the best switch position.  */
  switch (name[26]) {
  case 'C':
    if (memEQ(name, "ER__SHADER_TYPE__LIGHTSOURCE", 28)) {
    /*                                         ^        */
#ifdef ER__SHADER_TYPE__LIGHTSOURCE
      *iv_return = ER__SHADER_TYPE__LIGHTSOURCE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "ER__GET_RIB_DIRECTIVE__VALID", 28)) {
    /*                                         ^        */
#ifdef ER__GET_RIB_DIRECTIVE__VALID
      *iv_return = ER__GET_RIB_DIRECTIVE__VALID;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'O':
    if (memEQ(name, "ER__GET_RIB_DIRECTIVE__ERROR", 28)) {
    /*                                         ^        */
#ifdef ER__GET_RIB_DIRECTIVE__ERROR
      *iv_return = ER__GET_RIB_DIRECTIVE__ERROR;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_33 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE ER__ENUM_RIB_DIRECTIVE__DONT_CARE
     ER__ENUM_RIB_DIRECTIVE__FRAME_END ER__ENUM_RIB_DIRECTIVE__WORLD_END */
  /* Offset 27 gives the best switch position.  */
  switch (name[27]) {
  case 'L':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__WORLD_END", 33)) {
    /*                                          ^            */
#ifdef ER__ENUM_RIB_DIRECTIVE__WORLD_END
      *iv_return = ER__ENUM_RIB_DIRECTIVE__WORLD_END;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'M':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__FRAME_END", 33)) {
    /*                                          ^            */
#ifdef ER__ENUM_RIB_DIRECTIVE__FRAME_END
      *iv_return = ER__ENUM_RIB_DIRECTIVE__FRAME_END;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'R':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE", 33)) {
    /*                                          ^            */
#ifdef ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE
      *iv_return = ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__DONT_CARE", 33)) {
    /*                                          ^            */
#ifdef ER__ENUM_RIB_DIRECTIVE__DONT_CARE
      *iv_return = ER__ENUM_RIB_DIRECTIVE__DONT_CARE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_35 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
     ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN */
  /* Offset 27 gives the best switch position.  */
  switch (name[27]) {
  case 'H':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE", 35)) {
    /*                                          ^              */
#ifdef ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
      *iv_return = ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN", 35)) {
    /*                                          ^              */
#ifdef ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN
      *iv_return = ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'M':
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN", 35)) {
    /*                                          ^              */
#ifdef ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN
      *iv_return = ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/usr/bin/perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV)};
my @names = (qw(ER__CHUNK_TYPE__BAKE ER__CHUNK_TYPE__RS ER__CHUNK_TYPE__SHD
	       ER__CONTENT_FATE__DELETE ER__CONTENT_FATE__PRINT
	       ER__CONTENT_FATE__SAVE ER__CONTENT_FATE__UNDEF
	       ER__CONTENT_STATE__ADD ER__CONTENT_STATE__DELETE
	       ER__DECLARE__FRAME_BLOCK ER__DECLARE__FRAME_BLOCK_OUTSIDE
	       ER__DECLARE__NO_FRAME
	       ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
	       ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
	       ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
	       ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
	       ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
	       ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
	       ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
	       ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
	       ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
	       ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
	       ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
	       ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE
	       ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
	       ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
	       ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT
	       ER__ENUM_RIB_DIRECTIVE__DISPLAY
	       ER__ENUM_RIB_DIRECTIVE__DONT_CARE
	       ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN
	       ER__ENUM_RIB_DIRECTIVE__FRAME_END
	       ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
	       ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
	       ER__ENUM_RIB_DIRECTIVE__SURFACE
	       ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
	       ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
	       ER__ENUM_RIB_DIRECTIVE__UNDEF
	       ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN
	       ER__ENUM_RIB_DIRECTIVE__WORLD_END ER__GET_RIB_DIRECTIVE__EOF
	       ER__GET_RIB_DIRECTIVE__ERROR ER__GET_RIB_DIRECTIVE__EXCEPTION
	       ER__GET_RIB_DIRECTIVE__UNKNOWN ER__GET_RIB_DIRECTIVE__VALID
	       ER__RS__CAST_SHADOW ER__RS__DELETE ER__RS__MATTE
	       ER__RS__RECV_SHADOW ER__RS__RT ER__RS__SSSC ER__RS__VISIBILITY
	       ER__RS__VIS_REFL ER__RS__VIS_REFR ER__SHADER_TYPE__LIGHTSOURCE
	       ER__SHADER_TYPE__SURFACE ER__STACK_TYPE__ATTRIBUTE
	       ER__STACK_TYPE__FRAME ER__STACK_TYPE__TRANSFORM
	       ER__VAR_SCOPE__ALL ER__VAR_SCOPE__GLOBAL
	       ER__VAR_SCOPE__JOB_CHUNK));

print constant_types(); # macro defs
foreach (C_constant ("ERModule", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "#### XS Section:\n";
print XS_constant ("ERModule", $types);
__END__
   */

  switch (len) {
  case 10:
    if (memEQ(name, "ER__RS__RT", 10)) {
#ifdef ER__RS__RT
      *iv_return = ER__RS__RT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 12:
    if (memEQ(name, "ER__RS__SSSC", 12)) {
#ifdef ER__RS__SSSC
      *iv_return = ER__RS__SSSC;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 13:
    if (memEQ(name, "ER__RS__MATTE", 13)) {
#ifdef ER__RS__MATTE
      *iv_return = ER__RS__MATTE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 14:
    if (memEQ(name, "ER__RS__DELETE", 14)) {
#ifdef ER__RS__DELETE
      *iv_return = ER__RS__DELETE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 16:
    /* Names all of length 16.  */
    /* ER__RS__VIS_REFL ER__RS__VIS_REFR */
    /* Offset 15 gives the best switch position.  */
    switch (name[15]) {
    case 'L':
      if (memEQ(name, "ER__RS__VIS_REF", 15)) {
      /*                              L      */
#ifdef ER__RS__VIS_REFL
        *iv_return = ER__RS__VIS_REFL;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'R':
      if (memEQ(name, "ER__RS__VIS_REF", 15)) {
      /*                              R      */
#ifdef ER__RS__VIS_REFR
        *iv_return = ER__RS__VIS_REFR;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 18:
    return constant_18 (aTHX_ name, iv_return);
    break;
  case 19:
    return constant_19 (aTHX_ name, iv_return);
    break;
  case 20:
    if (memEQ(name, "ER__CHUNK_TYPE__BAKE", 20)) {
#ifdef ER__CHUNK_TYPE__BAKE
      *iv_return = ER__CHUNK_TYPE__BAKE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 21:
    return constant_21 (aTHX_ name, iv_return);
    break;
  case 22:
    /* Names all of length 22.  */
    /* ER__CONTENT_FATE__SAVE ER__CONTENT_STATE__ADD */
    /* Offset 21 gives the best switch position.  */
    switch (name[21]) {
    case 'D':
      if (memEQ(name, "ER__CONTENT_STATE__AD", 21)) {
      /*                                    D      */
#ifdef ER__CONTENT_STATE__ADD
        *iv_return = ER__CONTENT_STATE__ADD;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'E':
      if (memEQ(name, "ER__CONTENT_FATE__SAV", 21)) {
      /*                                    E      */
#ifdef ER__CONTENT_FATE__SAVE
        *iv_return = ER__CONTENT_FATE__SAVE;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 23:
    /* Names all of length 23.  */
    /* ER__CONTENT_FATE__PRINT ER__CONTENT_FATE__UNDEF */
    /* Offset 19 gives the best switch position.  */
    switch (name[19]) {
    case 'N':
      if (memEQ(name, "ER__CONTENT_FATE__UNDEF", 23)) {
      /*                                  ^          */
#ifdef ER__CONTENT_FATE__UNDEF
        *iv_return = ER__CONTENT_FATE__UNDEF;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'R':
      if (memEQ(name, "ER__CONTENT_FATE__PRINT", 23)) {
      /*                                  ^          */
#ifdef ER__CONTENT_FATE__PRINT
        *iv_return = ER__CONTENT_FATE__PRINT;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 24:
    return constant_24 (aTHX_ name, iv_return);
    break;
  case 25:
    return constant_25 (aTHX_ name, iv_return);
    break;
  case 26:
    if (memEQ(name, "ER__GET_RIB_DIRECTIVE__EOF", 26)) {
#ifdef ER__GET_RIB_DIRECTIVE__EOF
      *iv_return = ER__GET_RIB_DIRECTIVE__EOF;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 28:
    return constant_28 (aTHX_ name, iv_return);
    break;
  case 29:
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__UNDEF", 29)) {
#ifdef ER__ENUM_RIB_DIRECTIVE__UNDEF
      *iv_return = ER__ENUM_RIB_DIRECTIVE__UNDEF;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 30:
    if (memEQ(name, "ER__GET_RIB_DIRECTIVE__UNKNOWN", 30)) {
#ifdef ER__GET_RIB_DIRECTIVE__UNKNOWN
      *iv_return = ER__GET_RIB_DIRECTIVE__UNKNOWN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 31:
    /* Names all of length 31.  */
    /* ER__ENUM_RIB_DIRECTIVE__DISPLAY ER__ENUM_RIB_DIRECTIVE__SURFACE */
    /* Offset 26 gives the best switch position.  */
    switch (name[26]) {
    case 'R':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__SURFACE", 31)) {
      /*                                         ^           */
#ifdef ER__ENUM_RIB_DIRECTIVE__SURFACE
        *iv_return = ER__ENUM_RIB_DIRECTIVE__SURFACE;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'S':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__DISPLAY", 31)) {
      /*                                         ^           */
#ifdef ER__ENUM_RIB_DIRECTIVE__DISPLAY
        *iv_return = ER__ENUM_RIB_DIRECTIVE__DISPLAY;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 32:
    /* Names all of length 32.  */
    /* ER__DECLARE__FRAME_BLOCK_OUTSIDE ER__GET_RIB_DIRECTIVE__EXCEPTION */
    /* Offset 28 gives the best switch position.  */
    switch (name[28]) {
    case 'S':
      if (memEQ(name, "ER__DECLARE__FRAME_BLOCK_OUTSIDE", 32)) {
      /*                                           ^          */
#ifdef ER__DECLARE__FRAME_BLOCK_OUTSIDE
        *iv_return = ER__DECLARE__FRAME_BLOCK_OUTSIDE;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'T':
      if (memEQ(name, "ER__GET_RIB_DIRECTIVE__EXCEPTION", 32)) {
      /*                                           ^          */
#ifdef ER__GET_RIB_DIRECTIVE__EXCEPTION
        *iv_return = ER__GET_RIB_DIRECTIVE__EXCEPTION;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 33:
    return constant_33 (aTHX_ name, iv_return);
    break;
  case 35:
    return constant_35 (aTHX_ name, iv_return);
    break;
  case 36:
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT", 36)) {
#ifdef ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT
      *iv_return = ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 37:
    /* Names all of length 37.  */
    /* ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
       ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END */
    /* Offset 25 gives the best switch position.  */
    switch (name[25]) {
    case 'R':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END", 37)) {
      /*                                        ^                  */
#ifdef ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
        *iv_return = ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'T':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END", 37)) {
      /*                                        ^                  */
#ifdef ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
        *iv_return = ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 39:
    /* Names all of length 39.  */
    /* ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
       ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN */
    /* Offset 25 gives the best switch position.  */
    switch (name[25]) {
    case 'R':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN", 39)) {
      /*                                        ^                    */
#ifdef ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
        *iv_return = ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'T':
      if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN", 39)) {
      /*                                        ^                    */
#ifdef ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
        *iv_return = ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 43:
    if (memEQ(name, "ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE", 43)) {
#ifdef ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
      *iv_return = ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 44:
    if (memEQ(name, "ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY", 44)) {
#ifdef ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
      *iv_return = ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 45:
    if (memEQ(name, "ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE", 45)) {
#ifdef ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
      *iv_return = ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 46:
    if (memEQ(name, "ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE", 46)) {
#ifdef ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
      *iv_return = ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 48:
    if (memEQ(name, "ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN", 48)) {
#ifdef ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
      *iv_return = ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 50:
    if (memEQ(name, "ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER", 50)) {
#ifdef ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
      *iv_return = ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 52:
    if (memEQ(name, "ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER", 52)) {
#ifdef ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
      *iv_return = ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 53:
    if (memEQ(name, "ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK", 53)) {
#ifdef ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
      *iv_return = ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 56:
    if (memEQ(name, "ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN", 56)) {
#ifdef ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
      *iv_return = ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 57:
    if (memEQ(name, "ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE", 57)) {
#ifdef ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
      *iv_return = ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 60:
    if (memEQ(name, "ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK", 60)) {
#ifdef ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
      *iv_return = ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 68:
    if (memEQ(name, "ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK", 68)) {
#ifdef ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
      *iv_return = ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

