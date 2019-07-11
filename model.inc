
template< int f_DEC >
struct lztp : Coroutine {

  coro_pin_w( lztp, xInp, 3 );
  coro_pin_w( lztp, xOut, 3 );

  int err;

  struct lztp_t {
    byte  hash[16];
    uint  size;
  };

  lztp_t hdr;

  #include "decode.inc"
  #include "encode.inc"
  #include "recode.inc"

  void do_process( void ) {
    if( f_DEC==1 ) err = decode();
    if( f_DEC==0 ) err = encode( hdr ); // encode 1i to 2o
    if( f_DEC==2 ) err = recode( hdr ); // recode 2i to 1o
    yield(this,0);
  }

};
