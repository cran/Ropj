// tree.hh relies on unqualified ptrdiff_t and size_t, which aren't
// guaranteed to be defined. This actually breaks on GCC 6.3.0 (Debian
// Stretch). Work around the problem by making sure that the include
// files defining global types are processed.
#include <stddef.h>
#include "liborigin/OriginFile.cpp"
#include "liborigin/OriginParser.cpp"

#ifdef _WIN32
	// MinGW compiler warns on the use of C++11 time formats %F and %T,
	// despite UCRT supporting them. There's only one such call in
	// liborigin, and we never perform it. Inistead of patching the
	// source code, which resides in the upstream repo, let's patch the
	// call out using the preprocessor.
	// 1. Make sure that the symbol is defined and the include file
	// won't be processed again
	#include <ctime>
	// 2. Remove the call
	#define strftime(s, n, f, t)
#endif
#include "liborigin/OriginAnyParser.cpp"
