

typedef struct printed_t {
    int refwidth;
    int refheight;
    
    uint32_t hash;

    uint8_t* klut;
    uint8_t* clut;
    uint8_t* mlut;
    uint8_t* ylut;

    uint8_t* c020src;
    uint8_t* m020src;
    uint8_t* y020src;
    uint8_t* c050src;
    uint8_t* m050src;
    uint8_t* y050src;

    uint8_t* c020;
    uint8_t* m020;
    uint8_t* y020;
    uint8_t* c050;
    uint8_t* m050;
    uint8_t* y050;
    uint8_t* c100;
    uint8_t* m100;
    uint8_t* y100;

    uint8_t* dingssrc;
    int dingssrc_width;
    int dingssrc_height;

    uint8_t* dingsmask;
    uint8_t* dings;

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


int internal_printed_addclamp( int a, int b ) {
    int v = a + b;
    return v > 255 ? 255 : v;
}


printed_t* printed_create( void ) {
    printed_t* printed = (printed_t*)malloc( sizeof( printed_t ) );
    memset( printed, 0, sizeof( printed_t ) );

    printed->refwidth = 1988;
    printed->refheight = 3056;

    int w, h, t;
    printed->klut = (uint8_t*) stbi_load( "data/klut.png", &w, &h, &t, 1 );
    if( !printed->klut ) goto error;
    printed->clut = (uint8_t*) stbi_load( "data/c.png", &w, &h, &t, 1 );
    if( !printed->clut ) goto error;
    printed->mlut = (uint8_t*) stbi_load( "data/m.png", &w, &h, &t, 1 );
    if( !printed->mlut ) goto error;
    printed->ylut = (uint8_t*) stbi_load( "data/y.png", &w, &h, &t, 1 );
    if( !printed->ylut ) goto error;

    printed->c020src = (uint8_t*) stbi_load( "data/c020.png", &w, &h, &t, 1 );
    if( !printed->c020src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->m020src = (uint8_t*) stbi_load( "data/m020.png", &w, &h, &t, 1 );
    if( !printed->m020src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->y020src = (uint8_t*) stbi_load( "data/y020.png", &w, &h, &t, 1 );
    if( !printed->y020src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->c050src = (uint8_t*) stbi_load( "data/c050.png", &w, &h, &t, 1 );
    if( !printed->c050src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->m050src = (uint8_t*) stbi_load( "data/m050.png", &w, &h, &t, 1 );
    if( !printed->m050src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->y050src = (uint8_t*) stbi_load( "data/y050.png", &w, &h, &t, 1 );
    if( !printed->y050src || w != printed->refwidth * 2 || h != printed->refheight ) goto error;

    printed->noise = (uint8_t*) stbi_load( "data/cloud_noise.png", &w, &h, &t, 1 );
    if( !printed->noise || w != printed->refwidth * 2 || h != printed->refheight ) goto error;
    printed->unfiltered_noise = (uint8_t*) stbi_load( "data/unfiltered_noise.png", &w, &h, &t, 1 );
    if( !printed->unfiltered_noise || w != printed->refwidth * 2 || h != printed->refheight ) goto error;

    printed->tex = (uint32_t*) stbi_load( "data/paper2.png", &w, &h, &t, 4 );
    if( !printed->tex || w != printed->refwidth * 2 || h != printed->refheight ) goto error;

    printed->dingssrc = (uint8_t*) stbi_load( "data/dings.png", &printed->dingssrc_width, &printed->dingssrc_height, &t, 1 );
    if( !printed->dingssrc ) goto error;
    printed->dingsmask = (uint8_t*) stbi_load( "data/dingsmask.png", &w, &h, &t, 1 );
    if( !printed->dingsmask || w != printed->refwidth * 4 || h != printed->refheight * 2 ) goto error;

    printed->dings = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );

    printed->c020 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->m020 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->y020 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->c050 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->m050 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->y050 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->c100 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->m100 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );
    printed->y100 = (uint8_t*) malloc( printed->refwidth * printed->refheight * 2 * sizeof( uint8_t ) );


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
    if( printed->c100 ) free( printed->c100 );
    if( printed->m100 ) free( printed->m100 );
    if( printed->y100 ) free( printed->y100 );
    if( printed->c020src ) free( printed->c020src );
    if( printed->m020src ) free( printed->m020src );
    if( printed->y020src ) free( printed->y020src );
    if( printed->c050src ) free( printed->c050src );
    if( printed->m050src ) free( printed->m050src );
    if( printed->y050src ) free( printed->y050src );
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


void internal_printed_make_dings( printed_t* printed ) {
    int mask_ofs_x = rand() % ( printed->refwidth * 2 );
    int mask_ofs_y = rand() % printed->refheight;
    int dings_ofs_x = rand() % ( printed->dingssrc_width - printed->refwidth * 2 );
    int dings_ofs_y = rand() % ( printed->dingssrc_height - printed->refheight );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->dings[ x + y * printed->refwidth * 2 ] = ( printed->dingssrc[ dings_ofs_x + x + ( dings_ofs_y + y ) * printed->dingssrc_width ] * ( 255 - printed->dingsmask [ mask_ofs_x + x + ( mask_ofs_y + y ) * printed->refwidth * 4 ] ) ) / 255;
        }
    }
}

uint32_t* internal_printed_rgb_to_cmy_halftone_dings( printed_t* printed, uint32_t* source, int width, int height ) {    
    int valy = 200;
    int valc = valy + 9;
    int valm = valy + 18;
    //val = 0;
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->c020[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->c020src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valc ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->c050[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->c050src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valc ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->c100[ x + y * printed->refwidth * 2 ] = internal_printed_subclamp( 0, internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valc / 2) );
        }
    }

    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->m020[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->m020src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valm ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->m050[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->m050src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valm ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->m100[ x + y * printed->refwidth * 2 ] = internal_printed_subclamp( 0, internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valm / 2) );
        }
    }

    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->y020[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->y020src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valy ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->y050[ x + y * printed->refwidth * 2 ] = internal_printed_addclamp( printed->y050src[ x + y * printed->refwidth * 2 ], internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valy ) );
        }
    }
    internal_printed_make_dings( printed );
    for( int y = 0; y < printed->refheight; ++y ) {
        for( int x = 0; x < printed->refwidth * 2; ++x ) {
            printed->y100[ x + y * printed->refwidth * 2 ] = internal_printed_subclamp( 0, internal_printed_lerp( printed->dings[ x + y * printed->refwidth * 2 ], 0, valy / 2 ) );
        }
    }

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
            /*
            uint32_t ic = ( x - 0 >= 0 && y + 10 < height ) ? ( source[ x - 0 + ( y + 10 ) * w ] ) : 0xffffff;
            uint8_t cmask = ( x - 0 >= 0 && y + 10 < height ) ? ( kmask[ x - 0 + ( y + 10 ) * w ] ) : 0x00;
            ic = lerp( source[ x + y * w ], ic, cmask > 64 ? 255 : 0 ) & 0xffffff;

            uint32_t im = ( x + 8 < width && y + 6 < height ) ? ( source[ x + 8 + ( y + 6 ) * w ] ) : 0xffffff;
            uint8_t mmask = ( x + 8 < width && y + 6 < height ) ? ( kmask[ x + 8 + ( y + 6 ) * w ] ) : 0x00;
            im = lerp( source[ x + y * w ], im, mmask > 64 ? 255 : 0 ) & 0xffffff;
            */
            uint32_t i = source[ x + y * w ] & 0xffffff;
            uint32_t i_shift = internal_printed_hueshift(i);
            //uint32_t ic_shift = internal_printed_hueshift(ic);
            //uint32_t im_shift = internal_printed_hueshift(im);
            uint32_t cc = 255 - printed->clut[ i_shift /*ic_shift*/ ];
            uint32_t cm = 255 - printed->mlut[ i_shift /*im_shift*/ ];
            uint32_t cy = 255 - printed->ylut[ i_shift ];
            int xp = x;
            int yp = y;
            if( cc < 25 ) cc = 255; else if( cc < 100 ) cc = printed->c020[ xp + yp * printed->refwidth * 2 ]; else if( cc < 160 ) cc = printed->c050[ xp + yp * printed->refwidth * 2 ]; else cc = printed->c100[ xp + yp * printed->refwidth * 2 ];
            if( cm < 35 ) cm = 255; else if( cm <  85 ) cm = printed->m020[ xp + yp * printed->refwidth * 2 ]; else if( cm < 170 ) cm = printed->m050[ xp + yp * printed->refwidth * 2 ]; else cm = printed->m100[ xp + yp * printed->refwidth * 2 ];
            if( cy < 25 ) cy = 255; else if( cy <  80 ) cy = printed->y020[ xp + yp * printed->refwidth * 2 ]; else if( cy < 170 ) cy = printed->y050[ xp + yp * printed->refwidth * 2 ]; else cy = printed->y100[ xp + yp * printed->refwidth * 2 ];
            cc = internal_printed_lerp( printed->clut[ i /*ic*/ ], cc, kmask[ x + y * w ] );
            cm = internal_printed_lerp( printed->mlut[ i /*im*/ ], cm, kmask[ x + y * w ] );
            cy = internal_printed_lerp( printed->ylut[ i ], cy, kmask[ x + y * w ] );
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
        uint32_t n = printed->unfiltered_noise[ ( x % ( printed->refwidth * 2 ) ) + ( y % printed->refheight ) * printed->refwidth * 2];
        acc = internal_printed_lerp( n, 255, acc ); 
        uint8_t k = in_k[ x + y * w ];
        acc = acc < k ? acc : k;
        output[ x + y * w ] = acc;
        }
    }
    
    internal_printed_make_dings( printed );
    for( int y = 0; y < h; ++y ) {
        for( int x = 0; x < w; ++x ) {
            uint32_t n = printed->noise[ ( x % ( printed->refwidth * 2 ) ) + ( y % printed->refheight ) * ( printed->refwidth * 2 ) ];
            
            uint8_t k = in_k[ x + y * w ];
            k = internal_printed_lerp( k, internal_printed_lerp( k, 255, n ), 105 );

            uint32_t v = output[ x + y * w ];
            v = internal_printed_lerp( v, internal_printed_lerp( v, 255, n ), 105 );
            
            output[ x + y * w ] = internal_printed_lerp( v < k ? v : k, 255, internal_printed_subclamp( 40, printed->dings[ x + y * printed->refwidth * 2 ] / 8 ) );
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
            uint32_t nc = internal_printed_lerp( printed->noise[ ( ( (printed->refwidth*2) - 1 ) - ( x % (printed->refwidth*2) ) ) + ( y % printed->refheight ) * (printed->refwidth*2) ], 0, f );                              
            uint32_t nm = internal_printed_lerp( printed->noise[ ( x % (printed->refwidth*2) ) + ( ( printed->refheight - 1 ) - ( y % printed->refheight ) ) * (printed->refwidth*2) ], 0, f );                             
            uint32_t ny = internal_printed_lerp( printed->noise[ ( ( (printed->refwidth*2) - 1 ) - ( x % (printed->refwidth*2) ) ) + ( ( printed->refheight - 1 ) - ( y % printed->refheight ) ) * (printed->refwidth*2) ], 0, f );
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
            uint32_t t = printed->tex[ ( x % ( printed->refwidth * 2 ) ) + ( y % printed->refheight ) * printed->refwidth * 2 ];
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

    uint32_t hash = 5381u; 
    for( char const* s = filename; *s != '\0'; ++s ) {
        hash = ( ( hash << 5u ) + hash ) ^ (*s);
    }
    printed->hash = hash;
    srand( hash );
    
    
    float ar = (float)w / (float)h;
    int scaledw = w;
    int scaledh = h;
    if( 3056 * ar > 3976 ) {
        scaledw = 3976;
        scaledh = 3976 / ar;
    } else {
        scaledw = 3056 * ar;
        scaledh = 3056;
    }

    if( scaledw != w && scaledh != h ) {
        uint32_t* scaled = (uint32_t*) malloc( scaledw * scaledh * sizeof( uint32_t ) );    
        stbir_resize_uint8_linear( (unsigned char*)source, w, h, w * 4, (unsigned char*)scaled, scaledw, scaledh, scaledw * 4, STBIR_BGRA );

        free( source );
        source = scaled;
    }


    int width = scaledw;
    int height = scaledh;

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