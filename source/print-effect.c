#include <stdlib.h> 
#include <stdint.h> 
#include <stdio.h> 
#include <string.h> 
#include <math.h> 
#include <sys/stat.h> 

#include "libs/stb_image.h"
#include "libs/stb_image_resize.h"
#include "libs/stb_image_write.h"
#include "libs/dir.h"

#include "printed.h"

void combine_k_lut( void ) {
    float* klut = (float*)malloc( 4096ull * 4096ull * sizeof( float ) );
    memset( klut, 0, 4096ull * 4096ull * sizeof( float ) );
    for( int i = 0; i < 15; ++i ) {
        char name[ 256 ];
        sprintf( name, "lut_src_data/k%02d.png", i + 1 );
        int w, h, c;
        uint8_t* k = stbi_load( name, &w, &h, &c, 1 );
        for( int y = 0; y < 4096; ++y ) {
            for( int x = 0; x < 4096; ++x ) {
                klut[ x + 4096 * y ] += ( 255 - k[ x + 4096 * y ] ) / 255.0f;
            }
        }
        free( k );
    }

    uint8_t* k = (uint8_t*)malloc( 4096ull * 4096ull );
    memset( k, 0, 4096ull * 4096ull );
    for( int y = 0; y < 4096; ++y ) {
        for( int x = 0; x < 4096; ++x ) {
            float f = klut[ x + 4096 * y ] ;
            int v = ( f * 255.0f );
            v = v < 0 ? 0 : v > 255 ? 255 : v;
            k[ x + 4096 * y ]  = (uint8_t)( 255 - v );
        }
    }
    free( klut );
    stbi_write_png( "klut.png", 4096, 4096, 1, k, 4096 );
    free( k );
}


void rgb_to_k( char const* in, char const* out ) {
    int w, h, c;
    uint8_t* klut = (uint8_t*) stbi_load( "klut.png", &w, &h, &c, 1 );
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &c, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = img[ x + y * w ] & 0xffffff;
            uint32_t k = klut[ i ];
            img[ x + y * w ] = 0xff000000 | ( k << 16 ) | ( k << 8 ) | k;
        }
    }
    free( klut );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}



int32_t lerp( int32_t a, int32_t b, int32_t i ) {
    return a + ( ( b - a ) * i ) / 255;
}


uint32_t rgblerp( uint32_t color1, uint32_t color2, uint8_t alpha )	{
    uint64_t c1 = (uint64_t) color1;
    uint64_t c2 = (uint64_t) color2;
    uint64_t a = (uint64_t)( alpha );
    // bit magic to alpha blend R G B with single mul
    c1 = ( c1 | ( c1 << 24 ) ) & 0x00ff00ff00ffull;
    c2 = ( c2 | ( c2 << 24 ) ) & 0x00ff00ff00ffull;
    uint64_t o = ( ( ( ( c2 - c1 ) * a ) >> 8 ) + c1 ) & 0x00ff00ff00ffull; 
    return (uint32_t) ( o | ( o >> 24 ) );
}


#define RGBMUL32( a, b) \
    ( ( ( ( ( ( (a) >> 16U ) & 0xffU ) * ( ( (b) >> 16U ) & 0xffU ) ) >> 8U ) << 16U ) | \
        ( ( ( ( ( (a) >> 8U  ) & 0xffU ) * ( ( (b) >> 8U  ) & 0xffU ) ) >> 8U ) << 8U  ) | \
        ( ( ( ( ( (a)        ) & 0xffU ) * ( ( (b)        ) & 0xffU ) ) >> 8U )        ) )

void rgb_to_cmy( char const* in, char const* out ) {
    int w, h, t;
    uint8_t* clut = (uint8_t*) stbi_load( "lut_src_data/c.png", &w, &h, &t, 1 );
    uint8_t* mlut = (uint8_t*) stbi_load( "lut_src_data/m.png", &w, &h, &t, 1 );
    uint8_t* ylut = (uint8_t*) stbi_load( "lut_src_data/y.png", &w, &h, &t, 1 );
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = img[ x + y * w ] & 0xffffff;
            uint32_t cc = clut[ i ];
            uint32_t cm = mlut[ i ];
            uint32_t cy = ylut[ i ];
            uint32_t cmy = ( cc << 16 ) | ( cm << 8 ) | cy;
            img[ x + y * w ] = 0xff000000 | ( cmy & 0xffffff );
        }
    }
    free( clut );
    free( mlut );
    free( ylut );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}


