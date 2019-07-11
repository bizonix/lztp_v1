
template < class Model, int inpbufsize=1<<16, int in2bufsize=1<<16, int outbufsize=1<<16 > 
struct CoroFileProc21 : Model {

  using Model::xInp;
  using Model::f_quit;
  using Model::outptr;

  ALIGN(4096) byte inpbuf[inpbufsize];
  ALIGN(4096) byte in2buf[in2bufsize];
  ALIGN(4096) byte outbuf[outbufsize];

  void processfile( filehandle0 f, filehandle0 g, filehandle0 h ) {
    uint l,r;
    coro_init();
    xInp.pin_init();
    addout( outbuf, outbufsize );
    uint& f_quit2 = xInp.f_EOF;
    while( 1 ) {
      r = coro_call(this); 
printf( "r=%i f_quit=%i f_quit2=%i\n", r, f_quit,f_quit2 ); //fflush(stdout);
      if( r==1 ) {
        l = f.read( inpbuf, inpbufsize ); if( l==0 ) f_quit=1;
printf( "r1.l=%i\n", l );
        addinp( inpbuf, l ); 
      }
      if( r==3 ) {
        l = g.read( in2buf, in2bufsize ); if( l==0 ) f_quit2=1;
printf( "xInp.l=%i\n", l );
        xInp.addinp( in2buf, l ); 
      } 
      if( (r==2) || (r==0) ) {
        h.writ( (byte*)outbeg, outptr-outbuf );
        addout( outbuf, outbufsize );
        if( r==0 ) break;
      }
    } // while

  } // processfile

};
