

typedef struct printed_t {
    int refwidth;
    int refheight;
    
    uint8_t* klut;
    uint8_t* clut;
    uint8_t* mlut;
    uint8_t* ylut;

    uint8_t* c020;
    uint8_t* m020;
    uint8_t* y020;
    uint8_t* c050;
    uint8_t* m050;
    uint8_t* y050;

    uint8_t* dingsc;
    uint8_t* dingsm;
    uint8_t* dingsy;
    uint8_t* dingsk;

    uint8_t* noise;
    uint8_t* unfiltered_noise;

    uint32_t* tex;

} printed_t;


void printed_destroy( printed_t* printed );


int32_t internal_printed_lerp( int32_t a, int32_t b, int32_t i ) {
    return a + ( ( b - a ) * i ) / 255;
}


int internal_printed_subclamp( int a, int b ) {
    int v = a - b;
    return v < 0 ? 0 : v;
}


printed_t* printed_create( void ) {
    printed_t* printed = (printed_t*)malloc( sizeof( printed_t ) );
    memset( printed, 0, sizeof( printed_t ) );

    printed->refwidth = 1988;
    printed->refheight = 3056;

    int w, h, t;
    printed->klut = (uint8_t*) stbi_load( "klut.png", &w, &h, &t, 1 );
    if( !printed->klut ) goto error;
    printed->clut = (uint8_t*) stbi_load( "lut_src_data/c.png", &w, &h, &t, 1 );
    if( !printed->clut ) goto error;
    printed->mlut = (uint8_t*) stbi_load( "lut_src_data/m.png", &w, &h, &t, 1 );
    if( !printed->mlut ) goto error;
    printed->ylut = (uint8_t*) stbi_load( "lut_src_data/y.png", &w, &h, &t, 1 );
    if( !printed->ylut ) goto error;

    printed->c020 = (uint8_t*) stbi_load( "masks_src/32/c020.png", &w, &h, &t, 1 );
    if( !printed->c020 || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->m020 = (uint8_t*) stbi_load( "masks_src/32/m020.png", &w, &h, &t, 1 );
    if( !printed->m020 || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->y020 = (uint8_t*) stbi_load( "masks_src/32/y020.png", &w, &h, &t, 1 );
    if( !printed->y020 || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->c050 = (uint8_t*) stbi_load( "masks_src/32/c050.png", &w, &h, &t, 1 );
    if( !printed->c050 || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->m050 = (uint8_t*) stbi_load( "masks_src/32/m050.png", &w, &h, &t, 1 );
    if( !printed->m050 || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->y050 = (uint8_t*) stbi_load( "masks_src/32/y050.png", &w, &h, &t, 1 );
    if( !printed->y050 || w != printed->refwidth || h != printed->refheight ) goto error;

    printed->dingsc = (uint8_t*) stbi_load( "lut_src_data/dings1.png", &w, &h, &t, 1 );
    if( !printed->dingsc || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->dingsm = (uint8_t*) stbi_load( "lut_src_data/dings2.png", &w, &h, &t, 1 );
    if( !printed->dingsm || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->dingsy = (uint8_t*) stbi_load( "lut_src_data/dings3.png", &w, &h, &t, 1 );
    if( !printed->dingsy || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->dingsk = (uint8_t*) stbi_load( "lut_src_data/dings4.png", &w, &h, &t, 1 );
    if( !printed->dingsk || w != printed->refwidth || h != printed->refheight ) goto error;

    printed->noise = (uint8_t*) stbi_load( "lut_src_data/cloud_noise.png", &w, &h, &t, 1 );
    if( !printed->noise || w != printed->refwidth || h != printed->refheight ) goto error;
    printed->unfiltered_noise = (uint8_t*) stbi_load( "lut_src_data/unfiltered_noise.png", &w, &h, &t, 1 );
    if( !printed->unfiltered_noise || w != printed->refwidth || h != printed->refheight ) goto error;

    printed->tex = (uint32_t*) stbi_load( "lut_src_data/paper_desat.png", &w, &h, &t, 4 );
    if( !printed->tex || w != printed->refwidth || h != printed->refheight ) goto error;

    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            printed->c020[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->c020[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsc[ x + y * w ], 0, 224 ) );
            printed->c050[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->c050[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsc[ x + y * w ], 0, 224 ) );
            printed->m020[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->m020[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsm[ x + y * w ], 0, 224 ) );
            printed->m050[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->m050[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsm[ x + y * w ], 0, 224 ) );
            printed->y020[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->y020[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsy[ x + y * w ], 0, 224 ) );
            printed->y050[ x + y * printed->refwidth ] = internal_printed_subclamp( printed->y050[ x + y * printed->refwidth ], internal_printed_lerp( printed->dingsy[ x + y * w ], 0, 224 ) );
        }
    }

    return printed;
error:
    printed_destroy( printed );
    return NULL;
}


void printed_destroy( printed_t* printed ) {
    if( printed->klut ) free( printed->klut );
    if( printed->clut ) free( printed->clut );
    if( printed->mlut ) free( printed->mlut );
    if( printed->ylut ) free( printed->ylut );
    if( printed->c020 ) free( printed->c020 );
    if( printed->m020 ) free( printed->m020 );
    if( printed->y020 ) free( printed->y020 );
    if( printed->c050 ) free( printed->c050 );
    if( printed->m050 ) free( printed->m050 );
    if( printed->y050 ) free( printed->y050 );
    if( printed->dingsc ) free( printed->dingsc );
    if( printed->dingsm ) free( printed->dingsm );
    if( printed->dingsy ) free( printed->dingsy );
    if( printed->dingsk ) free( printed->dingsk );
    if( printed->noise ) free( printed->noise );
    if( printed->unfiltered_noise ) free( printed->unfiltered_noise );
    if( printed->tex ) free( printed->tex );
    free( printed );
}


uint8_t* internal_printed_rgb_to_k( printed_t* printed, uint32_t* source, int width, int height ) {
    uint8_t* output = (uint8_t*) malloc( width * height * sizeof( uint8_t ) );
    for( int y = 0; y < height; ++y ) {
        for( int x = 0; x < width; ++x ) {
            uint32_t i = source[ x + y * width ] & 0xffffff;
            uint32_t k = printed->klut[ i ];
            output[ x + y * width ] = k;
        }
    }
    return output;
}


int internal_printed_clamp( int v ) {
    return v < 0 ? 0 : v > 255 ? 255 : v;
}


uint32_t internal_printed_hueshift( uint32_t in ) {
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
    int r = internal_printed_clamp( in_r*matrix[0][0] + in_g*matrix[0][1] + in_b*matrix[0][2] );
    int g = internal_printed_clamp( in_r*matrix[1][0] + in_g*matrix[1][1] + in_b*matrix[1][2] );
    int b = internal_printed_clamp( in_r*matrix[2][0] + in_g*matrix[2][1] + in_b*matrix[2][2] );
    return ( r << 16 ) | ( g << 8 ) | b;
}


uint32_t* internal_printed_rgb_to_cmy_halftone_dings( printed_t* printed, uint32_t* source, int width, int height ) {    
    int w = width;
    int h = height;
    uint8_t* kmask = (uint8_t*)malloc( w * h );
    memset( kmask, 255, w * h );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            for( int i = -1; i <= 1; ++i ) {
                for( int j = -1; j <= 1; ++j ) {
                    uint8_t k = printed->klut[ source[ x + y * w ] & 0xffffff ];
                    int xp = x + j;
                    int yp = y + i;
                    if( xp >= 0 && xp < w && yp >= 0 && yp < h ) {
                        uint32_t v = kmask[ xp + yp * w ] & 0xffffff; 
                        v = ( v * k ) / 255;
                        kmask[ xp + yp * w ] = v;
                    }
                }
            }
        }
    }
    uint32_t* output = (uint32_t*) malloc( width * height * sizeof( uint32_t ) );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t i = source[ x + y * w ] & 0xffffff;
            uint32_t i_shift = internal_printed_hueshift(i);
            uint32_t cc = 255 - printed->clut[ i ];
            uint32_t cm = 255 - printed->mlut[ i_shift ];
            uint32_t cy = 255 - printed->ylut[ i_shift ];
            int xp = x;
            int yp = y;
            if( cc < 45 ) cc = 255; else if( cc < 100 ) cc = printed->c020[ xp + yp * printed->refwidth ]; else if( cc < 170 ) cc = printed->c050[ xp + yp * printed->refwidth ]; else cc = 0;
            if( cm < 35 ) cm = 255; else if( cm <  85 ) cm = printed->m020[ xp + yp * printed->refwidth ]; else if( cm < 170 ) cm = printed->m050[ xp + yp * printed->refwidth ]; else cm = 0;
            if( cy < 30 ) cy = 255; else if( cy <  80 ) cy = printed->y020[ xp + yp * printed->refwidth ]; else if( cy < 170 ) cy = printed->y050[ xp + yp * printed->refwidth ]; else cy = 0;
            cc = lerp( printed->clut[ i ], cc, kmask[ x + y * w ] );
            cm = lerp( printed->mlut[ i ], cm, kmask[ x + y * w ] );
            cy = lerp( printed->ylut[ i ], cy, kmask[ x + y * w ] );
            uint32_t cmy = ( cc << 16 ) | ( cm << 8 ) | cy;
            output[ x + y * w ] = 0xff000000 | ( cmy & 0xffffff );
        }
    }
    free( kmask );
    return output;
}


uint8_t* internal_printed_noise_k( printed_t* printed, uint8_t* in_k, int width, int height ) {
    uint8_t* output = (uint8_t*) malloc( width * height * sizeof( uint8_t ) );
    int w = width;
    int h = height;
    memset( output, 255, w * h );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t acc = 0;
            for( int i = -1; i <= 1; ++i ) {
                for( int j = -1; j <= 1; ++j ) {
                    int xp = x + j;
                    int yp = y + i;
                    if( xp >= 0 && xp < w && yp >= 0 && yp < h ) {
                        uint32_t v = in_k[ xp + yp * w ]; 
                        acc += 255 - v;
                    }
                }
            }
        acc = 255 - ( acc / 9 );
        uint32_t n = printed->unfiltered_noise[ ( x % printed->refwidth ) + ( y % printed->refheight ) * printed->refwidth ];
        acc = lerp( n, 255, acc ); 
        uint8_t k = in_k[ x + y * w ];
        acc = acc < k ? acc : k;
        output[ x + y * w ] = acc;
        }
    }
    
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t n = printed->noise[ ( x % printed->refwidth ) + ( y % printed->refheight ) * printed->refwidth ];
            
            uint8_t k = in_k[ x + y * w ];
            k = lerp( k, lerp( k, 255, n ), 105 );

            uint32_t v = output[ x + y * w ];
            v = lerp( v, lerp( v, 255, n ), 105 );
            
            output[ x + y * w ] = lerp( v < k ? v : k, 255, subclamp( 40, printed->dingsk[ x + y * printed->refwidth ] / 8 ) );
        }
    }
    
    return output;
}


