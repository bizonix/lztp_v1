
struct coro_pin_0 {

  byte* ptr;
  byte* beg;
  byte* end;
  uint  f_EOF;

  byte get0( void ) { return *ptr++; }
  void put0( uint c ) { *ptr++ = c; }

  void pin_init( void ) {
    ptr=beg=end=0; f_EOF=0;
  }

  uint getinplen()  { return end-ptr; } //-V110
  uint getinpleft() { return end-ptr; } //-V110
  uint getinpsize() { return ptr-beg; } //-V110

  uint getoutlen()  { return end-ptr; } //-V110
  uint getoutleft() { return end-ptr; } //-V110
  uint getoutsize() { return ptr-beg; } //-V110

  void addbuf( byte* buf,uint len ) {
    beg = ptr = buf;
    end = &buf[len];
  }

  void addinp( byte* inp,uint inplen ) { addbuf(inp,inplen); }
  void addout( byte* out,uint outlen ) { addbuf(out,outlen); }

};

template< class wrap, int (*base_offs)(void), int _r_code >
struct coro_pin : coro_pin_0 {

  enum{ r_code = _r_code };

  void yield_r( int value = r_code ) {
    wrap& W = *(wrap*)(((char*)this) - ((*base_offs)()) );
    yield( (void*)&W, value );
    //W.yield( value );
    //__assume(0);
  }

  uint f_quit( void ) {
    wrap& W = *(wrap*)(((char*)this) - ((*base_offs)()) );
    return f_EOF || W.f_quit;
  }

  void chkinp( void ) { if( __builtin_expect(ptr>=end,0) ) yield_r(); }

  void chkout( uint d=0 ) { if( __builtin_expect(ptr>=end-d,0) ) yield_r(); }

  uint get( void ) { 
    if( __builtin_expect(ptr>=end,0) ) {
      do {
        if( f_quit() ) return uint(-1);
        yield_r();
      } while( ptr>=end );
    }
    return *ptr++; 
  }

  void put( uint c ) { 
    *ptr++ = c; chkout(); 
  }

#if 0
  #include "coro2_getstr.inc"
  #include "coro2_putstr.inc"
#endif

};

#define coro_pin_w(wrap,pin,code) \
static int get__##pin(void) { return offsetof(wrap,pin); }\
coro_pin< wrap, get__##pin, code > pin;

//#define coro_pin_W(pin,code) coro_pin_W( decltype(*this),pin,code )

#define coro_pin_W(pin,code) \
void dummy__##pin(void) {} \
template < class T > static T type__##pin( void (T::*func)(void) ) { return T(); } \
coro_pin_w( decltype(type__##pin(dummy__##pin)),pin,code )









