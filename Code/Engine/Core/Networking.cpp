#include "Networking.h"

#include "../../External/RakNet/RakPeerInterface.h"
#include "../../External/RakNet/MessageIdentifiers.h"
#include "../../External/RakNet/BitStream.h"
#include "../../External/RakNet/RakNetTypes.h"
#include "../../Game/Game/WindowsProgram.h"

#include "../Graphics/Graphics.h"

#include "../UserOutput/UserOutput.h"
#include "../Audio/AudioControl.h"
enum GameMessages
{
	ID_PLAYER_LOCATION = ID_USER_PACKET_ENUM + 1,
	ID_ENEMY_FLAG_LOCATION = ID_USER_PACKET_ENUM + 2,
	ID_ENEMY_SCORE = ID_USER_PACKET_ENUM + 3,
	ID_TAG_BITCH = ID_USER_PACKET_ENUM + 4,
};

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

namespace eae6320
{
	namespace Networking
	{
		
		RakNet::RakPeerInterface *m_Peer = NULL;
		int m_totalClients = 0;
		bool m_IsServer;
		RakNet::SystemAddress m_ServerAddress;
		RakNet::SystemAddress m_Clients[MAX_CLIENTS];

		bool m_IsInitialized;

		bool bPlayerCreated;

		eae6320::Core::Player * m_ConnectedPlayers;
		eae6320::Core::Player * m_ConnectedPlayers2;
		//Graphics::Renderable* m_ConnectedPlayers = new Graphics::Renderable[10];
		
		void Initialize()
		{
			m_Peer = RakNet::RakPeerInterface::GetInstance();
			m_IsInitialized = true;
		}

		bool IsInitialized()
		{
			return m_IsInitialized;
		}

		void TagBitch()
		{
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_TAG_BITCH);
			m_Peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, !m_IsServer ? m_ServerAddress : RakNet::AddressOrGUID(), m_IsServer);
		}

		void CreateClient()
		{
			if (m_Peer == NULL)
				Initialize();

			RakNet::SocketDescriptor sd;
			m_Peer->Startup(1, &sd, 1);
			m_IsServer = false;

			m_Peer->Connect("127.0.0.1", SERVER_PORT, 0, 0);
			m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);
		}

		//void CreatePlayer()
		//{
		//	if (!bPlayerCreated)
		//	{
		//		for (int i = 0; i < 10; i++)
		//		{
		//			bPlayerCreated = true;
		//			char path[100];
		//			sprintf_s(path, "data/phong%d.mesh", i + 1);
		//			//m_ConnectedPlayers.m_Mesh.LoadMeshFromFile(path);

		//			sprintf_s(path, "data/phong%d.material", i + 1);
		//			//m_ConnectedPlayers[i].m_Material.LoadMaterial(path);
		//		}
		//	}
		//}

		void SendPlayerPosition()
		{
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_PLAYER_LOCATION);
			bsOut.Write(m_ConnectedPlayers2->Position.x);
			bsOut.Write(m_ConnectedPlayers2->Position.y);
			bsOut.Write(m_ConnectedPlayers2->Position.z);
			bsOut.Write(m_ConnectedPlayers2->Orientation.m_w);
			bsOut.Write(m_ConnectedPlayers2->Orientation.m_x);
			bsOut.Write(m_ConnectedPlayers2->Orientation.m_y);
			bsOut.Write(m_ConnectedPlayers2->Orientation.m_z);
			bsOut.Write(m_ConnectedPlayers2->Flag);
			bsOut.Write(m_ConnectedPlayers2->score);

			m_Peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, !m_IsServer ? m_ServerAddress : RakNet::AddressOrGUID(), m_IsServer);
		}

		eae6320::Core::Player* GetConnectedPlayers()
		{
			return m_ConnectedPlayers ;
		}

		bool IsServer()
		{
			return m_IsServer;
		}
		void InitializePlayer(eae6320::Core::Player* i_player1, eae6320::Core::Player* i_player2)
		{
			m_ConnectedPlayers = i_player1;
			m_ConnectedPlayers2 = i_player2;
		}

		void Update()
		{
			RakNet::Packet *packet;
			for (packet = m_Peer->Receive(); packet; m_Peer->DeallocatePacket(packet), packet = m_Peer->Receive())
			{
				switch (packet->data[0])
				{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					UserOutput::Print("Another client has disconnected.\n");
					break;
				case ID_REMOTE_CONNECTION_LOST:
					UserOutput::Print("Another client has lost the connection.\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					UserOutput::Print("Another client has connected.\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");
					m_ServerAddress = packet->systemAddress;
					// Use a BitStream to write a custom user message
					// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
					SendPlayerPosition();
				}
					break;
				case ID_NEW_INCOMING_CONNECTION:
					UserOutput::Print("A connection is incoming.\n");
					m_Clients[m_totalClients] = packet->systemAddress;
					m_totalClients++;
					SendPlayerPosition();
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					UserOutput::Print("The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					if (m_IsServer) {
						UserOutput::Print("A client has disconnected.\n");
					}
					else {
						UserOutput::Print("We have been disconnected.\n");
					}
					break;
				case ID_PLAYER_LOCATION:
				{
					//CreatePlayer();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					float x, y, z;
					float w, Ox, Oy, Oz;
					bool Flag1, Flag2;
					int score;
					bsIn.Read(x);
					bsIn.Read(y);
					bsIn.Read(z);
					bsIn.Read(w);
					bsIn.Read(Ox);
					bsIn.Read(Oy);
					bsIn.Read(Oz);
					bsIn.Read(Flag1);
					bsIn.Read(score);
					
					
					m_ConnectedPlayers->Position.x = x;
					m_ConnectedPlayers->Position.y = y;
					m_ConnectedPlayers->Position.z = z;
					m_ConnectedPlayers->Orientation.m_w = w;
					m_ConnectedPlayers->Orientation.m_x = Ox;
					m_ConnectedPlayers->Orientation.m_y = Oy;
					m_ConnectedPlayers->Orientation.m_z = Oz;
					if (Flag1 && !m_ConnectedPlayers->Flag)
					{
						eae6320::Audio::PlayAudio(1);
					}
					m_ConnectedPlayers->Flag = Flag1;
					if (score != m_ConnectedPlayers->score)
					{
						eae6320::Audio::PlayAudio(7);
					}
					m_ConnectedPlayers->score = score;

				}
					break;

				break;

				case ID_TAG_BITCH:
				{
					ResetFlag();
				}break;
				case ID_CONNECTION_LOST:
					if (m_IsServer) {
						UserOutput::Print("A client lost the connection.\n");
					}
					else {
						UserOutput::Print("Connection lost.\n");
					}
					break;
				default:
					char buf[100];
					sprintf(buf, "Message with identifier %i has arrived.\n", packet->data[0]);
					UserOutput::Print(buf);
					break;
				}
			}
		}

		void CreateServer()
		{
			if (m_Peer == NULL)
				Initialize();

			RakNet::SocketDescriptor sd(SERVER_PORT, 0);
			m_Peer->Startup(MAX_CLIENTS, &sd, 1);
			m_IsServer = true;

			m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);
		}

		void Destroy()
		{
			RakNet::RakPeerInterface::DestroyInstance(m_Peer);
		}
	}
}