void rgb_to_cmy_dings( char const* in, char const* out ) {
    int w, h, t;
    uint8_t* dingsc = (uint8_t*) stbi_load( "lut_src_data/dings1.png", &w, &h, &t, 1 );
    uint8_t* dingsm = (uint8_t*) stbi_load( "lut_src_data/dings2.png", &w, &h, &t, 1 );
    uint8_t* dingsy = (uint8_t*) stbi_load( "lut_src_data/dings3.png", &w, &h, &t, 1 );
    uint8_t* clut = (uint8_t*) stbi_load( "lut_src_data/c.png", &w, &h, &t, 1 );
    uint8_t* mlut = (uint8_t*) stbi_load( "lut_src_data/m.png", &w, &h, &t, 1 );
    uint8_t* ylut = (uint8_t*) stbi_load( "lut_src_data/y.png", &w, &h, &t, 1 );
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = img[ x + y * w ] & 0xffffff;
            uint32_t cc = clut[ i ];
            uint32_t cm = mlut[ i ];
            uint32_t cy = ylut[ i ];
            cc = lerp( cc, ( cc * 128 ) / 255, dingsc[ x + y * w ] / 8 );
            cm = lerp( cm, ( cm * 128 ) / 255, dingsm[ x + y * w ] / 8 );
            cy = lerp( cy, ( cy * 128 ) / 255, dingsy[ x + y * w ] / 8 );
            uint32_t cmy = ( cc << 16 ) | ( cm << 8 ) | cy;
            img[ x + y * w ] = 0xff000000 | ( cmy & 0xffffff );
        }
    }
    free( clut );
    free( mlut );
    free( ylut );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}


void rgb_to_cmy_halftone( char const* in, char const* out ) {
    int mw, mh, mt;
    uint8_t* c020 = (uint8_t*) stbi_load( "masks_src/32/c020.png", &mw, &mh, &mt, 1 );
    uint8_t* m020 = (uint8_t*) stbi_load( "masks_src/32/m020.png", &mw, &mh, &mt, 1 );
    uint8_t* y020 = (uint8_t*) stbi_load( "masks_src/32/y020.png", &mw, &mh, &mt, 1 );
    uint8_t* c050 = (uint8_t*) stbi_load( "masks_src/32/c050.png", &mw, &mh, &mt, 1 );
    uint8_t* m050 = (uint8_t*) stbi_load( "masks_src/32/m050.png", &mw, &mh, &mt, 1 );
    uint8_t* y050 = (uint8_t*) stbi_load( "masks_src/32/y050.png", &mw, &mh, &mt, 1 );
    uint8_t* c070 = (uint8_t*) stbi_load( "masks_src/32/c070.png", &mw, &mh, &mt, 1 );
    uint8_t* m070 = (uint8_t*) stbi_load( "masks_src/32/m070.png", &mw, &mh, &mt, 1 );
    uint8_t* y070 = (uint8_t*) stbi_load( "masks_src/32/y070.png", &mw, &mh, &mt, 1 );
    int w, h, t;
    uint8_t* klut = (uint8_t*) stbi_load( "klut.png", &w, &h, &t, 1 );
    uint8_t* clut = (uint8_t*) stbi_load( "lut_src_data/c.png", &w, &h, &t, 1 );
    uint8_t* mlut = (uint8_t*) stbi_load( "lut_src_data/m.png", &w, &h, &t, 1 );
    uint8_t* ylut = (uint8_t*) stbi_load( "lut_src_data/y.png", &w, &h, &t, 1 );
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    uint8_t* imgk = (uint8_t*)malloc( w * h );
    memset( imgk, 255, w * h );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            for( int i = -1; i <= 1; ++i ) {
                for( int j = -1; j <= 1; ++j ) {
                    uint8_t k = klut[ img[ x + y * w ] & 0xffffff ];
                    int xp = x + j;
                    int yp = y + i;
                    if( xp >= 0 && xp < w && yp >= 0 && yp < h ) {
                        uint32_t v = imgk[ xp + yp * w ] & 0xffffff; 
                        v = ( v * k ) / 255;
                        imgk[ xp + yp * w ] = v;
                    }
                }
            }
        }
    }
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = img[ x + y * w ] & 0xffffff;
            uint32_t cc = 255 - clut[ i ];
            uint32_t cm = 255 - mlut[ i ];
            uint32_t cy = 255 - ylut[ i ];
            int xp = x;
            int yp = y;
            //if( cc < 26 ) cc = 255; else if( cc < 90 ) cc = c020[ xp + yp * w ]; else if( cc < 153 ) cc = c050[ xp + yp * w ]; else if( cc < 216 ) cc = c070[ xp + yp * w ]; else cc = 0;
            //if( cm < 26 ) cm = 255; else if( cm < 90 ) cm = m020[ xp + yp * w ]; else if( cm < 153 ) cm = m050[ xp + yp * w ]; else if( cm < 216 ) cm = m070[ xp + yp * w ]; else cm = 0;
            //if( cy < 26 ) cy = 255; else if( cy < 90 ) cy = y020[ xp + yp * w ]; else if( cy < 153 ) cy = y050[ xp + yp * w ]; else if( cy < 216 ) cy = y070[ xp + yp * w ]; else cy = 0;
            if( cc < 40 ) cc = 255; else if( cc < 90 ) cc = c020[ xp + yp * mw ]; else if( cc < 153 ) cc = c050[ xp + yp * mw ]; else if( cc < 216 ) cc = c070[ xp + yp * mw ]; else cc = 0;
            if( cm < 40 ) cm = 255; else if( cm < 90 ) cm = m020[ xp + yp * mw ]; else if( cm < 153 ) cm = m050[ xp + yp * mw ]; else if( cm < 216 ) cm = m070[ xp + yp * mw ]; else cm = 0;
            if( cy < 40 ) cy = 255; else if( cy < 90 ) cy = y020[ xp + yp * mw ]; else if( cy < 153 ) cy = y050[ xp + yp * mw ]; else if( cy < 216 ) cy = y070[ xp + yp * mw ]; else cy = 0;
            cc = lerp( clut[ i ], cc, imgk[ x + y * w ] );
            cm = lerp( mlut[ i ], cm, imgk[ x + y * w ] );
            cy = lerp( ylut[ i ], cy, imgk[ x + y * w ] );
            cc = lerp( clut[ i ], cc, 128 );
            cm = lerp( mlut[ i ], cm, 128 );
            cy = lerp( ylut[ i ], cy, 128 );
            uint32_t cmy = ( cc << 16 ) | ( cm << 8 ) | cy;
            img[ x + y * w ] = 0xff000000 | ( cmy & 0xffffff );
        }
    }
    free( clut );
    free( mlut );
    free( ylut );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}

