/*
	This file contains all of the function definitions for this example program
*/

// Header Files
//=============

#include "WindowsProgram.h"

// Resource.h contains the #defines for the icon resources
// that the main window will use
#include "Resources/Resource.h"
// WindowsFunctions.h contains convenience functionality for Windows features;
// in this example program we just use it to get error messages
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Graphics/Renderable.h"
#include "../../Engine/Graphics/Camera.h"
#include "../../Engine/Math/cQuaternion.h"
#include "../../Engine/Math/cMatrix_transformation.h"
#include "../../Engine/Graphics/UI.h"
#include "../../Engine/Core/CollisionSystem.h"
#include "../../Engine/Core/Player.h"
#include "../../Engine/Core/TPCamera.h"
#include "../../Engine/Audio/AudioControl.h"

// Static Data Initialization
//===========================

namespace
{
	// Instead of a pointer Windows provides a "handle"
	// to represent each window that is created
	// ("HWND" == "window handle").
	// In Windows every separate element
	// (buttons, text fields, scroll bars, etc.) is a "window",
	// and so a typical Windows program will have many different windows.
	// In our class, however, we will only have a single main window.
	HWND s_mainWindow = NULL;

	// Window classes are almost always identified by name;
	// there is also a unique ATOM associated with them,
	// but in practice Windows expects to use the class name as an identifier.
	// If you don't change the name below from the default then
	// your program could have problems when it is run at the same time on the same computer
	// as one of your classmate's
	const char* s_mainWindowClass_name = "tapia_aqeel's Main Window Class";

	eae6320::Math::cVector * Flag1Position = new eae6320::Math::cVector(100, -150, 1200);
	eae6320::Math::cVector * Flag2Position = new eae6320::Math::cVector(-100, -150, -1200);

	int player1score = 0;
	int player2score = 0;
}

// Main Function
//==============

int CreateMainWindowAndReturnExitCodeWhenItCloses( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState )
{

	// Try to create the main window
	if ( CreateMainWindow( i_thisInstanceOfTheProgram, i_initialWindowDisplayState ) )
	{
		// If the main window was successfully created wait for it to be closed
		const int exitCode = WaitForMainWindowToCloseAndReturnExitCode( i_thisInstanceOfTheProgram );
		return exitCode;
	}
	else
	{
		// If the main window wasn't created return a made-up error code
		return -1;
	}
}

// Helper Functions
//=================
bool UpdateEntities_floats();
void UpdateInputNumber(float i_gameTime);
//bool checkObjectShoot();
void checkFlagPosition(eae6320::Core::Player * i_player1, eae6320::Core::Player * i_player2);


bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState )
{
	// Every window that Windows creates must belong to a "class".
	// Note that this is different than a C++ class (but similar in theory):
	// A windows class defines things that will be the same for every window
	// that belongs to the class.
	// (In other words, every window will have exactly one class,
	// but a windows class can have many windows.)
	// To create a new windows class it must be "registered" with Windows.
	ATOM mainWindowClass = RegisterMainWindowClass( i_thisInstanceOfTheProgram );
	if ( mainWindowClass != NULL )
	{
		s_mainWindow = CreateMainWindowHandle( i_thisInstanceOfTheProgram, i_initialWindowDisplayState );
		if ( s_mainWindow == NULL )
		{
			goto OnError;
		}

		return true;

OnError:

		// Unregister the main window class
		{
			UnregisterMainWindowClass( i_thisInstanceOfTheProgram );
			mainWindowClass = NULL;
		}

		return false;
	}
	else
	{
		return false;
	}
}

int WaitForMainWindowToCloseAndReturnExitCode( const HINSTANCE i_thisInstanceOfTheProgram )
{
	// Wait for the main window to close
	int exitCode;
	bool wereThereErrors = WaitForMainWindowToClose( exitCode );

	// Clean up anything that was created/registered/initialized
	if ( OnMainWindowClosed( i_thisInstanceOfTheProgram ) )
	{
		return exitCode;
	}
	else
	{
		if ( wereThereErrors )
		{
			// If the program already had errors while waiting for the main window to close
			// the existing error exit code can be returned
			return exitCode;
		}
		else
		{
			// In a real program you might decide to define a special constant for this situation,
			// but for this example program -1 is a good arbitrary error code
			return -1;
		}
	}
}


