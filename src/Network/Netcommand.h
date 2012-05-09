#pragma once

namespace pwskoag
{
	struct C_Netcommand
	{
		enum e_Command
		{
			Connect,
			Disconnect,
			Update,
			HeartBeat,
			ConnectedPlayers,
			EOP
		};
	};
}