int addclamp( int a, int b ) {
    int v = a + b;
    return v > 255 ? 255 : v;
}

int subclamp( int a, int b ) {
    int v = a - b;
    return v < 0 ? 0 : v;
}

int clamp( int v ) {
    return v < 0 ? 0 : v > 255 ? 255 : v;
}

uint32_t hueshift( uint32_t in ) {
    int in_r = ( in >> 16 ) & 0xff;
    int in_g = ( in >> 8  ) & 0xff;
    int in_b = ( in       ) & 0xff;
    float fHue = 4.5f;
    const float cosA = cos(fHue*3.14159265f/180); //convert degrees to radians
    const float sinA = sin(fHue*3.14159265f/180); //convert degrees to radians
    //calculate the rotation matrix, only depends on Hue
    float matrix[3][3] = {{cosA + (1.0f - cosA) / 3.0f, 1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA, 1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA},
        {1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA, cosA + 1.0f/3.0f*(1.0f - cosA), 1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA},
        {1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA, 1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA, cosA + 1.0f/3.0f * (1.0f - cosA)}};
    //Use the rotation matrix to convert the RGB directly
    int r = clamp( in_r*matrix[0][0] + in_g*matrix[0][1] + in_b*matrix[0][2] );
    int g = clamp( in_r*matrix[1][0] + in_g*matrix[1][1] + in_b*matrix[1][2] );
    int b = clamp( in_r*matrix[2][0] + in_g*matrix[2][1] + in_b*matrix[2][2] );
    return ( r << 16 ) | ( g << 8 ) | b;
}

