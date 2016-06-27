#include "Player.h"

namespace eae6320
{
	namespace Networking
	{
		void CreateClient();
		void CreateServer();
		void Update();
		void Destroy();
		void TagBitch();

		
		
		eae6320::Core::Player* GetConnectedPlayers();

		void InitializePlayer(eae6320::Core::Player* i_player1, eae6320::Core::Player* i_player2);
		bool IsServer();
		void SendPlayerPosition();
		bool IsInitialized();
	}
}
