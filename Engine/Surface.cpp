#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>
#include "Graphics.h"

Surface::Surface( int width,int height ) :
	width( width ),
	height( height ),
	pixels( width * height )
{}

Surface::Surface( const std::string& filename )
{
	std::ifstream file( filename,std::ios::binary );
	assert( file );

	BITMAPFILEHEADER bmFileHeader;
	file.read( reinterpret_cast< char* >( &bmFileHeader ),
		sizeof( bmFileHeader ) );

	BITMAPINFOHEADER bmInfoHeader;
	file.read( reinterpret_cast< char* >( &bmInfoHeader ),
		sizeof( bmInfoHeader ) );

	assert( bmInfoHeader.biBitCount == 24 ||
		bmInfoHeader.biBitCount == 32 );
	assert( bmInfoHeader.biCompression == BI_RGB );

	const bool is32b = bmInfoHeader.biBitCount == 32;

	width = bmInfoHeader.biWidth;

	// Test for reverse row order and
	//  control y loop accordingly.
	int yStart;
	int yEnd;
	int dy;
	if( bmInfoHeader.biHeight < 0 )
	{
		height = -bmInfoHeader.biHeight;
		yStart = 0;
		yEnd = height;
		dy = 1;
	}
	else
	{
		height = bmInfoHeader.biHeight;
		yStart = height - 1;
		yEnd = -1;
		dy = -1;
	}

	pixels.resize( width * height );

	file.seekg( bmFileHeader.bfOffBits );
	// Padding is for 24 bit depth only.
	const int padding = ( 4 - ( width * 3 ) % 4 ) % 4;

	for( int y = yStart; y != yEnd; y += dy )
	{
		for( int x = 0; x < width; ++x )
		{
			PutPixel( x,y,Color( file.get(),file.get(),file.get() ) );
			if( is32b )
			{
				file.seekg( 1,std::ios::cur );
			}
		}
		if( !is32b )
		{
			file.seekg( padding,std::ios::cur );
		}
	}
}

Surface::Surface( const Surface& other,const RectI& clip )
{
	*this = other.GetClipped( clip );
}

Surface::Surface( const Surface& other,const Vei2& expandSize )
{
	*this = other.GetExpandedBy( expandSize );
}

Surface::Surface( const Surface& other,bool xFlipped,bool yFlipped )
{
	Surface guineaPig = { other.GetWidth(),other.GetHeight() };

	if( xFlipped ) guineaPig = other.GetXReversed();

	if( yFlipped ) guineaPig = guineaPig.GetYReversed();

	*this = guineaPig;
}

Surface::Surface( Surface&& donor )
{
	*this = std::move( donor );
}

Surface& Surface::operator=( Surface&& rhs )
{
	width = rhs.width;
	height = rhs.height;
	pixels = std::move( rhs.pixels );

	rhs.width = 0;
	rhs.height = 0;

	return( *this );
}

void Surface::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < width );
	assert( y >= 0 );
	assert( y < height );
	pixels.data()[y * width + x] = c;
}

void Surface::DrawRect( int x,int y,int width,int height,Color c )
{
	for( int i = y; i < y + height; ++i )
	{
		for( int j = x; j < x + width; ++j )
		{
			PutPixel( j,i,c );
		}
	}
}

void Surface::DrawLine( Vec2 p0,Vec2 p1,Color c )
{
	float m = 0.0f;
	if( p1.x != p0.x )
	{
		m = ( p1.y - p0.y ) / ( p1.x - p0.x );
	}

	if( p1.x != p0.x && std::abs( m ) <= 1.0f )
	{
		if( p0.x > p1.x )
		{
			std::swap( p0,p1 );
		}

		const float b = p0.y - m * p0.x;

		for( int x = int( p0.x ); x < int( p1.x ); x++ )
		{
			const float y = m * float( x ) + b;
			PutPixel( x,int( y ),c );
		}
	}
	else
	{
		if( p0.y > p1.y )
		{
			std::swap( p0,p1 );
		}

		const float w = ( p1.x - p0.x ) / ( p1.y - p0.y );
		const float p = p0.x - w * p0.y;

		for( int y = int( p0.y ); y < int( p1.y ); y++ )
		{
			const float x = w * float( y ) + p;
			PutPixel( int( x ),y,c );
		}
	}
}