void rgb_to_cmy_halftone_dings( char const* in, char const* out ) {
    int mw, mh, mt;
    uint8_t* c020 = (uint8_t*) stbi_load( "masks_src/32/c020.png", &mw, &mh, &mt, 1 );
    uint8_t* m020 = (uint8_t*) stbi_load( "masks_src/32/m020.png", &mw, &mh, &mt, 1 );
    uint8_t* y020 = (uint8_t*) stbi_load( "masks_src/32/y020.png", &mw, &mh, &mt, 1 );
    uint8_t* c050 = (uint8_t*) stbi_load( "masks_src/32/c050.png", &mw, &mh, &mt, 1 );
    uint8_t* m050 = (uint8_t*) stbi_load( "masks_src/32/m050.png", &mw, &mh, &mt, 1 );
    uint8_t* y050 = (uint8_t*) stbi_load( "masks_src/32/y050.png", &mw, &mh, &mt, 1 );
    //uint8_t* c070 = (uint8_t*) stbi_load( "masks_src/32/c070.png", &mw, &mh, &mt, 1 );
    //uint8_t* m070 = (uint8_t*) stbi_load( "masks_src/32/m070.png", &mw, &mh, &mt, 1 );
    //uint8_t* y070 = (uint8_t*) stbi_load( "masks_src/32/y070.png", &mw, &mh, &mt, 1 );

    int w, h, t;
    uint8_t* dingsc = (uint8_t*) stbi_load( "lut_src_data/dings1.png", &w, &h, &t, 1 );
    uint8_t* dingsm = (uint8_t*) stbi_load( "lut_src_data/dings2.png", &w, &h, &t, 1 );
    uint8_t* dingsy = (uint8_t*) stbi_load( "lut_src_data/dings3.png", &w, &h, &t, 1 );

    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            c020[ x + y * mw ] = subclamp( c020[ x + y * mw ], lerp( dingsc[ x + y * w ], 0, 224 ) );
            c050[ x + y * mw ] = subclamp( c050[ x + y * mw ], lerp( dingsc[ x + y * w ], 0, 224 ) );
            //c070[ x + y * mw ] = subclamp( c070[ x + y * mw ], lerp( dingsc[ x + y * w ], 0, 224 ) );
            m020[ x + y * mw ] = subclamp( m020[ x + y * mw ], lerp( dingsm[ x + y * w ], 0, 224 ) );
            m050[ x + y * mw ] = subclamp( m050[ x + y * mw ], lerp( dingsm[ x + y * w ], 0, 224 ) );
            //m070[ x + y * mw ] = subclamp( m070[ x + y * mw ], lerp( dingsm[ x + y * w ], 0, 224 ) );
            y020[ x + y * mw ] = subclamp( y020[ x + y * mw ], lerp( dingsy[ x + y * w ], 0, 224 ) );
            y050[ x + y * mw ] = subclamp( y050[ x + y * mw ], lerp( dingsy[ x + y * w ], 0, 224 ) );
            //y070[ x + y * mw ] = subclamp( y070[ x + y * mw ], lerp( dingsy[ x + y * w ], 0, 224 ) );
        }
    }

    uint8_t* klut = (uint8_t*) stbi_load( "klut.png", &w, &h, &t, 1 );
    uint8_t* clut = (uint8_t*) stbi_load( "lut_src_data/c.png", &w, &h, &t, 1 );
    uint8_t* mlut = (uint8_t*) stbi_load( "lut_src_data/m.png", &w, &h, &t, 1 );
    uint8_t* ylut = (uint8_t*) stbi_load( "lut_src_data/y.png", &w, &h, &t, 1 );
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    uint8_t* imgk = (uint8_t*)malloc( w * h );
    memset( imgk, 255, w * h );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            for( int i = -1; i <= 1; ++i ) {
                for( int j = -1; j <= 1; ++j ) {
                    uint8_t k = klut[ img[ x + y * w ] & 0xffffff ];
                    int xp = x + j;
                    int yp = y + i;
                    if( xp >= 0 && xp < w && yp >= 0 && yp < h ) {
                        uint32_t v = imgk[ xp + yp * w ] & 0xffffff; 
                        v = ( v * k ) / 255;
                        imgk[ xp + yp * w ] = v;
                    }
                }
            }
        }
    }
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = img[ x + y * w ] & 0xffffff;
            uint32_t i_shift = hueshift(i);
            uint32_t cc = 255 - clut[ i ];
            uint32_t cm = 255 - mlut[ i_shift ];
            uint32_t cy = 255 - ylut[ i_shift ];
            int xp = x;
            int yp = y;
            //if( cc < 26 ) cc = 255; else if( cc < 90 ) cc = c020[ xp + yp * w ]; else if( cc < 153 ) cc = c050[ xp + yp * w ]; else if( cc < 216 ) cc = c070[ xp + yp * w ]; else cc = 0;
            //if( cm < 26 ) cm = 255; else if( cm < 90 ) cm = m020[ xp + yp * w ]; else if( cm < 153 ) cm = m050[ xp + yp * w ]; else if( cm < 216 ) cm = m070[ xp + yp * w ]; else cm = 0;
            //if( cy < 26 ) cy = 255; else if( cy < 90 ) cy = y020[ xp + yp * w ]; else if( cy < 153 ) cy = y050[ xp + yp * w ]; else if( cy < 216 ) cy = y070[ xp + yp * w ]; else cy = 0;
            if( cc < /*40*/ 45 ) cc = 255; else if( cc < /*90*/ 100 ) cc = c020[ xp + yp * mw ]; else if( cc < /*153*/ 170 ) cc = c050[ xp + yp * mw ]; /*else if( cc < 216 ) cc = c070[ xp + yp * mw ]; */ else cc = 0;
            if( cm < /*40*/ 35 ) cm = 255; else if( cm < /*90*/ 85 ) cm = m020[ xp + yp * mw ]; else if( cm < /*153*/ 170 ) cm = m050[ xp + yp * mw ]; /*else if( cm < 216 ) cm = m070[ xp + yp * mw ]; */ else cm = 0;
            if( cy < /*40*/ 30 ) cy = 255; else if( cy < /*90*/ 80 ) cy = y020[ xp + yp * mw ]; else if( cy < /*153*/ 170 ) cy = y050[ xp + yp * mw ]; /*else if( cy < 216 ) cy = y070[ xp + yp * mw ]; */ else cy = 0;
            cc = lerp( clut[ i ], cc, imgk[ x + y * w ] );
            cm = lerp( mlut[ i ], cm, imgk[ x + y * w ] );
            cy = lerp( ylut[ i ], cy, imgk[ x + y * w ] );
            //cc = lerp( clut[ i ], cc, 128 );
            //cm = lerp( mlut[ i ], cm, 128 );
            //cy = lerp( ylut[ i ], cy, 128 );
            uint32_t cmy = ( cc << 16 ) | ( cm << 8 ) | cy;
            img[ x + y * w ] = 0xff000000 | ( cmy & 0xffffff );
        }
    }
    free( clut );
    free( mlut );
    free( ylut );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}

void noise_k( char const* in, char const* out ) {
    int nw, nh, t;
    uint8_t* noise = (uint8_t*) stbi_load( "lut_src_data/cloud_noise.png", &nw, &nh, &t, 1 );
    uint8_t* unfiltered_noise = (uint8_t*) stbi_load( "lut_src_data/unfiltered_noise.png", &nw, &nh, &t, 1 );
    int w, h;
    uint8_t* dings = (uint8_t*) stbi_load( "lut_src_data/dings4.png", &w, &h, &t, 1 );
    uint8_t* ik = (uint8_t*) stbi_load( in, &w, &h, &t, 1 );
    uint8_t* io = (uint8_t*)malloc( w * h );
    memset( io, 255, w * h );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t acc = 0;
            for( int i = -1; i <= 1; ++i ) {
                for( int j = -1; j <= 1; ++j ) {
                    int xp = x + j;
                    int yp = y + i;
                    if( xp >= 0 && xp < w && yp >= 0 && yp < h ) {
                        uint32_t v = ik[ xp + yp * w ]; 
                        acc += 255 - v;
                    }
                }
            }
        acc = 255 - ( acc / 9 );
        uint32_t n = unfiltered_noise[ ( x % nw ) + ( y % nh ) * nw ];
        acc = lerp( n, 255, acc ); 
        uint8_t k = ik[ x + y * w ];
        acc = acc < k ? acc : k;
        io[ x + y * w ] = acc;
        }
    }
    
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t n = noise[ ( x % nw ) + ( y % nh ) * nw ];
            
            uint8_t k = ik[ x + y * w ];
            k = lerp( k, lerp( k, 255, n ), 105 );

            uint32_t v = io[ x + y * w ];
            v = lerp( v, lerp( v, 255, n ), 105 );
            
            io[ x + y * w ] = lerp( v < k ? v : k, 255, subclamp( 40, dings[ x + y * w ] / 8 ) );
        }
    }
    
    free( ik );
    free( noise );
    stbi_write_png( out, w, h, 1, io, w );
    free( io );
}