// CreateMainWindow
//-----------------

HWND CreateMainWindowHandle( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState )
{
	// Create the main window
	HWND mainWindow;
	{
		// The window's "caption"
		// (The text that is displayed in the title bar)
		const char* windowCaption = "tapia_aqeel's EAE6320 Game Client";
		if (eae6320::Networking::IsServer())
		{
			windowCaption = "tapia_aqeel's EAE6320 Game Server";
		}
		
		// The window's style
		const DWORD windowStyle =
			// "Overlapped" is basically the same as "top-level"
			WS_OVERLAPPED
			// The caption is the title bar when in windowed-mode
			| WS_CAPTION
			// The window should never change dimensions, so only a minimize box is allowed
			| WS_MINIMIZEBOX
			// The system menu appears when you right-click the title bar
			| WS_SYSMENU;
		// The window's extended style
		const DWORD windowStyle_extended =
			// The following is a macro to make the extended style the default top-level look
			WS_EX_OVERLAPPEDWINDOW;
		// The width and height of the window.
		// A game cares about the width and height of the actual "client area",
		// which is the part of the window that doesn't include the borders and title bar;
		// this means that if we say that a game runs at a resolution of 800 x 600,
		// the actual window will be slightly bigger than that.
		// Initially, then, the window will be created with default values that Windows chooses
		// and then resized after creation)
		const int width = CW_USEDEFAULT;
		const int height = CW_USEDEFAULT;
		// The initial position of the window
		// (We don't care, and will let Windows decide)
		const int position_x = CW_USEDEFAULT;
		const int position_y = CW_USEDEFAULT;
		// Handle to the parent of this window
		// (Since this is our main window, it can't have a parent)
		const HWND hParent = NULL;
		// Handle to the menu for this window
		// (The main window won't have a menu)
		const HMENU hMenu = NULL;
		// Handle to the instance of the program that this window should be associated with
		const HINSTANCE hProgram = i_thisInstanceOfTheProgram;
		// Any arbitrary pointer can be associated with this window;
		// usually a program's own custom representation of the window will be used, like this:
		// void* userData = someCustomStructOrClass;
		// Since this example program is simple, though, no data will be associated:
		void* userData = NULL;

		// Ask Windows to create the specified window.
		// CreateWindowEx() will return a handle to the window,
		// which is what we'll use to communicate with Windows about this window
		mainWindow = CreateWindowEx( windowStyle_extended, s_mainWindowClass_name, windowCaption, windowStyle,
			position_x, position_y, width, height,
			hParent, hMenu, hProgram, userData );
		if ( mainWindow == NULL )
		{
			const char* errorCaption = "No Main Window";
			std::string errorMessage( "Windows failed to create the main window: " );
			errorMessage += eae6320::GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
			return NULL;
		}
	}

	// Change the window's size based on the desired client area resolution
	{
		// In a real game these values would come from an external source
		// rather than be hard-coded
		const int desiredWidth = 800;
		const int desiredHeight = 600;

		// Calculate how much of the window is coming from the "non-client area"
		// (the borders and title bar)
		RECT windowCoordinates;
		struct
		{
			long width;
			long height;
		} nonClientAreaSize;
		{
			// Get the coordinates of the entire window
			if ( GetWindowRect( mainWindow, &windowCoordinates ) == FALSE )
			{
				std::string errorMessage( "Windows failed to get the coordinates of the main window: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
			// Get the dimensions of the client area
			RECT clientDimensions;
			if ( GetClientRect( mainWindow, &clientDimensions ) == FALSE )
			{
				std::string errorMessage( "Windows failed to get the dimensions of the main window's client area: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
			// Get the difference between them
			nonClientAreaSize.width = ( windowCoordinates.right - windowCoordinates.left ) - clientDimensions.right;
			nonClientAreaSize.height = ( windowCoordinates.bottom - windowCoordinates.top ) - clientDimensions.bottom;
		}
		// Resize the window
		{
			BOOL shouldTheResizedWindowBeRedrawn = TRUE;
			if ( MoveWindow( mainWindow, windowCoordinates.left, windowCoordinates.top,
				desiredWidth + nonClientAreaSize.width, desiredHeight + nonClientAreaSize.height,
				shouldTheResizedWindowBeRedrawn ) == FALSE )
			{
				std::string errorMessage( "Windows failed to resize the main window: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
		}
	}

	// Display the window in the initial state that Windows requested
	ShowWindow( mainWindow, i_initialWindowDisplayState );

	return mainWindow;

OnError:

	if ( DestroyWindow( mainWindow ) == FALSE )
	{
		std::string errorMessage( "Windows failed to destroy the main window "
			"after an error in creation: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
	}
	mainWindow = NULL;

	return NULL;
}

ATOM RegisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram )
{
	WNDCLASSEX wndClassEx = { 0 };
	wndClassEx.cbSize = sizeof( WNDCLASSEX );
	wndClassEx.hInstance = i_thisInstanceOfTheProgram;

	// The class's style
	wndClassEx.style = 0;
#ifdef EAE6320_PLATFORM_GL
	// Reserve a unique device context for this window for OpenGL's use
	wndClassEx.style |= CS_OWNDC;
#endif
	// The function that will process all of the messages
	// that Windows will send to windows of this class
	wndClassEx.lpfnWndProc = OnMessageReceived;
	// Extra bytes can be set aside in the class for user data
	wndClassEx.cbClsExtra = 0;
	// Extra bytes can be set aside for each window of this class,
	// but this is usually specified for each window individually
	wndClassEx.cbWndExtra = 0;
	// The large and small icons that windows of this class should use
	// (These can be found in the Resources folder; feel free to change them)
	wndClassEx.hIcon = LoadIcon( i_thisInstanceOfTheProgram, MAKEINTRESOURCE( IDI_BIG ) );
	wndClassEx.hIconSm = LoadIcon( i_thisInstanceOfTheProgram, MAKEINTRESOURCE( IDI_SMALL ) );
	// The cursor that should display when the mouse pointer is over windows of this class
	wndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );
	// The "brush" that windows of this class should use as a background
	// (Setting this is a bit confusing but not important,
	// so don't be alarmed if the next line looks scary)
	wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>( IntToPtr( COLOR_BACKGROUND + 1 ) );
	// A menu can be specified that all windows of this class would use by default,
	// but usually this is set for each window individually
	wndClassEx.lpszMenuName = NULL;
	// The class name (see comments where this is initialized)
	wndClassEx.lpszClassName = s_mainWindowClass_name;

	// Now all of the above information is given to Windows.
	// If all goes well, the class will be successfully registered
	// and it can be specified by name when creating the main window.
	const ATOM mainWindowClass = RegisterClassEx( &wndClassEx );
	if ( mainWindowClass == NULL )
	{
		const char* errorCaption = "No Main Window Class";
		std::string errorMessage( "Windows failed to register the main window's class: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
	}
	return mainWindowClass;
}

// WaitForMainWindowToCloseAndReturnExitCode
//------------------------------------------

bool CleanupMainWindow()
{
	if ( s_mainWindow != NULL )
	{
		if ( DestroyWindow( s_mainWindow ) != FALSE )
		{
			s_mainWindow = NULL;
		}
		else
		{
			const char* errorCaption = "Couldn't Destroy Main Window";
			std::string errorMessage( "Windows failed to destroy the main window: " );
			errorMessage += eae6320::GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
			return false;
		}
	}
	return true;
}

bool OnMainWindowClosed( const HINSTANCE i_thisInstanceOfTheProgram )
{
	bool wereThereErrors = false;

	if ( !CleanupMainWindow() )
	{
		wereThereErrors = true;
	}

	if ( !UnregisterMainWindowClass( i_thisInstanceOfTheProgram ) )
	{
		wereThereErrors = true;
	}

	return !wereThereErrors;
}

LRESULT CALLBACK OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam )
{
	// DispatchMessage() will send messages to the main window here.
	// There are many messages that get sent to a window,
	// but games can ignore most of them
	// and let Windows handle them in the default way.

	// Process any messages that the game cares about
	// (any messages that are processed here should return a value
	// rather than letting the default processing function try to handle them a second time)
	switch( i_message )
	{
	// A key has been pressed down, and this is the translated character
	case WM_CHAR:
		{
			// This isn't usually the best way to handle keyboard input in a real-time game,
			// but it is a convenient way to handle an exit key
			// (The WPARAM input parameter indicates which key was pressed,
			// but this example program only cares about the escape key)
			if ( i_wParam == VK_ESCAPE )
			{
				// You do _not_ need to do this in your game,
				// but this example program shows you how to display a "message box"
				// that asks the user for confirmation
				int result;
				{
					const char* caption = "Exit Program?";
					const char* message = "Are you sure you want to quit?";
					result = MessageBox( s_mainWindow, message, caption, MB_YESNO | MB_ICONQUESTION );
				}
				if ( result == IDYES )
				{
					// Instruct Windows to send a WM_QUIT message
					{
						// The exit code is ignored,
						// but just as an example of what's possible a different exit code is sent here
						// than when the user closes the program in the usual way
						// (by clicking the X in the upper-right hand corner).
						int exitCode = 1;
						PostQuitMessage( exitCode );
					}

					// For WM_CHAR messages, return 0 to indicate that it was processed
					return 0;
				}
				else
				{
					// If the user doesn't select "Yes" to quit
					// then do nothing to keep running the program
				}
			}

			// If the key press wasn't handled pass it on to Windows to process in the default way
			break;
		}
	// The window's nonclient area is being destroyed
	case WM_NCDESTROY:
		{
			// This is the last message a window will receive
			// (Any child windows have already been destroyed).
			// After this message has been processed the window's handle will be invalid:
			s_mainWindow = NULL;

			// When the main window is destroyed
			// a WM_QUIT message should be sent
			// (if this isn't done the application would continue to run with no window).
			// This is where the exitCode in WaitForShutdown() comes from:
			int exitCode = 0;	// Arbitrary de facto success code
			PostQuitMessage( exitCode );	// This sends a WM_QUIT message

			// For WM_NCDESTROY messages, return 0 to indicate that it was processed
			return 0;
		}
	}

	// Pass any messages that weren't handled on to Windows
	return DefWindowProc( i_window, i_message, i_wParam, i_lParam );
}

bool UnregisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram )
{
	if ( UnregisterClass( s_mainWindowClass_name, i_thisInstanceOfTheProgram ) != FALSE )
	{
		return true;
	}
	else
	{
		const char* errorCaption = "Couldn't Unregister Main Window Class";
		std::string errorMessage( "Windows failed to unregister the main window's class: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
		return false;
	}
}


bool WaitForMainWindowToClose( int& o_exitCode )
{
	// Any time something happens that Windows cares about, it will send the main window a message.

	// One of the messages it sends is that the application should quit;
	// this can be sent when a user closes the window
	// (e.g. presses the X in the upper-right corner),
	// but we can also tell Windows to send a quit message if the user chooses to quit from within the game
	// (e.g. from an in-game menu)

	// Enter an infinite loop that will continue until a quit message (WM_QUIT) is received from Windows
	eae6320::Audio::Initialize();
	eae6320::Audio::AddAudioFile("data/Audio/GameMusic.wav", true, 0.2f);
	eae6320::Audio::AddAudioFile("data/Audio/EnemyPickUpFlag.wav");
	eae6320::Audio::AddAudioFile("data/Audio/PickUpFlag.wav");
	eae6320::Audio::AddAudioFile("data/Audio/ResetFlag.wav");
	eae6320::Audio::AddAudioFile("data/Audio/Scored.wav");
	eae6320::Audio::AddAudioFile("data/Audio/WalkingGround.wav", true);
	eae6320::Audio::AddAudioFile("data/Audio/WalkingUp.wav", true);
	eae6320::Audio::AddAudioFile("data/Audio/EnemyScored.wav");
	eae6320::Audio::AddAudioFile("data/Audio/Sprint.wav", true);
	eae6320::Audio::PlayAudio(0);

	eae6320::Graphics::Initialize(s_mainWindow);
	
	eae6320::Core::CollisionSystem::Initialize("data/collisionData.mesh");
	

	Camera::getInstance().m_Flag1 = *Flag1Position;
	Camera::getInstance().m_Flag2 = *Flag2Position;

	eae6320::Core::Player * player = new eae6320::Core::Player();

	eae6320::Core::Player * SecondPlayer = new eae6320::Core::Player();
	eae6320::Networking::InitializePlayer(SecondPlayer, player);
	if (eae6320::Networking::IsServer())
	{
		player->team = 0;
	}
	else
		player->team = 1;

	player->Position = Camera::getInstance().m_Position; //eae6320::Math::cVector(0, -120, 0);

	SecondPlayer->Position = Camera::getInstance().m_Position;
	SecondPlayer->Position.y += 2000;

	bool flyCamActive = false;

	
	eae6320::Core::TPCamera * playerCamera = new eae6320::Core::TPCamera();
	playerCamera->AspectRatio = (float)1024 / (float)768;
	
	
	int UIDelay = 0;
	float rotationOffset = 0;
	MSG message = { 0 };
	do
	{
		

		// To send us a message, Windows will add it to a queue.
		// Most Windows applications should wait until a message is received and then react to it.
		// Real-time programs, though, must continually draw new images to the screen as fast as possible
		// and only pause momentarily when there is a Windows message to deal with.

		// This means that the first thing that must be done every iteration of the game loop is to "peek" at the message queue
		// and see if there are any messages from Windows that need to be handled
		bool hasWindowsSentAMessage;
		{
			HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = NULL;
			unsigned int getAllMessageTypes = 0;
			unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
			hasWindowsSentAMessage = PeekMessage( &message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
				getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue ) == TRUE;
		}
		if ( !hasWindowsSentAMessage )
		{
			eae6320::Time::OnNewFrame();
			HWND currentWindow = GetForegroundWindow();
			

			
				//if (sphereEnabled)
				//{
				//	//change size of shapes and draw them
				//	
				//}

				if (eae6320::UserInput::IsKeyUp(VK_OEM_3) && currentWindow == s_mainWindow)
				{
					eae6320::Graphics::UI::ToggleDebugMenu();
				}
				if (eae6320::UserInput::IsKeyUp('F') && currentWindow == s_mainWindow)
				{
					flyCamActive = !flyCamActive;
				}
				if (eae6320::Graphics::UI::IsDebugMenuActive() && currentWindow == s_mainWindow)
				{	
					if (eae6320::UserInput::IsKeyUp(VK_UP))
						eae6320::Graphics::UI::Update(eae6320::Graphics::UI::Up);
					else if (eae6320::UserInput::IsKeyUp(VK_DOWN))
						eae6320::Graphics::UI::Update(eae6320::Graphics::UI::Down);
					else if (eae6320::UserInput::IsKeyUp(VK_SPACE))
						eae6320::Graphics::UI::Update(eae6320::Graphics::UI::Interact);
					else if (eae6320::UserInput::IsKeyUp(VK_LEFT))
						eae6320::Graphics::UI::Update(eae6320::Graphics::UI::Left);
					else if (eae6320::UserInput::IsKeyUp(VK_RIGHT))
						eae6320::Graphics::UI::Update(eae6320::Graphics::UI::Right);
				}
				else
				{
					if (flyCamActive)
					{
						//code for fly cam
						UpdateEntities_floats();
					}

					else
					{
						if (eae6320::UserInput::IsKeyPressed(VK_LEFT) && currentWindow == s_mainWindow)
						{
							rotationOffset = -45;
						}
						else if (eae6320::UserInput::IsKeyPressed(VK_RIGHT) && currentWindow == s_mainWindow)
						{
							rotationOffset = 45;
						}
						else
							rotationOffset = 0;

						if( currentWindow == s_mainWindow)
							player->UpdateInput();

						if (player->boost)
						{
							if(player->boostValue <120)
							player->boostValue += 0.5f;
							//eae6320::Audio::StopAudio(5);
							eae6320::Audio::PlayAudio(8);
							
						}
						else
						{
							if(player->boostValue > 0)
							player->boostValue -= 0.1f;
							eae6320::Audio::StopAudio(8);
							
							
						}
						eae6320::Graphics::GameSpriteList[2]->UpdateHeight(0.0f, (int) player->boostValue);
						
						player->Update(eae6320::Time::GetSecondsElapsedThisFrame());
						
						checkFlagPosition(player, SecondPlayer);

						eae6320::Networking::SendPlayerPosition();
						eae6320::Networking::Update();
						
						playerCamera->eulerX = 5;
						playerCamera->eulerY += (player->eulerY + rotationOffset - playerCamera->eulerY) * eae6320::Time::GetSecondsElapsedThisFrame() * 3;

						player->UpdateCamera(playerCamera);

						Camera::getInstance().m_Position = playerCamera->Position;
						Camera::getInstance().m_PositionPlayer = player->Position;

						Camera::getInstance().m_PositionPlayerTwo = SecondPlayer->Position;

						Camera::getInstance().m_PositionPlayerRay = player->Position - player->getLocalZ() * 100;

						Camera::getInstance().m_PositionPlayerTwoRay = SecondPlayer->Position - SecondPlayer->getLocalZ() * 100;

						Camera::getInstance().m_Orientation = playerCamera->Orientation;
					
				}
			}
			
			//UpdateInputNumber(eae6320::Time::GetSecondsElapsedThisFrame());
			
			eae6320::Graphics::Render(eae6320::Time::GetSecondsElapsedThisFrame());
			}
		else
		{
			// If Windows _has_ sent a message, this iteration of the loop will handle it.
			// Note that Windows messages will take precedence over our game functionality;
			// this is because if we don't handle Windows messages the window can appear sluggish to the user
			// (if s/he tries to move it, for example, but we give too much precedence to our own game code).

			// First, the message must be "translated"
			// (Key presses are translated into character messages)
			TranslateMessage( &message );

			// Then, the message is sent on to the appropriate processing function.
			// This function is specified in the lpfnWndProc field of the WNDCLASSEX struct
			// used to register a class with Windows.
			// In the case of the main window in this example program
			// it will always be OnMessageReceived()
			DispatchMessage( &message );
		}
	} while ( message.message != WM_QUIT );

	eae6320::Graphics::ShutDown();

	// The exit code for the application is stored in the WPARAM of a WM_QUIT message
	o_exitCode = static_cast<int>( message.wParam );

	return true;
}

void checkFlagPosition(eae6320::Core::Player * i_player1, eae6320::Core::Player * i_player2)
{
	
	eae6320::Math::cVector playerDistance = i_player1->Position - i_player2->Position;

	eae6320::Audio::SetVolume(5, playerDistance.GetLength(), 1000.0f, 100.0f);

	if (i_player1->team == 0)
	{
		eae6320::Math::cVector playerFlag = i_player1->Position - Camera::getInstance().m_Flag1;
		eae6320::Math::cVector flagCheckpoint = Camera::getInstance().m_Flag1 - Camera::getInstance().m_FlagEnd1;
		if (playerFlag.GetLength() < 100)
		{
			//pick up flag
			if(!i_player1->Flag)
				eae6320::Audio::PlayAudio(2);
			Camera::getInstance().m_Flag1 = i_player1->Position;
			i_player1->Flag = true;
			
		}

		if (flagCheckpoint.GetLength() < 100)
		{
			Camera::getInstance().m_Flag1 = *Flag1Position;
			Camera::getInstance().m_Flag2 = *Flag2Position;
			i_player1->score++;
			eae6320::Graphics::GameSpriteList[0]->Update(0.0f, i_player1->score);
			i_player1->Flag = false;
			i_player2->Flag = false;
			eae6320::Audio::PlayAudio(4);
			
		}

		if (playerDistance.GetLength() < 100)
		{
			//tagging
			Camera::getInstance().m_Flag1 = *Flag1Position;
			Camera::getInstance().m_Flag2 = *Flag2Position;
			eae6320::Networking::TagBitch();
			if (i_player1->Flag || i_player2->Flag)
				eae6320::Audio::PlayAudio(3);
			i_player1->Flag = false;
			i_player2->Flag = false;
			
		}

		if (i_player2->Flag)
		{
			Camera::getInstance().m_Flag2 = i_player2->Position;
		}
		else
		{
			Camera::getInstance().m_Flag2 = *Flag2Position;
		}
	}
	else
	{
		eae6320::Math::cVector playerFlag = i_player1->Position - Camera::getInstance().m_Flag2;
		eae6320::Math::cVector flagCheckpoint = Camera::getInstance().m_Flag2 - Camera::getInstance().m_FlagEnd2;
		if (playerFlag.GetLength() < 100)
		{
			//pick up flag
			if(!i_player1->Flag)
				eae6320::Audio::PlayAudio(2);
			Camera::getInstance().m_Flag2 = i_player1->Position;
			i_player1->Flag = true;
			
		}

		if (flagCheckpoint.GetLength() < 100)
		{
			Camera::getInstance().m_Flag2 = *Flag2Position;
			Camera::getInstance().m_Flag1 = *Flag1Position;
			i_player1->score++;
			eae6320::Graphics::GameSpriteList[0]->Update(0.0f, i_player1->score);
			i_player1->Flag = false;
			i_player2->Flag = false;
			eae6320::Audio::PlayAudio(4);
		}

		if (playerDistance.GetLength() < 100)
		{
			//tagging
			Camera::getInstance().m_Flag1 = *Flag1Position;
			Camera::getInstance().m_Flag2 = *Flag2Position;
			eae6320::Networking::TagBitch();
			if(i_player1->Flag || i_player2->Flag)
				eae6320::Audio::PlayAudio(3);
			i_player1->Flag = false;
			i_player2->Flag = false;
			
		}

		if (i_player2->Flag)
		{
			Camera::getInstance().m_Flag1 = i_player2->Position;
		}
		else
		{
			Camera::getInstance().m_Flag1 = *Flag1Position;
		}
	}

	eae6320::Graphics::GameSpriteList[1]->Update(0.0f, i_player2->score);
	
	

}

void ResetFlag() 
{
	Camera::getInstance().m_Flag1 = *Flag1Position;
	Camera::getInstance().m_Flag2 = *Flag2Position;
}

bool checkObjectShoot()
{
	if (eae6320::UserInput::IsKeyPressed(VK_SPACE))
	{

		if (eae6320::Graphics::RenderableList[2]->m_position.x-1 < eae6320::Graphics::RenderableList[4]->m_position.x &&
			eae6320::Graphics::RenderableList[2]->m_position.x+1 > eae6320::Graphics::RenderableList[4]->m_position.x &&
			eae6320::Graphics::RenderableList[2]->m_position.y - 1 < eae6320::Graphics::RenderableList[4]->m_position.y &&
			eae6320::Graphics::RenderableList[2]->m_position.y + 1 > eae6320::Graphics::RenderableList[4]->m_position.y		)
		{
			//reduce alpha transparency
			eae6320::Graphics::RenderableList[2]->m_Material.uniformArray[1].values[0] -= 0.1f;
 			return true;
		}
		if (eae6320::Graphics::RenderableList[3]->m_position.x - 1 < eae6320::Graphics::RenderableList[4]->m_position.x &&
			eae6320::Graphics::RenderableList[3]->m_position.x + 1 > eae6320::Graphics::RenderableList[4]->m_position.x &&
			eae6320::Graphics::RenderableList[3]->m_position.y - 1 < eae6320::Graphics::RenderableList[4]->m_position.y &&
			eae6320::Graphics::RenderableList[3]->m_position.y + 1 > eae6320::Graphics::RenderableList[4]->m_position.y)
		{
			//reduce alpha transparency
			eae6320::Graphics::RenderableList[3]->m_Material.uniformArray[1].values[0] -= 0.1f;
   			return true;
		}
	}
	return true;
}

void UpdateInputNumber(float i_gameTime)
{
	if (eae6320::UserInput::IsKeyPressed('0'))
	{
		eae6320::Graphics::GameSpriteList[1]->Update(0.0f, 0);
	}
	if (eae6320::UserInput::IsKeyPressed('1'))
	{
		eae6320::Graphics::GameSpriteList[1]->Update(0.0f, 1);
	}
	if (eae6320::UserInput::IsKeyPressed('2'))
	{
		eae6320::Graphics::GameSpriteList[1]->Update(0.0f, 2);
	}
	if (eae6320::UserInput::IsKeyPressed('3'))
	{
		eae6320::Graphics::GameSpriteList[1]->Update(0.0f, 3);
	}
}

bool UpdateEntities_floats()
{
	bool wereThereErrors = false;

	// The following is an array of two floats,
	// but defined as a struct so that I can reference them with the human-readable "x" and "y"
	// rather than the less-descriptive [0] and [1]:
	struct 
	{
		float x, y, z;
	}offset;

	float rotationSpeed = 1.0f; //in degrees

	eae6320::Math::cVector cameraDir;

	eae6320::Math::cQuaternion * cameraOrientationLeft = new eae6320::Math::cQuaternion(eae6320::Math::ConvertDegreesToRadians(rotationSpeed), eae6320::Math::cVector(0,1,0));
	eae6320::Math::cQuaternion * cameraOrientationRight = new eae6320::Math::cQuaternion(eae6320::Math::ConvertDegreesToRadians(-rotationSpeed), eae6320::Math::cVector(0, 1, 0));
	eae6320::Math::cQuaternion * cameraOrientationUp = new eae6320::Math::cQuaternion(eae6320::Math::ConvertDegreesToRadians(rotationSpeed), eae6320::Math::cVector(1, 0, 0));
	eae6320::Math::cQuaternion * cameraOrientationDown = new eae6320::Math::cQuaternion(eae6320::Math::ConvertDegreesToRadians(-rotationSpeed), eae6320::Math::cVector(1, 0, 0));

	offset.x = offset.y = 0.0f;
	{
		// Get the direction
		{
			if (eae6320::UserInput::IsKeyPressed(VK_LEFT))
			{
				offset.x -= 2.2f;
			}
			if (eae6320::UserInput::IsKeyPressed(VK_RIGHT))
			{
				offset.x += 2.2f;
			}
			if (eae6320::UserInput::IsKeyPressed(VK_UP))
			{
				offset.y += 2.2f;
			}
			if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
			{
				offset.y -= 2.2f;
			}
		}
		// Get the speed
		const float unitsPerSecond = 1.0f;	// This is arbitrary
		const float unitsToMove = unitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent
																								// Normalize the offset
		offset.x *= unitsToMove;
		offset.y *= unitsToMove;
	}
	// The following line assumes there is some "entity" for the rectangle that the game code controls
	// that encapsulates a mesh, an effect, and a position offset.
	// You don't have to do it this way for your assignment!
	// You just need a way to update the position offset associated with the colorful rectangle.
	
	//eae6320::Graphics::RenderableList[0]->m_position.x += offset.x;
	//eae6320::Graphics::RenderableList[0]->m_position.y += offset.y;

	float distToMove = 5.0f;


	eae6320::Math::cQuaternion cameraOrientation = Camera::getInstance().m_Orientation;
	{
		if (eae6320::UserInput::IsKeyPressed('A'))	
		{
			Camera::getInstance().m_Orientation = Camera::getInstance().m_Orientation * (*cameraOrientationRight);
		}
		if (eae6320::UserInput::IsKeyPressed('D'))
		{
			Camera::getInstance().m_Orientation = Camera::getInstance().m_Orientation * (*cameraOrientationLeft);
		}
 		if (eae6320::UserInput::IsKeyPressed('W'))
		{
			cameraDir.z -= distToMove;
		}
		
		if (eae6320::UserInput::IsKeyPressed('S'))
		{
			cameraDir.z += distToMove;

		}
		if (eae6320::UserInput::IsKeyPressed(VK_UP))
		{
			cameraDir.y += distToMove;
		}
		if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
		{
			cameraDir.y -= distToMove;
		}
	}

	

	//update camera

	const float cameraUnitsPerSecond = 1.0f;	// This is arbitrary
	const float camUnitsToMove = cameraUnitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent
																							// Normalize the offset
	//cameraDir.x *= camUnitsToMove;
	cameraDir.y *= camUnitsToMove;

	eae6320::Math::cMatrix_transformation FlyCamPosition(Camera::getInstance().m_Orientation, Camera::getInstance().m_Position);

	Camera::getInstance().m_Position.x += FlyCamPosition.m_00 * cameraDir.x + FlyCamPosition.m_10 * cameraDir.y + FlyCamPosition.m_20 * cameraDir.z + FlyCamPosition.m_30 * 0;
	Camera::getInstance().m_Position.y += FlyCamPosition.m_01 * cameraDir.x + FlyCamPosition.m_11 * cameraDir.y + FlyCamPosition.m_21 * cameraDir.z + FlyCamPosition.m_31 * 0;
	Camera::getInstance().m_Position.z += FlyCamPosition.m_02 * cameraDir.x + FlyCamPosition.m_12 * cameraDir.y + FlyCamPosition.m_22 * cameraDir.z + FlyCamPosition.m_32 * 0;

	/*eae6320::Graphics::RenderableList[4]->m_position += cameraDir;
	eae6320::Graphics::RenderableList[5]->m_position += cameraDir;*/
	
	return !wereThereErrors;
}
