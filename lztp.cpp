
#include "common.inc"
#include "coro2b.inc"
#include "file_api_win.inc"
#include "coro_fhp2.inc"

#include "coro2_pin.inc"

#include "coro_fhp12.inc"
#include "coro_fhp21.inc"

#undef min
#undef max

#include "model.inc"

uint flen( FILE* f ) {
  fseek( f, 0, SEEK_END );
  uint len = ftell(f);
  fseek( f, 0, SEEK_SET );
  return len;
}

ALIGN(4096) static union {
  CoroFileProc< lztp<1> > D;
  CoroFileProc12< lztp<0> > C;
  CoroFileProc21< lztp<2> > E;
};

int main( int argc, char** argv ) {

  if( argc<4 ) return 2;

  uint r,l;

  uint f_DEC = (argv[1][0]=='d')+2*(argv[1][0]=='e');
  filehandle f(argv[2]);   if( f==0 ) return 2;
  filehandle g(argv[3],(f_DEC!=2)); if( g==0 ) return 2;
  filehandle h = 0;
  if( f_DEC!=1 ) {
    if( argc>4 ) h.make(argv[4]);
    if( h==0 ) return 2;
  }

  if( f_DEC==1 ) { D.processfile( f, g ); r=D.err; }
  if( f_DEC==0 ) { C.processfile( f, g, h ); r=C.err; }
  if( f_DEC==2 ) { 
    E.processfile( f, g, h ); r=E.err; if( r==0 ) { 
      h.seek(0); h.writ( E.hdr ); 
      h.seek( AlignUp(h.size(),8) );
      SetEndOfFile( h.f );
    } 
  }

  printf( "f_DEC=%i r=%i\n", f_DEC, r );

}