void noise_cmy( char const* in, char const* out ) {
    int nw, nh, t;
    uint8_t* noise = (uint8_t*) stbi_load( "lut_src_data/cloud_noise.png", &nw, &nh, &t, 1 );
    int w, h;
    uint32_t* icmy = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t cmy = icmy[ x + y * w ];
            uint32_t cc = ( cmy >> 16 ) & 0xff;
            uint32_t cm = ( cmy >> 8  ) & 0xff;
            uint32_t cy = ( cmy       ) & 0xff;
            uint32_t f = 128;
            uint32_t nc = lerp( noise[ ( ( nw - 1 ) - ( x % nw ) ) + ( y % nh ) * nw ], 0, f );
            uint32_t nm = lerp( noise[ ( x % nw ) + ( ( nh - 1 ) - ( y % nh ) ) * nw ], 0, f );
            uint32_t ny = lerp( noise[ ( ( nw - 1 ) - ( x % nw ) ) + ( ( nh - 1 ) - ( y % nh ) ) * nw ], 0, f );
            cc = lerp( cc, 0xff, nc );
            cm = lerp( cm, 0xff, nm );
            cy = lerp( cy, 0xff, ny );
            icmy[ x + y * w ] = 0xff000000 | ( cc << 16 ) | ( cm << 8 ) | cy;
            //icmy[ x + y * w ] = 0xff000000 | rgblerp( cmy, 0xffffff, n );
        }
    }
    free( noise );
    stbi_write_png( out, w, h, 4, icmy, w * 4 );
    free( icmy );
}


void xxx_noise_cmy( char const* in, char const* out ) {
    int nw, nh, t;
    uint8_t* noise = (uint8_t*) stbi_load( "lut_src_data/cloud_noise.png", &nw, &nh, &t, 1 );
    uint8_t* dingsc = (uint8_t*) stbi_load( "lut_src_data/dings1.png", &nw, &nh, &t, 1 );
    uint8_t* dingsm = (uint8_t*) stbi_load( "lut_src_data/dings2.png", &nw, &nh, &t, 1 );
    uint8_t* dingsy = (uint8_t*) stbi_load( "lut_src_data/dings3.png", &nw, &nh, &t, 1 );
    int w, h;
    uint32_t* icmy = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t cmy = icmy[ x + y * w ];
            uint32_t cc = ( cmy >> 16 ) & 0xff;
            uint32_t cm = ( cmy >> 8  ) & 0xff;
            uint32_t cy = ( cmy       ) & 0xff;
            uint32_t f = 128;
            uint32_t nc = lerp( noise[ ( ( nw - 1 ) - ( x % nw ) ) + ( y % nh ) * nw ], 0, f );
            uint32_t nm = lerp( noise[ ( x % nw ) + ( ( nh - 1 ) - ( y % nh ) ) * nw ], 0, f );
            uint32_t ny = lerp( noise[ ( ( nw - 1 ) - ( x % nw ) ) + ( ( nh - 1 ) - ( y % nh ) ) * nw ], 0, f );
            cc = lerp( cc, 0xff, nc );
            cm = lerp( cm, 0xff, nm );
            cy = lerp( cy, 0xff, ny );

            uint32_t dc = dingsc[ x + y * w ];
            uint32_t dm = dingsm[ x + y * w ];
            uint32_t dy = dingsy[ x + y * w ];
            dc = lerp( dc, 0x00, 167 );
            dm = lerp( dm, 0x00, 167 );
            dy = lerp( dy, 0x00, 167 );
            cc = lerp( cc, 0x00, dc );
            cm = lerp( cm, 0x00, dm );
            cy = lerp( cy, 0x00, dy );
            icmy[ x + y * w ] = 0xff000000 | ( cc << 16 ) | ( cm << 8 ) | cy;
            //icmy[ x + y * w ] = 0xff000000 | rgblerp( cmy, 0xffffff, n );
        }
    }
    free( noise );
    stbi_write_png( out, w, h, 4, icmy, w * 4 );
    free( icmy );
}


uint32_t rgbsub( uint32_t c1, uint32_t c2 ) {
    int32_t r1 = ( c1 >> 16 ) & 0xff;
    int32_t g1 = ( c1 >> 8  ) & 0xff;
    int32_t b1 = ( c1       ) & 0xff;
    int32_t r2 = ( c2 >> 16 ) & 0xff;
    int32_t g2 = ( c2 >> 8  ) & 0xff;
    int32_t b2 = ( c2       ) & 0xff;
    int32_t r = r1 - r2;
    int32_t g = g1 - g2;
    int32_t b = b1 - b2;
    r = r < 0 ? 0 : r;
    g = g < 0 ? 0 : g;
    b = b < 0 ? 0 : b;
    return ( r << 16 ) | ( g << 8 ) | ( b );
}

