#include "LayerManager.h"

LayerManager::LayerManager( const RectI& clipArea,const Vei2& canvSize )
	:
	canvSize( canvSize ),
	drawArea( clipArea.right + padding.x,
		Graphics::ScreenWidth - padding.x,
		clipArea.bottom - 237,clipArea.bottom )
{
	layers.emplace_back( Surface{ canvSize.x,canvSize.y } );
	layers.back().DrawRect( 0,0,canvSize.x,canvSize.y,Colors::Magenta );

	const auto start = Vei2{ drawArea.left + padding.x,
		drawArea.top + padding.y };
	for( int i = 0; i < 7; ++i )
	{
		layerButtons.emplace_back( Button{ Surface{ Surface{
			"Icons/LayerButton.bmp" },Vei2{ 3,3 } },
			start + ( padding.Y() + buttonSize.Y() ) * i } );
	}
}

void LayerManager::Update( const Keyboard& kbd,const Mouse& mouse,Surface& art )
{
	if( addLayer.Update( mouse ) ||
		( kbd.KeyIsPressed( VK_CONTROL ) &&
		kbd.KeyIsPressed( 'N' ) ) )
	{
		if( layers.size() < 7 )
		{
			layers.emplace_back( Surface{ canvSize.x,canvSize.y } );
			layers.back().DrawRect( 0,0,canvSize.x,canvSize.y,Colors::Magenta );
			++selectedLayer;
		}
	}
	if( dupeLayer.Update( mouse ) ||
		( kbd.KeyIsPressed( VK_CONTROL ) &&
		kbd.KeyIsPressed( 'J' ) ) )
	{
		if( layers.size() < 7 )
		{
			layers.push_back( layers.back() );
			++selectedLayer;
		}
	}
	if( removeLayer.Update( mouse ) ||
		kbd.KeyIsPressed( VK_DELETE ) )
	{
		if( layers.size() > 1 )
		{
			layers.pop_back();
			--selectedLayer;
		}
	}

	for( int i = 0; i < int( layers.size() ); ++i )
	{
		if( layerButtons[i].Update( mouse ) )
		{
			selectedLayer = i;
		}
	}
}

void LayerManager::Draw( Graphics& gfx ) const
{
	// TODO: Remove this ----------------------------------
	gfx.DrawRect( drawArea.left,drawArea.top,
		drawArea.GetWidth(),drawArea.GetHeight(),
		Colors::DarkGray );
	// ----------------------------------------------------
	
	for( int i = 0; i < int( layers.size() ); ++i )
	{
		layerButtons[i].Draw( gfx );
	}

	addLayer.Draw( gfx );
	dupeLayer.Draw( gfx );
	removeLayer.Draw( gfx );
}

void LayerManager::ResizeCanvas( const Vei2& newSize )
{
	canvSize = newSize;

	for( auto& layer : layers )
	{
		Surface temp = layer;
		layer = Surface{ newSize.x,newSize.y };
		layer.CopyInto( temp );
	}
}