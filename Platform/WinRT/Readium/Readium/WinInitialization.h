#ifndef __Readium_initialization_h__
#define __Readium_initialization_h__

#include "Readium.h"
#include "ePub3\initialization.h"

BEGIN_READIUM_API

public ref class Initialization sealed
{

public:
	void InitializeSdk();
	void PopulateFilterManager();
};


END_READIUM_API

#endif	/* __Readium_initialization_h__ */