uint32_t* internal_printed_noise_cmy( printed_t* printed, uint32_t* in_cmy, int width, int height ) {
    uint32_t* output = (uint32_t*) malloc( width * height * sizeof( uint32_t ) );
    int w = width;
    int h = height;
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t cmy = in_cmy[ x + y * w ];
            uint32_t cc = ( cmy >> 16 ) & 0xff;
            uint32_t cm = ( cmy >> 8  ) & 0xff;
            uint32_t cy = ( cmy       ) & 0xff;
            uint32_t f = 128;
            uint32_t nc = internal_printed_lerp( printed->noise[ ( ( printed->refwidth - 1 ) - ( x % printed->refwidth ) ) + ( y % printed->refheight ) * printed->refwidth ], 0, f );
            uint32_t nm = internal_printed_lerp( printed->noise[ ( x % printed->refwidth ) + ( ( printed->refheight - 1 ) - ( y % printed->refheight ) ) * printed->refwidth ], 0, f );
            uint32_t ny = internal_printed_lerp( printed->noise[ ( ( printed->refwidth - 1 ) - ( x % printed->refwidth ) ) + ( ( printed->refheight - 1 ) - ( y % printed->refheight ) ) * printed->refwidth ], 0, f );
            cc = internal_printed_lerp( cc, 0xff, nc );
            cm = internal_printed_lerp( cm, 0xff, nm );
            cy = internal_printed_lerp( cy, 0xff, ny );
            output[ x + y * w ] = 0xff000000 | ( cc << 16 ) | ( cm << 8 ) | cy;
        }
    }
    return output;
}


