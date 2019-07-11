
uint bitbuf_r;
uint bitnum_r;

uint getbit_r( void ) {
  uint r;
  if( bitnum_r==0 ) {
    bitbuf_r = byte(get()); 
    bitbuf_r|= 256*byte(get()); 
    put( bitbuf_r ); put( bitbuf_r>>8 );
    if( f_quit ) bitbuf_r=0;
    bitnum_r+=16;
  }
  r = (bitbuf_r>>15)&1;
  bitbuf_r<<=1;
  bitnum_r--;
  return r;
}

uint getvar_r( void ) {
  uint r = 1;
  do r = 2*r + getbit_r();
  while( getbit_r() );
  return r;
}

int recode( lztp_t& _hdr ) {
  lztp_t hdr;
  uint c,i, winptr, id, l,d;

  for( i=0; i<sizeof(hdr); i++ ) put( (((byte*)&hdr)[i])=0 );

  winptr=0; 
  bitbuf=0; bitnum=1;
  bitbuf_r=0; bitnum_r=1;
  get(); get(); 

//  while( winptr<hdr.size ) {
  while(1) {

    id = getbit_r(); if( f_quit ) break;
//printf( "id=%i\n", id );

    if( id==0 ) {
      // literal
      c = byte(xInp.get()); if( xInp.f_EOF ) break;
      put( win[(winptr++)&winmask]=c );
    } else {
      // match
      l = getvar_r()-2+4; if( f_quit ) break;
      d = (getvar_r()-2)*256; if( f_quit ) break;
      d+= byte(xInp.get()) + 1; if( xInp.f_EOF ) break;

      put(byte(d-1));

      while( l-- ) {
        c = win[(winptr-d)&winmask];
        win[(winptr++)&winmask]=c;
//        put( win[(winptr++)&winmask]=c );
      }

    } // if id

  } // while

  hdr.size = winptr;
  _hdr = hdr;

  return 0;
}