uint32_t rgbadd( uint32_t c1, uint32_t c2 ) {
    int32_t r1 = ( c1 >> 16 ) & 0xff;
    int32_t g1 = ( c1 >> 8  ) & 0xff;
    int32_t b1 = ( c1       ) & 0xff;
    int32_t r2 = ( c2 >> 16 ) & 0xff;
    int32_t g2 = ( c2 >> 8  ) & 0xff;
    int32_t b2 = ( c2       ) & 0xff;
    int32_t r = r1 + r2;
    int32_t g = g1 + g2;
    int32_t b = b1 + b2;
    r = r > 255 ? 255 : r;
    g = g > 255 ? 255 : g;
    b = b > 255 ? 255 : b;
    return ( r << 16 ) | ( g << 8 ) | ( b );
}


uint32_t rgbdarken( uint32_t c1, uint32_t c2 ) {
    int32_t r1 = ( c1 >> 16 ) & 0xff;
    int32_t g1 = ( c1 >> 8  ) & 0xff;
    int32_t b1 = ( c1       ) & 0xff;
    int32_t r2 = ( c2 >> 16 ) & 0xff;
    int32_t g2 = ( c2 >> 8  ) & 0xff;
    int32_t b2 = ( c2       ) & 0xff;
    int32_t r = r1 < r2 ? r1 : r2;
    int32_t g = g1 < g2 ? g1 : g2;
    int32_t b = b1 < b2 ? b1 : b2;
    return ( r << 16 ) | ( g << 8 ) | ( b );
}

void adjusted_img( char const* fcmy, char const* fk, char const* out ) {
    int w, h, t;
    uint8_t* k = (uint8_t*) stbi_load( fk, &w, &h, &t, 1 );
    uint32_t* cmy = (uint32_t*) stbi_load( fcmy, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t p = cmy[ x + y * w ];
            uint32_t cc = ( cmy[ x + y * w ] >> 16 ) & 0xff;
            uint32_t cm = ( cmy[ x + y * w ] >> 8  ) & 0xff;
            uint32_t cy = ( cmy[ x + y * w ]       ) & 0xff;
            uint32_t ck = ( k[ x + y * w ] ) & 0xff;
            //cc = rgblerp( 0xf8bf53, 0xffffff, cc );
            cc = rgblerp( 0xffbf13, 0xffffff, cc );
            cm = rgblerp( 0x6763d0, 0xffffff, cm );
            cy = rgblerp( 0x00edfc, 0xffffff, cy );
            uint32_t col = rgblerp( 0x000000, RGBMUL32( RGBMUL32( cc, cm ), cy ), ck );
            cmy[ x + y * w ] = 0xff000000 | col;
        }
    }
    free( k );
    stbi_write_png( out, w, h, 4, cmy, w * 4 );
    free( cmy );
}