uint32_t internal_printed_rgblerp( uint32_t color1, uint32_t color2, uint8_t alpha )	{
    uint64_t c1 = (uint64_t) color1;
    uint64_t c2 = (uint64_t) color2;
    uint64_t a = (uint64_t)( alpha );
    // bit magic to alpha blend R G B with single mul
    c1 = ( c1 | ( c1 << 24 ) ) & 0x00ff00ff00ffull;
    c2 = ( c2 | ( c2 << 24 ) ) & 0x00ff00ff00ffull;
    uint64_t o = ( ( ( ( c2 - c1 ) * a ) >> 8 ) + c1 ) & 0x00ff00ff00ffull; 
    return (uint32_t) ( o | ( o >> 24 ) );
}


uint32_t internal_printed_rgbmul( uint32_t a, uint32_t b) {
    return
        ( ( ( ( ( ( (a) >> 16U ) & 0xffU ) * ( ( (b) >> 16U ) & 0xffU ) ) >> 8U ) << 16U ) | 
          ( ( ( ( ( (a) >> 8U  ) & 0xffU ) * ( ( (b) >> 8U  ) & 0xffU ) ) >> 8U ) << 8U  ) | 
          ( ( ( ( ( (a)        ) & 0xffU ) * ( ( (b)        ) & 0xffU ) ) >> 8U )        ) );
}


