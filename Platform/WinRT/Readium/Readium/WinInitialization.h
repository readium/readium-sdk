#ifndef __Readium_initialization_h__
#define __Readium_initialization_h__

#include "Readium.h"
#include "ePub3\initialization.h"

BEGIN_READIUM_API

public ref class Initialization sealed
{

public:

	static void InitializeSdk();

	static void PopulateFilterManager();

private:
	Initialization() {}

};


END_READIUM_API

#endif	/* __Readium_initialization_h__ */
