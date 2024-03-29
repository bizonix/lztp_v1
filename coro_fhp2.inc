
template < class Model, int inpbufsize=1<<16, int outbufsize = 1<<16 > 
struct CoroFileProc : Model {

  using Model::f_quit;

  ALIGN(4096) byte inpbuf[inpbufsize];
  ALIGN(4096) byte outbuf[outbufsize];

  void processfile( filehandle f, filehandle g ) {
    uint l,r;

    coro_init();
    addout( outbuf, outbufsize );

    while(1) {
      r = coro_call(this); 
//printf( "r=%i\n", r );
      if( r==1 ) {
        l = f.read( inpbuf, inpbufsize );
        if( l==0 ) f_quit=1; // get/put don't support repeated failures
        addinp( inpbuf, l ); 
      } else { // r0=quit, r3=error
        l = getoutsize();
        if( l>0 ) g.writ( outbuf, l ); 
        if( r!=2 ) break;
        addout( outbuf, outbufsize );
      } // if
    } // while
  } // func

};

