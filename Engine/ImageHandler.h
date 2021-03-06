#pragma once

#include "Surface.h"
#include "Rect.h"
#include "Vec2.h"
#include "Graphics.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "ToolMode.h"
#include "Font.h"
#include "LayerManager.h"

class ImageHandler
{
public:
	ImageHandler( const RectI& clipArea,ToolMode& curTool,
		Mouse& mouse,Keyboard& kbd );

	// TODO: Heckin make this whole method cleaner.
	void Update( const Keyboard& kbd,ToolMode tool,
		Color& main,Color& off );
	void Draw( Graphics& gfx ) const;

	void CenterImage();
	void UpdateArt();
	Surface& GetArt();
	void ResizeCanvas( const Vei2& newSize );
	void CreateNewLayer();
	void UpdateSelectArea();

	void DrawCursor( Graphics& gfx ) const;
	Surface GetLayeredArt() const;
private:
	// Recursive, call to fill until hitting "walls".
	void TryFillPlusAt( const Vei2& pos,Color c,Color baseFill );
private:
	Mouse& mouse;
	Keyboard& kbd;
	Vei2 canvSize = { 8,8 };
	Surface art;
	const RectI& clipArea;
	Vec2 artPos;
	Vec2 scale = { 10.0f,10.0f };
	static constexpr Color chroma = Colors::Magenta;

	Vei2 mousePos = { 0,0 };
	Vei2 oldMousePos = { 0,0 };
	bool clickingLastFrame = false;
	ToolMode& curTool;
	// static constexpr int bgGrainAmount = 10;
	Surface bgPattern;

	Surface drawSurf;
	Surface bigPattern;

	Vei2 cropStart = { 0,0 };
	bool canCrop = false;
	Vei2 cropEnd = { 0,0 };
	bool appliedCrop = true;

	Vei2 lastClickPos = { 0,0 };

	std::vector<int> xGuidelines;
	std::vector<int> yGuidelines;
	int tempGuideline = -9999;
	bool guidelineX = false;
	bool draggingRuler = false;

	const Surface miniHand = { Surface{ "Icons/MiniHand.bmp" },Vei2{ 3,3 } };
	const Surface miniZoomer = { Surface{ "Icons/MiniZoomer.bmp" },Vei2{ 3,3 } };
	const Surface miniBucket = { Surface{ "Icons/MiniBucket.bmp" },Vei2{ 3,3 } };
	const Surface miniSampler = { Surface{ "Icons/MiniSampler.bmp" },Vei2{ 3,3 } };
	const Surface miniResizer = { Surface{ "Icons/MiniResizer.bmp" },Vei2{ 3,3 } };
	const Surface miniRuler = { Surface{ "Icons/MiniRuler.bmp" },Vei2{ 3,3 } };
	const Surface miniPointer = { Surface{ "Icons/MiniPointer.bmp" },Vei2{ 3,3 } };
	const Surface miniSelector = { Surface{ "Icons/MiniSelector.bmp" },Vei2{ 3,3 } };

	const Font luckyPixel = "Fonts/LuckyPixel24x36.bmp";

	LayerManager layerManager; // How creative.
	bool hoveringLastFrame = false;
	RectI selectedLayerRect = { -1,-1,-1,-1 };

	Vei2 selectStart = { 0,0 };
	bool canSelect = false;
	Vei2 selectEnd = { 0,0 };
	bool selectingStuff = false;
	Surface clipboard = { 0,0 };
	Vei2 clipboardPos = { 0,0 };
	bool canPaste = false;

	bool draggingPointer = false;
	Vei2 pointerStart = { 0,0 };
	Vei2 pointerDelta = { 0,0 };
	Vei2 pointerPos = { 0,0 };
	Surface pointerMoveClip = { 0,0 };
	Surface miniPointerMoveClip = { 0,0 };
};