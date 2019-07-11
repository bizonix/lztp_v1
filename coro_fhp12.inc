
template < class Model, int inpbufsize=1<<16, int outbufsize = 1<<16, int ou2bufsize=1<<16 > 
struct CoroFileProc12 : Model {

  using Model::xOut;
  using Model::f_quit;

//  enum { inpbufsize = 1<<16, outbufsize = 1<<16 };

  ALIGN(4096) byte inpbuf[inpbufsize];
  ALIGN(4096) byte outbuf[outbufsize];
  ALIGN(4096) byte ou2buf[ou2bufsize];

  void processfile( filehandle0 f, filehandle0 g, filehandle0 h ) {
    uint l=0,r;
    coro_init();
    xOut.pin_init();
    addout( outbuf, outbufsize );
    xOut.addout( ou2buf, ou2bufsize );

    while( 1 ) {
      r = coro_call(this); 
//printf( "r=%i\n", r ); fflush(stdout);

      if( r==1 ) {
        l = f.read(inpbuf,inpbufsize); if( l==0 ) f_quit=1;
        addinp( inpbuf, l );
      } 

      if( (r==2) || (r==0) ) {
        g.writ( outbuf, getoutsize() );
        addout( outbuf, outbufsize );
      }
  
      if( (r==3) || (r==0) ) {
        h.writ( ou2buf, xOut.getoutsize() );
        xOut.addout( ou2buf, ou2bufsize );
      }

      if( r==0 ) break;
    }

  }

};
