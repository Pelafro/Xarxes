#pragma once
#include <vector>

namespace OpenGLEngine {

	class IApp;		// Forward declaration of the main game app interface
	class IScreen;	// Forward declaration of the game screen interface

	// Screen list class to store the whole game screens
	class ScreenList {
	protected:
		std::vector<IScreen*> m_screens;	// Array of screens
		IApp* m_gameApp;					// Reference pointer to current game app	
		int m_currentScreenIndex;			// Index of the current running screen

		// Get the current screen object
		IScreen* GetCurScreen() const;
	public:
		explicit ScreenList(IApp* game);
		~ScreenList();

		IScreen* SetScreen(int index);
		void AddScreen(IScreen* newScreen, int index);
		void Destroy();
	};

}