#include "Base.h"
namespace Network
{	
	enum Command
	{
		Heartbeat,
		Connect,
		Disconnect,
		String,
		EOP=255
	};
}