void Surface::CopyInto( const Surface& other )
{
	// Don't copy over pixels we don't have or
	//  into pixels that don't exist.
	const int minWidth = std::min( GetWidth(),other.GetWidth() );
	const int minHeight = std::min( GetHeight(),other.GetHeight() );

	for( int y = 0; y < minHeight; ++y )
	{
		for( int x = 0; x < minWidth; ++x )
		{
			PutPixel( x,y,other.GetPixel( x,y ) );
		}
	}
}

void Surface::LightCopyInto( const Surface& other )
{
	// Don't copy over pixels we don't have or
	//  into pixels that don't exist.
	const int minWidth = std::min( GetWidth(),other.GetWidth() );
	const int minHeight = std::min( GetHeight(),other.GetHeight() );

	for( int y = 0; y < minHeight; ++y )
	{
		for( int x = 0; x < minWidth; ++x )
		{
			if( GetPixel( x,y ) == Colors::Magenta )
			{
				PutPixel( x,y,other.GetPixel( x,y ) );
			}
		}
	}
}

void Surface::LightCopyIntoPos( const Surface& other,const Vei2& pos )
{
	for( int y = pos.y; y < pos.y + other.GetHeight(); ++y )
	{
		for( int x = pos.x; x < pos.x + other.GetWidth(); ++x )
		{
			if( other.GetPixel( x - pos.x,y - pos.y ) != Colors::Magenta )
			{
				PutPixel( x,y,other.GetPixel( x - pos.x,y - pos.y ) );
			}
		}
	}
}

void Surface::Resize( const Vei2& newSize )
{
	Surface temp = *this;
	// const auto oldSize = GetSize();
	pixels.resize( newSize.x * newSize.y );

	width = newSize.x;
	height = newSize.y;

	DrawRect( 0,0,width,height,Colors::Magenta );
	CopyInto( temp );

	// for( int y = oldSize.y; y < height; ++y )
	// {
	// 	for( int x = oldSize.x; x < width; ++x )
	// 	{
	// 		PutPixel( x,y,Colors::Magenta );
	// 	}
	// }
}

void Surface::CopyIntoPos( const Surface& other,const Vei2& pos )
{
	const int minWidth = std::min( width,pos.x + other.width );
	const int minHeight = std::min( height,pos.y + other.height );

	for( int y = pos.y; y < minHeight; ++y )
	{
		for( int x = pos.x; x < minWidth; ++x )
		{
			PutPixel( x,y,other.GetPixel( x - pos.x,y - pos.y ) );
		}
	}
}

Color Surface::GetPixel( int x,int y ) const
{
	assert( x >= 0 );
	assert( x < width );
	assert( y >= 0 );
	assert( y < height );
	return pixels.data()[y * width + x];
}

int Surface::GetWidth() const
{
	return width;
}

int Surface::GetHeight() const
{
	return height;
}

Vei2 Surface::GetSize() const
{
	return Vei2{ width,height };
}

RectI Surface::GetRect() const
{
	return{ 0,width,0,height };
}

RectI Surface::GetNonMagentaRect() const
{
	RectI temp = { -1,-1,-1,-1 };

	// Set rect top.
	for( int y = 0; y < height; ++y )
	{
		for( int x = 0; x < width; ++x )
		{
			if( GetPixel( x,y ) != Colors::Magenta )
			{
				temp.top = y;
				break;
			}
		}
		if( temp.top != -1 ) break;
	}

	// Set rect bot.
	for( int y = height - 1; y >= 0; --y )
	{
		for( int x = 0; x < width; ++x )
		{
			if( GetPixel( x,y ) != Colors::Magenta )
			{
				temp.bottom = y;
				break;
			}
		}
		if( temp.bottom != -1 ) break;
	}

	// Set rect left.
	for( int x = 0; x < width; ++x )
	{
		for( int y = 0; y < height; ++y )
		{
			if( GetPixel( x,y ) != Colors::Magenta )
			{
				temp.left = x;
				break;
			}
		}
		if( temp.left != -1 ) break;
	}

	// Set rect right.
	for( int x = width - 1; x >= 0; --x )
	{
		for( int y = 0; y < height; ++y )
		{
			if( GetPixel( x,y ) != Colors::Magenta )
			{
				temp.right = x;
				break;
			}
		}
		if( temp.right != -1 ) break;
	}

	return( temp );
}