void overlay( char const* in, char const* out ) {
    int tw, th, t;
    uint32_t* tex = (uint32_t*) stbi_load( "lut_src_data/paper_desat.png", &tw, &th, &t, 4 );
    int w, h;
    uint32_t* img = (uint32_t*) stbi_load( in, &w, &h, &t, 4 );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t p = img[ x + y * w ];
            //p = rgbsub( 0xffffff, p );
            p = rgblerp( p, rgbadd( 0x101010, RGBMUL32( p, p ) ), 200 );
            //p = rgbsub( 0xffffff, p );
            uint32_t t = tex[ ( x % tw ) + ( y % th ) * tw ];
            t = rgbdarken( p, t );
            uint32_t col = rgblerp( p, t, 157 );
            //uint32_t col = rgblerp( p, t, 128 );
            img[ x + y * w ] = 0xff000000 | col;
        }
    }
    free( tex );
    stbi_write_png( out, w, h, 4, img, w * 4 );
    free( img );
}

 
int oldmain( int argc, char** argv ) { 
    //combine_k_lut();
    rgb_to_k( "test_src/test15.png", "outk15.png" );
    rgb_to_cmy_halftone_dings( "test_src/test15.png", "outcmy15.png" );
    noise_k( "outk15.png", "noisek15.png" );
    noise_cmy( "outcmy15.png", "noisecmy15.png" );
    adjusted_img( "noisecmy15.png", "noisek15.png", "out15.png" );
    overlay( "out15.png", "overlay15.png" );

    rgb_to_k( "test_src/test1.png", "outk1.png" );
    rgb_to_k( "test_src/test2.png", "outk2.png" );
    rgb_to_k( "test_src/test3.png", "outk3.png" );
    rgb_to_cmy( "test_src/test1.png", "outcmyraw1.png" );
    rgb_to_cmy( "test_src/test2.png", "outcmyraw2.png" );
    rgb_to_cmy( "test_src/test3.png", "outcmyraw3.png" );
    rgb_to_cmy_halftone_dings( "test_src/test1.png", "outcmy1.png" );
    rgb_to_cmy_halftone_dings( "test_src/test2.png", "outcmy2.png" );
    rgb_to_cmy_halftone_dings( "test_src/test3.png", "outcmy3.png" );
    noise_k( "outk1.png", "noisek1.png" );
    noise_k( "outk2.png", "noisek2.png" );
    noise_k( "outk3.png", "noisek3.png" );
    noise_cmy( "outcmy1.png", "noisecmy1.png" );
    noise_cmy( "outcmy2.png", "noisecmy2.png" );
    noise_cmy( "outcmy3.png", "noisecmy3.png" );
    adjusted_img( "noisecmy1.png", "noisek1.png", "out1.png" );
    adjusted_img( "noisecmy2.png", "noisek2.png", "out2.png" );
    adjusted_img( "noisecmy3.png", "noisek3.png", "out3.png" );
    overlay( "out1.png", "overlay1.png" );
    overlay( "out2.png", "overlay2.png" );
    overlay( "out3.png", "overlay3.png" );



    rgb_to_k( "test_src/test6.png", "outk6.png" );
    rgb_to_cmy_halftone_dings( "test_src/test6.png", "outcmy6.png" );
    noise_k( "outk6.png", "noisek6.png" );
    noise_cmy( "outcmy6.png", "noisecmy6.png" );
    adjusted_img( "noisecmy6.png", "noisek6.png", "out6.png" );
    overlay( "out6.png", "overlay6.png" );

    rgb_to_k( "test_src/test7.png", "outk7.png" );
    rgb_to_cmy_halftone_dings( "test_src/test7.png", "outcmy7.png" );
    noise_k( "outk7.png", "noisek7.png" );
    noise_cmy( "outcmy7.png", "noisecmy7.png" );
    adjusted_img( "noisecmy7.png", "noisek7.png", "out7.png" );
    overlay( "out7.png", "overlay7.png" );

    rgb_to_k( "test_src/test8.png", "outk8.png" );
    rgb_to_cmy_halftone_dings( "test_src/test8.png", "outcmy8.png" );
    noise_k( "outk8.png", "noisek8.png" );
    noise_cmy( "outcmy8.png", "noisecmy8.png" );
    adjusted_img( "noisecmy8.png", "noisek8.png", "out8.png" );
    overlay( "out8.png", "overlay8.png" );

    rgb_to_k( "test_src/test9.png", "outk9.png" );
    rgb_to_cmy_halftone_dings( "test_src/test9.png", "outcmy9.png" );
    noise_k( "outk9.png", "noisek9.png" );
    noise_cmy( "outcmy9.png", "noisecmy9.png" );
    adjusted_img( "noisecmy9.png", "noisek9.png", "out9.png" );
    overlay( "out9.png", "overlay9.png" );

    rgb_to_k( "test_src/test10.png", "outk10.png" );
    rgb_to_cmy_halftone_dings( "test_src/test10.png", "outcmy10.png" );
    noise_k( "outk10.png", "noisek10.png" );
    noise_cmy( "outcmy10.png", "noisecmy10.png" );
    adjusted_img( "noisecmy10.png", "noisek10.png", "out10.png" );
    overlay( "out10.png", "overlay10.png" );

    rgb_to_k( "test_src/test11.png", "outk11.png" );
    rgb_to_cmy_halftone_dings( "test_src/test11.png", "outcmy11.png" );
    noise_k( "outk11.png", "noisek11.png" );
    noise_cmy( "outcmy11.png", "noisecmy11.png" );
    adjusted_img( "noisecmy11.png", "noisek11.png", "out11.png" );
    overlay( "out11.png", "overlay11.png" );

    rgb_to_k( "test_src/test12.png", "outk12.png" );
    rgb_to_cmy_halftone_dings( "test_src/test12.png", "outcmy12.png" );
    noise_k( "outk12.png", "noisek12.png" );
    noise_cmy( "outcmy12.png", "noisecmy12.png" );
    adjusted_img( "noisecmy12.png", "noisek12.png", "out12.png" );
    overlay( "out12.png", "overlay12.png" );

    rgb_to_k( "test_src/test4.png", "outk4.png" );
    rgb_to_k( "test_src/test5.png", "outk5.png" );
    rgb_to_cmy_halftone_dings( "test_src/test4.png", "outcmy4.png" );
    rgb_to_cmy_halftone_dings( "test_src/test5.png", "outcmy5.png" );
    noise_k( "outk4.png", "noisek4.png" );
    noise_k( "outk5.png", "noisek5.png" );
    noise_cmy( "outcmy4.png", "noisecmy4.png" );
    noise_cmy( "outcmy5.png", "noisecmy5.png" );
    adjusted_img( "noisecmy4.png", "noisek4.png", "out4.png" );
    adjusted_img( "noisecmy5.png", "noisek5.png", "out5.png" );
    overlay( "out4.png", "overlay4.png" );
    overlay( "out5.png", "overlay5.png" );
    return EXIT_SUCCESS; 
} 

void process( printed_t* printed, char const* in, char const* out ) {
    int width, height;
    uint32_t* output = printed_process( printed, in, &width, &height );
    if( output ) {
        printf( "%s\n", in );
        stbi_write_jpg( out, width, height, 4, output, 85 );
        free( output );
    }
}


void set_app_dir( void);

