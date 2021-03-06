#pragma once

#include "Colors.h"
#include <string>
#include "Rect.h"
#include <vector>

class Surface
{
public:
	// Create blank surface with width and height.
	Surface( int width,int height );
	// Load a bitmap(.bmp) file with filename into surface.
	Surface( const std::string& filename );
	// Create a new surface from a clip of other.
	Surface( const Surface& other,const RectI& clip );
	// Create a new surface that is other expanded by expandedSize.
	Surface( const Surface& other,const Vei2& expandSize );
	// Create a new surface that is other but flipped over y or x axis.
	Surface( const Surface& other,bool xFlipped,bool yFlipped );
public:

	Surface( const Surface& ) = default;
	Surface& operator=( const Surface& ) = default;

	Surface( Surface&& donor );
	Surface& operator=( Surface&& rhs );

	void PutPixel( int x,int y,Color c );
	void DrawRect( int x,int y,int width,int height,Color c );
	void DrawLine( Vec2 p0,Vec2 p1,Color c );
	// Copies other surf into this one, even if it's smaller.
	void CopyInto( const Surface& other );
	// Copies other surf's pixels into my magenta pixels.
	void LightCopyInto( const Surface& other );
	void LightCopyIntoPos( const Surface& other,const Vei2& pos );
	void Resize( const Vei2& newSize );
	// Copies other surf into this one at specified pos.
	void CopyIntoPos( const Surface& other,const Vei2& pos );

	Color GetPixel( int x,int y ) const;
	int GetWidth() const;
	int GetHeight() const;
	Vei2 GetSize() const;
	RectI GetRect() const;
	RectI GetNonMagentaRect() const;

	// Expand a surface by amount.
	Surface GetExpandedBy( const Vei2& amount ) const;
	// Bilinearly interpolate a surface to be width wide and height high.
	Surface GetInterpolatedTo( int width,int height ) const;
	// Get a surface flipped over the y axis.
	Surface GetXReversed() const;
	// Get a surface flipped over the x axis.
	Surface GetYReversed() const;
	// Get a clipped area from a surface.
	Surface GetClipped( const RectI& clip ) const;
	// Get the cropped area of a surface.
	Surface GetCropped( const Vei2& cropStart,const Vei2& cropEnd );

	const std::vector<Color>& GetRawPixelData() const;

	bool operator!=( const Surface& rhs ) const
	{
		if( width != rhs.width || height != rhs.height ) return( false );

		for( int i = 0; i < int( pixels.size() ); ++i )
		{
			if( pixels[i] != rhs.pixels[i] )
			{
				return( true );
			}
		}
		return( false );
	}
private:
	std::vector<Color> pixels;
	int width;
	int height;
};