Surface Surface::GetExpandedBy( const Vei2& amount ) const
{
	Surface bigger = { amount.x * GetWidth(),amount.y * GetHeight() };

	for( int y = 0; y < this->height; ++y )
	{
		for( int x = 0; x < this->width; ++x )
		{
			bigger.DrawRect( x * amount.x,y * amount.y,
				int( amount.x ),int( amount.y ),
				GetPixel( x,y ) );
		}
	}

	return( bigger );
}

// https://rosettacode.org/wiki/Bilinear_interpolation helped a lot with this conversion code.
Surface Surface::GetInterpolatedTo( int width,int height ) const
{
	const int newWidth = width;
	const int newHeight = height;
	Surface newImage = Surface( newWidth,newHeight );
	for( int x = 0; x < newWidth; ++x )
	{
		for( int y = 0; y < newHeight; ++y )
		{
			const float gx = ( float( x ) ) / newWidth * ( GetWidth() - 1 );
			const float gy = ( float( y ) ) / newHeight * ( GetHeight() - 1 );
			const int gxi = int( gx );
			const int gyi = int( gy );
			int rgb = 0;
			const int c00 = GetPixel( gxi,gyi ).dword;
			const int c10 = GetPixel( gxi + 1,gyi ).dword;
			const int c01 = GetPixel( gxi,gyi + 1 ).dword;
			const int c11 = GetPixel( gxi + 1,gyi + 1 ).dword;
			for( int i = 0; i <= 2; ++i )
			{
				const int b00 = ( c00 >> ( i * 8 ) ) & 0xFF;
				const int b10 = ( c10 >> ( i * 8 ) ) & 0xFF;
				const int b01 = ( c01 >> ( i * 8 ) ) & 0xFF;
				const int b11 = ( c11 >> ( i * 8 ) ) & 0xFF;
				const int ble = ( int( Vec2
					::Blerp( float( b00 ),float( b10 ),
						float( b01 ),float( b11 ),
						gx - float( gxi ),gy - float( gyi ) ) ) )
					<< ( 8 * i );
				rgb = rgb | ble;
			}
			newImage.PutPixel( x,y,rgb );
		}
	}
	return newImage;
}

Surface Surface::GetXReversed() const
{
	Surface flipped = Surface{ width,height };

	for( int y = 0; y < height; ++y )
	{
		int otherX = 0;
		// Loop in reverse x.
		for( int x = width - 1; x >= 0; --x )
		{
			flipped.PutPixel( otherX,y,GetPixel( x,y ) );
			++otherX;
		}
	}

	return( flipped );
}

Surface Surface::GetYReversed() const
{
	Surface flipped = Surface{ width,height };

	int otherY = 0;
	// Loop in reverse y.
	for( int y = height - 1; y >= 0; --y )
	{
		for( int x = width - 1; x >= 0; --x )
		{
			flipped.PutPixel( x,otherY,GetPixel( x,y ) );
		}
		++otherY;
	}

	return( flipped );
}

Surface Surface::GetClipped( const RectI& clip ) const
{
	Surface clipped = { clip.GetWidth(),clip.GetHeight() };

	int i = 0;
	int j = 0;
	for( int y = clip.top; y < clip.bottom; ++y )
	{
		for( int x = clip.left; x < clip.right; ++x )
		{
			clipped.PutPixel( j,i,GetPixel( x,y ) );
			++j;
		}
		j = 0;
		++i;
	}

	return( clipped );
}

Surface Surface::GetCropped( const Vei2& cropStart,const Vei2& cropEnd )
{
	Surface temp{ cropEnd.x - cropStart.x,cropEnd.y - cropStart.y };

	for( int y = cropStart.y; y < cropEnd.y; ++y )
	{
		for( int x = cropStart.x; x < cropEnd.x; ++x )
		{
			temp.PutPixel( x - cropStart.x,y - cropStart.y,
				GetPixel( x,y ) );
		}
	}

	return( temp );
}

const std::vector<Color>& Surface::GetRawPixelData() const
{
	return( pixels );
}
