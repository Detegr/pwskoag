#pragma once

struct NET
{
	enum
	{
		Connect,
		Disconnect,
		ModelBegin,
		ModelIndex,
		ModelDimensions,
		EntityBegin,
		FullEntityBegin,
		EntityDeleted
	};
};
