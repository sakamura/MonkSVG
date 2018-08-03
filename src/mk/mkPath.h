/*
 BSD 3-Clause License - Please see LICENSE file for full license
 */

#ifndef __mkPath_h__
#define __mkPath_h__

#include "mkMath.h"

#include "mkPaint.h"
#include <tess3/tess.h>
#include <OpenGLES/ES2/gl.h>
#include "sakaDefs.h"

namespace MonkSVG {
    class MKSVGHandler;
}

namespace MonkVG {
    class MKPath {
	public:
        void clear( GLbitfield caps );
		
		inline int getNumSegments() const {
			return _numSegments;
		}
		inline void setNumSegments( const int ns ) {
			_numSegments = ns;
		}
		
		inline int getNumCoords() const {
			return _numCoords;
		}
		inline void setNumCoords( const int nc ) {
			_numCoords = nc;
		}
		
		// bounds
		inline float getMinX() {
			return _minX;
		}
		inline float getMinY() {
			return _minY;
		}
		inline float getWidth() {
			return _width;
		}
		inline float getHeight() {
			return _height;
		}
		
		//// internal data manipulators ////
        bool draw( GLbitfield paintModes );
		void appendData( const int numSegments, const unsigned char * pathSegments, const float * pathData ) ;
		int32_t segmentToNumCoordinates(VGPathSegment segment);
		void copy( const MKPath& src, const Matrix33& transform );
		void buildFillIfDirty();
		

        MKPath( MonkSVG::MKSVGHandler* h )
		:   _handler(h)
		,	_numSegments( 0 )
		,	_numCoords( 0 )
        ,	_minX( std::numeric_limits<float>::max() )
		,	_minY( std::numeric_limits<float>::max() )
		,	_width( -1 )
		,	_height( -1 )
        ,	_fillTesselator( 0 )
        ,   _fcoords(new Saka::vector<float>)
		{
		}
		
        ~MKPath();
        
        
	protected:
		int				_numSegments;	// VG_PATH_NUM_SEGMENTS
		int				_numCoords;		// VG_PATH_NUM_COORDS
		
		// data
		Saka::vector< unsigned char >	_segments;
		Saka::vector< float >	*_fcoords;
		
		// bounds
		float				_minX;
		float				_minY;
		float				_height;
		float				_width;

    private:
        MonkSVG::MKSVGHandler* _handler;
        
        struct textured_vertex_t {
            GLfloat		v[2];
            GLfloat		uv[2];
        };
        
    private:
        struct TessOptions : public Tess::DefaultOptions
        {
            using Coord = GLfloat; // wet dream: u_int16_t;
            struct Vec                      // One vector comprised of Coords.
            {
                Coord x;
                Coord y;
            };
        };
        using Tesselator = Tess::Tesselator<TessOptions, Tess::BaseAllocators<TessOptions> >;
        Tesselator*		_fillTesselator;
        Saka::vector<GLfloat>		_vertices;
        Saka::vector<v2_t>		_strokeVertices;
        int					_numberFillVertices;
        int					_numberStrokeVertices;
        MKPaint*		_fillPaintForPath;
        MKPaint*		_strokePaintForPath;

        
    private:		// tesseleator callbacks
        void endOfTesselation( GLbitfield paintModes );
        
    private:	// utility methods
        
        void updateBounds(float x, float y) {
            _minX = std::min(_minX, x);
            _width = std::max(_width, x);
            _minY = std::min(_minY, y);
            _height = std::max(_height, y);
        }
        
        void addVertex( float* v ) {
            float x = (float)v[0];
            float y = (float)v[1];
            updateBounds(x, y);
            _vertices.push_back(x);
            _vertices.push_back(y);
        }
        
        void buildFill();
        void buildStroke();
        void buildFatLineSegment( Saka::vector<v2_t>& vertices, const v2_t& p0, const v2_t& p1, const float stroke_width );
                
    };
}
#endif //__mkPath_h__
