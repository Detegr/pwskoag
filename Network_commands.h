#include "Base.h"
namespace Network
{	
	enum Command
	{
		Heartbeat=1,
		Connect,
		Disconnect,
		String,
		EOP=255
	};
}