uint32_t* internal_printed_adjusted_image( printed_t* printed, uint32_t* in_cmy, uint8_t* in_k, int width, int height ) {
    uint32_t* output = (uint32_t*) malloc( width * height * sizeof( uint32_t ) );
    int w = width;
    int h = height;
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t p = in_cmy[ x + y * w ];
            uint32_t cc = ( in_cmy[ x + y * w ] >> 16 ) & 0xff;
            uint32_t cm = ( in_cmy[ x + y * w ] >> 8  ) & 0xff;
            uint32_t cy = ( in_cmy[ x + y * w ]       ) & 0xff;
            uint32_t ck = ( in_k[ x + y * w ] ) & 0xff;
            cc = internal_printed_rgblerp( 0xffbf13, 0xffffff, cc );
            cm = internal_printed_rgblerp( 0x6763d0, 0xffffff, cm );
            cy = internal_printed_rgblerp( 0x00edfc, 0xffffff, cy );
            uint32_t col = internal_printed_rgblerp( 0x000000, internal_printed_rgbmul( internal_printed_rgbmul( cc, cm ), cy ), ck );
            output[ x + y * w ] = 0xff000000 | col;
        }
    }
    return output;
}


uint32_t internal_printed_rgbadd( uint32_t c1, uint32_t c2 ) {
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


uint32_t internal_printed_rgbdarken( uint32_t c1, uint32_t c2 ) {
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


uint32_t* internal_printed_overlay( printed_t* printed, uint32_t* image, int width, int height ) {
    uint32_t* output = (uint32_t*) malloc( width * height * sizeof( uint32_t ) );
    int w = width;
    int h = height;
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t p = image[ x + y * w ];
            p = internal_printed_rgblerp( p, internal_printed_rgbadd( 0x101010, internal_printed_rgbmul( p, p ) ), 200 );
            uint32_t t = printed->tex[ ( x % printed->refwidth ) + ( y % printed->refheight ) * printed->refwidth ];
            t = internal_printed_rgbdarken( p, t );
            uint32_t col = internal_printed_rgblerp( p, t, 157 );
            output[ x + y * w ] = 0xff000000 | col;
        }
    }
    return output;
}


uint32_t* printed_process( printed_t* printed, char const* filename, int* out_width, int* out_height ) {
    int w, h, t;
    uint32_t* source = (uint32_t*) stbi_load( filename, &w, &h, &t, 4 );
    if( !source ) return NULL;
    
    // TODO: rescale
    int width = w;
    int height = h;

    uint8_t* k = internal_printed_rgb_to_k( printed, source, width, height );
    uint8_t* noise_k = internal_printed_noise_k( printed, k, width, height );
    uint32_t* cmy = internal_printed_rgb_to_cmy_halftone_dings( printed, source, width, height );
    uint32_t* noise_cmy = internal_printed_noise_cmy( printed, cmy, width, height );
    uint32_t* adjusted = internal_printed_adjusted_image( printed, noise_cmy, noise_k, width, height );
    uint32_t* overlay = internal_printed_overlay( printed, adjusted, width, height );


    free( k );
    free( noise_k );
    free( cmy );
    free( noise_cmy );
    free( adjusted );
    free( source );

    *out_width = width;
    *out_height = height;
    return overlay;
}