void create_path( char const* path, int pos );
void makedir( char const* path );
int folder_exists( char const* filename );
int file_exists( char const* filename );


void process_dir( printed_t* printed, char const* path ) {
    dir_t* dir = dir_open( path );
    for( dir_entry_t* entry = dir_read( dir ); entry != NULL; entry = dir_read( dir ) ) {
        if( dir_is_file( entry ) ) {
            char filename[ 256 ];
            sprintf( filename, "%s\\%s", path, dir_name( entry ) );
            process( printed, filename, filename );
        }
        if( dir_is_folder( entry ) && strcmp( dir_name( entry ), "." ) != 0 && strcmp( dir_name( entry ), ".." ) != 0 ) {
            char filename[ 256 ];
            sprintf( filename, "%s\\%s", path, dir_name( entry ) );
            process_dir( printed, filename );
        }
    }
    dir_close( dir );
}


void process_cbz( char const* infilename, char const* outpath ) {
    char const* ext = strrchr( infilename, '.' );
    if( !ext ) return;
    char const* sep = strrchr( infilename, '\\' );
    char name[ 256 ];
    int len = sep ? ext - ( sep + 1 ) : ext - infilename;
    strncpy( name, sep ? sep + 1 : infilename, len );
    name[ len ] = 0;

    create_path( outpath, 0 );

    char outfilename[ 1024 ];
    strcpy( outfilename, outpath );
    strcat( outfilename, "\\" );
    strcat( outfilename, name );
    strcat( outfilename, " - Printed.cbz" );
   
    char command[ 512 ];
    sprintf( command, "rd /s /q imgtmp & 7z.exe x -oimgtmp \"%s\"", infilename );
	system( command );

    printed_t* printed = printed_create();
    process_dir( printed, "imgtmp" );
    printed_destroy( printed );

    sprintf( command, "7z.exe a -tzip \"%s\" ./imgtmp/* -r & rd /s /q imgtmp", outfilename );
	system( command );
}


int main( int argc, char** argv ) { 
    set_app_dir();
    if( argc == 2 ) {
        char const* filename = argv[ 1 ];
        if( file_exists( filename ) ) {
            process_cbz( filename, "processed" );
            return EXIT_SUCCESS; 
        }
    }
    return EXIT_FAILURE; 
}


int xmain( int argc, char** argv ) { 
    printed_t* printed = printed_create();
    process( printed, "test_src/test1.png", "processed/test1.jpg" );
    process( printed, "test_src/test2.png", "processed/test2.jpg" );
    process( printed, "test_src/test3.png", "processed/test3.jpg" );
    process( printed, "test_src/test4.png", "processed/test4.jpg" );
    process( printed, "test_src/test5.png", "processed/test5.jpg" );
    process( printed, "test_src/test6.png", "processed/test6.jpg" );
    process( printed, "test_src/test7.png", "processed/test7.jpg" );
    process( printed, "test_src/test8.png", "processed/test8.jpg" );
    process( printed, "test_src/test9.png", "processed/test9.jpg" );
    process( printed, "test_src/test10.png", "processed/test10.jpg" );
    process( printed, "test_src/test11.png", "processed/test11.jpg" );
    process( printed, "test_src/test12.png", "processed/test12.jpg" );
    process( printed, "test_src/test13.png", "processed/test13.jpg" );
    process( printed, "test_src/test14.png", "processed/test14.jpg" );
    process( printed, "test_src/test15.png", "processed/test15.jpg" );
    printed_destroy( printed );
    return EXIT_SUCCESS; 
} 


#pragma warning( push )
#pragma warning( disable: 4255 )
#pragma warning( disable: 4668 )
#define STB_IMAGE_IMPLEMENTATION
#if defined( _WIN32 ) && ( defined( __clang__ ) || defined( __TINYC__ ) )
    #define STBI_NO_SIMD
#endif
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#pragma warning( pop )

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "libs/stb_image_resize.h"

#pragma warning( push )
#pragma warning( disable: 4204 )
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#pragma warning( pop )

#define DIR_IMPLEMENTATION
#define DIR_WINDOWS
#include "libs/dir.h"


#include <windows.h>

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )


void set_app_dir( void) {
    char exe_path[ 256 ];
    GetModuleFileNameA( NULL, exe_path, sizeof( exe_path ) );
    PathRemoveFileSpecA ( exe_path );
    SetCurrentDirectoryA( exe_path );
}


int folder_exists( char const* filename ) {
    struct stat result;
    int ret = stat( filename, &result );
    if( ret == 0 ) {
        return result.st_mode & S_IFDIR;
    }

    return 0;
}


int file_exists( char const* filename ) {
    struct stat result;
    int ret = stat( filename, &result );
    if( ret == 0 ) {
        return result.st_mode & S_IFREG;
    }

    return 0;
}


void makedir( char const* path ) {
    #ifdef _WIN32
        CreateDirectoryA( path, NULL );
    #else
        mkdir( path, S_IRWXU );
    #endif
}


void create_path( char const* path, int pos ) {
    char const* delim = strchr( path + pos, '/' );
    if( !delim ) {
        makedir( path );
        return;
    }
    pos = (int)( delim - path );
    char dir[ 256 ] = { 0 };
    strncpy( dir, path, pos );
    makedir( dir );
    create_path( path, pos + 1 );
}
