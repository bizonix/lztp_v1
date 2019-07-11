
struct Dec {
  word id;
  word len; // sym
  uint dist; 
};

struct Cell {
  uint d;
};

enum{ winlog=15, winsize=2<<winlog, winmask=winsize-1, dist_max=1<<winlog,lah_max=1024,lah_min=4 };
byte win[winsize+1];

enum{ hashlog=16, hashsize=1<<hashlog, hashmask=hashsize-1 };
ALIGN(4) Cell hash[hashsize+1];

uint hc_d[winsize];

int encode( lztp_t& _hdr ) {
  lztp_t hdr;// = { 'ZLLS', 0, 1, 0x10, 0/*len*/, 0 };
  uint c,i,j,k,n,x, win_a,win_b,win_c, id, l,d, flag=0;
  Dec cur;

  bzero(win);
  for( i=0; i<winsize; i++ ) hc_d[i]=-dist_max;
  for( i=0; i<hashsize; i++ ) hash[i].d=-dist_max;

  win_a=win_b=win_c=1;
  bitbuf=0; bitnum=1;

  while(1) {

    // fill lookahead
    while( win_c-win_b<lah_max ) if( (c=get())==-1 ) break; else win[(win_c++)&winmask]=c;

    if_e0( win_c-win_b<lah_min ) l=1; else {// no lookahead bytes

      Cell& hc = hash[ hashfunc( win[(win_b+0)&winmask],win[(win_b+1)&winmask],win[(win_b+2)&winmask] ) ];

      // find longest match
      l=1; d=0; x = Min<uint>(lah_max,win_c-win_b); k=0;
      for_e1( i=hc.d, ((win_b-i)<=dist_max), i=hc_d[i&winmask] ) {
        #pragma loop_count min(1),max(lah_max),avg(5)
        for( j=0; j<x; j++ ) if( win[(i+j)&winmask]!=win[(win_b+j)&winmask] ) break;
//        if_e0( j>win_b-i ) j=win_b-i; // original encoder doesn't allow overlaps
        if_e0( j>l ) l=j,d=win_b-i; // remember longer match
//        if( ++k>3 ) break;
      }
      l = l<lah_min ? 1 : l;

      hc_d[win_b&winmask] = hc.d;
      hc.d = win_b;
      for_e1( k=1, k<l, k++ ) {
        Cell& hc = hash[ hashfunc( win[(win_b+k+0)&winmask],win[(win_b+k+1)&winmask],win[(win_b+k+2)&winmask] ) ];
        hc_d[ (win_b+k)&winmask] = hc.d;
        hc.d = win_b+k;
      }

    } // if win_c

    if_e1( win_c-win_b>=1 ) {
      c = win[win_b&winmask]; // first lookahead byte
      Dec cur1 = { (l<lah_min?0:1), (l<lah_min?c:l), d };
      cur = cur1;
      win_b += l<lah_min?1:l; // shift current pos in window
    } else flag=1;

    if_e1( win_b-win_a>dist_max ) win_a=win_b-dist_max;

    if_e0( flag ) break;

    putbit( cur.id );

    if( cur.id==0 ) {

      xOut.put( cur.len ); // literal

    } else {

      //l = getvar()-2+4;
      //d = (getvar()-2)*256;
      //d+= byte(get()) + 1;
      putvar( cur.len-4 );
      putvar( (cur.dist-1)/256 );
      xOut.put( byte(cur.dist-1) );

    }

  } // while

  while( bitnum!=15 ) putbit(0); // flush bits

  hdr.size = win_c-1;
//  hdr.zsize = zsize;
  _hdr = hdr;

  return 0;
}

uint lowbias32( uint x ) const {
  x ^= x >> 16;
  x *= uint(0x7feb352d);
  x ^= x >> 15;
  x *= uint(0x846ca68b);
  x ^= x >> 16;
  return x;
}

uint hashfunc( byte b1, byte b2, byte b3 ) const {
  uint x = b1;
  x |= uint(b2)<<8;
  x |= uint(b3)<<16;
  x |= uint(b1)<<24;
  x = lowbias32(x) & hashmask;
  return x;
}

void putbit( uint bit ) {
  if( bitnum==0 ) {
    put( bitbuf );
    put( bitbuf>>8 );
    bitbuf=0; bitnum=16;
  }
  bitbuf |= bit<<(bitnum-1);
  bitnum--;
}

void putvar( uint var ) {
  uint r=1; var+=2;
  do {
    r = 2*r + (var&1); var>>=1;
  } while( var!=1 );

  do {
    putbit( r&1 ); r>>=1;
    putbit( r>1 );
  } while( r>1 );
}




