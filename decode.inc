
uint bitbuf;
uint bitnum;

uint getbit( void ) {
  uint r;
  if( bitnum==0 ) {
    bitbuf = byte(get());
    bitbuf|= 256*byte(get());
    bitnum+=16;
  }
  r = (bitbuf>>15)&1;
  bitbuf<<=1;
  bitnum--;
  return r;
}

uint getvar( void ) {
  uint r = 1;
  do r = 2*r + getbit();
  while( getbit() );
  return r;
}

int decode( void ) {
  lztp_t hdr;
  uint c,i, winptr, id, l,d;

//  enum{ winlog=16, winsize=1<<winlog, winmask=winsize-1 };
//  byte win[winsize];

  for( i=0; i<sizeof(hdr); i++ ) if( (c=get())==-1 ) break; else ((byte*)&hdr)[i]=c;
  if( c==-1 ) return -1;

  winptr=0; 
  bitbuf=0; bitnum=1;

  while( winptr<hdr.size ) {

    id = getbit();
//printf( "id=%i\n", id );

    if( id==0 ) {
      // literal
      c = byte(get());
      put( win[(winptr++)&winmask]=c );
    } else {
      // match
      l = getvar()-2+4;
      d = (getvar()-2)*256;
      d+= byte(get()) + 1;
//printf( "l=%i d=%i\n", l, d );
      while( l-- ) {
        c = win[(winptr-d)&winmask];
        put( win[(winptr++)&winmask]=c );
      }

    } // if id

  } // while

  return 0;
}

