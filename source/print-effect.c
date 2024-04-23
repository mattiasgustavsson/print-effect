#include <stdlib.h> 
#include <stdint.h> 
#include <stdio.h> 
#include <string.h> 
#include <math.h> 
#include <sys/stat.h> 

#ifndef _WIN32
    #include <strings.h> 
    #define stricmp strcasecmp
#endif

#include "libs/stb_image.h"
#include "libs/stb_image_resize2.h"
#include "libs/stb_image_write.h"
#include "libs/dir.h"

#include "printed.h"


void process( printed_t* printed, char const* in, char const* out ) {
    int width, height;
    uint32_t* output = printed_process( printed, in, &width, &height );
    if( output ) {
        printf( "%s\n", in );
        if( stricmp( strrchr( out, '.' ), ".png" ) == 0 ) {
            stbi_write_png( out, width, height, 4, output, width * 4 );
        } else {
            stbi_write_jpg( out, width, height, 4, output, 85 );
        }
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

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#if defined( _WIN32 ) && ( defined( __clang__ ) || defined( __TINYC__ ) )
    #define STBIR_NO_SIMD
#endif
#include "libs/stb_image_resize2.h"

#pragma warning( push )
#pragma warning( disable: 4204 )
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#pragma warning( pop )

#define DIR_IMPLEMENTATION
#if defined( _WIN32 )
    #define DIR_WINDOWS
#else
    #define DIR_POSIX
#endif
#include "libs/dir.h"


#if defined( _WIN32 )

    #include <windows.h>

    #include <shlwapi.h>
    #pragma comment( lib, "shlwapi.lib" )


    void set_app_dir( void) {
        char exe_path[ 256 ];
        GetModuleFileNameA( NULL, exe_path, sizeof( exe_path ) );
        PathRemoveFileSpecA ( exe_path );
        SetCurrentDirectoryA( exe_path );
    }
#else

    void set_app_dir( void) {

        #error "Unsupported platform"

    }


#endif


void makedir( char const* path ) {
    #ifdef _WIN32
        CreateDirectoryA( path, NULL );
    #else
        mkdir( path, S_IRWXU );
    #endif
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

