#pragma once

#include "Rect.h"
#include "Surface.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "Button.h"
#include "MainWindow.h"
#include "ImageHandler.h"

class FileMenu
{
public:
	FileMenu( const RectI& screenArea,Surface& art,
		MainWindow& wnd );

	bool Update( const Mouse& mouse,const Keyboard& kbd,ImageHandler& imgHand );
	void Draw( Graphics& gfx ) const;
private:
	const RectI& screenArea;
	Surface& art;
	MainWindow& wnd;

	Button open = Button{ Surface{ { "Icons/OpenButton.bmp" },Vei2{ 3,3 } },
		Vei2{ screenArea.right + 5,screenArea.top + 5 } };
	// Button save = Button{ Surface{ { "Icons/SaveButton.bmp" },Vei2{ 3,3 } },
	// 	Vei2{ screenArea.right + 5,screenArea.top + 24 + 5 } };
	Button save = Button{ Surface{ { "Icons/SaveButton.bmp" },Vei2{ 3,3 } },
		Vei2{ screenArea.right + 5 * 2 + 24 * 3,screenArea.top + 5 } };
};