#include "peanut_gb.h"
#include "ruby/internal/globals.h"
#include "ruby/internal/intern/array.h"
#include "ruby/internal/special_consts.h"

// TODO: we need pointer to gb_s
void audio_read(const uint_fast32_t);
void audio_write(const uint_fast32_t, const uint8_t);

#include "vendor/peanut_gb.h"

VALUE rb_mPeanutGb;
VALUE rb_cEmulator;

ID mid_rom_read;
ID mid_cart_ram_read;
ID mid_cart_ram_write;
ID mid_error;
ID mid_lcd_draw_line;
ID mid_audio_read;
ID mid_audio_write;
ID mid_serial_tx;
ID mid_serial_rx;
ID mid_bootrom_read;

ID e_unknown_error;
ID e_invalid_opcode;
ID e_invalid_read;
ID e_invalid_write;
ID e_invalid_max;

ID joypad_a;
ID joypad_b;
ID joypad_start;
ID joypad_select;
ID joypad_up;
ID joypad_down;
ID joypad_left;
ID joypad_right;

void free_gb_emulator(struct gb_s*);
size_t memsize_gb_emulator(const void*);

uint8_t rb_gb_rom_read(struct gb_s*, const uint_fast32_t);
uint8_t rb_gb_cart_ram_read(struct gb_s*, const uint_fast32_t);
void rb_gb_cart_ram_write(struct gb_s*, const uint_fast32_t, const uint8_t);
void rb_gb_error(struct gb_s*, const enum gb_error_e, const uint16_t);

void rb_gb_serial_tx(struct gb_s*, const uint8_t);
enum gb_serial_rx_ret_e rb_gb_serial_rx(struct gb_s*, uint8_t*);

uint8_t rb_gb_bootrom_read(struct gb_s*, const uint_fast16_t);

void rb_lcd_draw_line(struct gb_s*, const uint8_t*, uint_fast8_t);

static const rb_data_type_t gb_emulator_type = {
    .wrap_struct_name = "gb_emulator",
    .function = {
      .dfree = RBIMPL_DATA_FUNC(free_gb_emulator),
      .dsize = memsize_gb_emulator
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

#define GetGB(obj, gb) do {\
    TypedData_Get_Struct((obj), struct gb_s, &gb_emulator_type, (gb));\
} while (0)

void free_gb_emulator(struct gb_s *gb)
{
  free(gb);
}

size_t memsize_gb_emulator(const void *gb)
{
  return sizeof(struct gb_s);
}

static VALUE
gb_emulator_alloc(VALUE klass)
{
  struct gb_s *gb = malloc(sizeof(struct gb_s));
  return TypedData_Make_Struct(klass, struct gb_s, &gb_emulator_type, gb);
}

VALUE rb_gb_init(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);

  enum gb_init_error_e e = gb_init(gb,
    rb_gb_rom_read,
    rb_gb_cart_ram_read,
    rb_gb_cart_ram_write,
    rb_gb_error,
    (void*)self
  );

  if (e != GB_INIT_NO_ERROR) {
    rb_raise(rb_eRuntimeError, "error initializing PEanutGB");
  }

  gb_init_lcd(gb, rb_lcd_draw_line);
  gb_init_serial(gb, rb_gb_serial_tx, rb_gb_serial_rx);

  return Qnil;
}

uint8_t rb_gb_rom_read(struct gb_s* gb, const uint_fast32_t addr)
{
  VALUE self = (VALUE)gb->direct.priv;
  VALUE ret = rb_funcall(self, mid_rom_read, 1, UINT2NUM(addr));
  return (uint8_t)FIX2UINT(ret);
}

uint8_t rb_gb_cart_ram_read(struct gb_s* gb, const uint_fast32_t addr)
{
  VALUE self = (VALUE)gb->direct.priv;
  VALUE ret = rb_funcall(self, mid_cart_ram_read, 1, UINT2NUM(addr));
  return (uint8_t)FIX2UINT(ret);
}

void rb_gb_cart_ram_write(struct gb_s* gb, const uint_fast32_t addr, const uint8_t value)
{
  VALUE self = (VALUE)gb->direct.priv;
  rb_funcall(self, mid_cart_ram_write, 2, UINT2NUM(addr), UINT2NUM(value));
}

ID gb_error_to_id(const enum gb_error_e e) {
  switch (e) {
    default:
    case GB_UNKNOWN_ERROR:
      return e_unknown_error;
    case GB_INVALID_OPCODE:
      return e_invalid_opcode;
    case GB_INVALID_READ:
      return e_invalid_read;
    case GB_INVALID_WRITE:
      return e_invalid_write;
    case GB_INVALID_MAX:
      return e_invalid_max;
  }
}

void rb_gb_error(struct gb_s* gb, const enum gb_error_e e, const uint16_t addr)
{
  VALUE self = (VALUE)gb->direct.priv;
  rb_funcall(self, mid_error, 2, ID2SYM(gb_error_to_id(e)), UINT2NUM(addr));
}

void rb_gb_serial_tx(struct gb_s *gb, const uint8_t tx)
{
  VALUE self = (VALUE)gb->direct.priv;
  rb_funcall(self, mid_serial_tx, 1, UINT2NUM(tx));
}

enum gb_serial_rx_ret_e rb_gb_serial_rx(struct gb_s *gb, uint8_t *rx)
{
  VALUE self = (VALUE)gb->direct.priv;
  VALUE ret = rb_funcall(self, mid_serial_rx, 0);
  if (ret == Qnil) {
    return GB_SERIAL_RX_NO_CONNECTION;
  }
  *rx = (uint8_t)FIX2UINT(ret);
  return GB_SERIAL_RX_SUCCESS;
}

uint8_t rb_gb_bootrom_read(struct gb_s *gb, const uint_fast16_t addr)
{
  VALUE self = (VALUE)gb->direct.priv;
  VALUE ret = rb_funcall(self, mid_bootrom_read, 1, UINT2NUM(addr));
  return (uint8_t)FIX2UINT(ret);
}

void rb_lcd_draw_line(struct gb_s *gb, const uint8_t *pixels, uint_fast8_t line)
{
  VALUE self = (VALUE)gb->direct.priv;
  VALUE rb_pixels = rb_ary_new2(LCD_WIDTH);
  for (unsigned int i = 0; i < LCD_WIDTH; i++) {
    rb_ary_store(rb_pixels, i, UINT2NUM(pixels[i]));
  }
  rb_funcall(self, mid_lcd_draw_line, 2, rb_pixels, UINT2NUM(line));
}

void audio_read(const uint_fast32_t addr)
{
  rb_warn("audio is not implemented");
}

void audio_write(const uint_fast32_t addr, const uint8_t value)
{
  rb_warn("audio is not implemented");
}

VALUE rb_gb_reset(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  gb_reset(gb);
  return Qnil;
}

VALUE rb_gb_get_save_size(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  return UINT2NUM(gb_get_save_size(gb));
}

VALUE rb_gb_run_frame(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  gb_run_frame(gb);
  return Qnil;
}

VALUE rb_gb_colour_hash(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  return UINT2NUM(gb_colour_hash(gb));
}

VALUE rb_gb_get_rom_name(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  char title[16];
  gb_get_rom_name(gb, title);
  return rb_str_new_cstr(title);
}

VALUE rb_gb_set_rtc(VALUE self, VALUE rbtime)
{
  struct gb_s *gb;
  GetGB(self, gb);
  struct timeval tv = rb_time_timeval(rbtime);
  struct tm *time = localtime(&tv.tv_sec);
  gb_set_rtc(gb, time);
  return Qnil;
}

VALUE rb_set_buttons(VALUE self, VALUE buttons)
{
  struct gb_s *gb;
  GetGB(self, gb);
  
  gb->direct.joypad_bits.a      = !RTEST(rb_ary_entry(buttons, 0));
  gb->direct.joypad_bits.b      = !RTEST(rb_ary_entry(buttons, 1));
  gb->direct.joypad_bits.select = !RTEST(rb_ary_entry(buttons, 2));
  gb->direct.joypad_bits.start  = !RTEST(rb_ary_entry(buttons, 3));
  gb->direct.joypad_bits.right  = !RTEST(rb_ary_entry(buttons, 4));
  gb->direct.joypad_bits.left   = !RTEST(rb_ary_entry(buttons, 5));
  gb->direct.joypad_bits.up     = !RTEST(rb_ary_entry(buttons, 6));
  gb->direct.joypad_bits.down   = !RTEST(rb_ary_entry(buttons, 7));

  return Qnil;
}

VALUE rb_get_interlace(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  return gb->direct.interlace ? Qtrue : Qfalse;
}

VALUE rb_set_interlace(VALUE self, VALUE value)
{
  struct gb_s *gb;
  GetGB(self, gb);
  gb->direct.interlace = RTEST(value);
  return Qnil;
}

VALUE rb_get_frame_skip(VALUE self)
{
  struct gb_s *gb;
  GetGB(self, gb);
  return gb->direct.frame_skip ? Qtrue : Qfalse;
}

VALUE rb_set_frame_skip(VALUE self, VALUE value)
{
  struct gb_s *gb;
  GetGB(self, gb);
  gb->direct.frame_skip = RTEST(value);
  return Qnil;
}

RUBY_FUNC_EXPORTED void
Init_peanut_gb(void)
{
  mid_rom_read = rb_intern("rom_read");
  mid_cart_ram_read = rb_intern("cart_ram_read");
  mid_cart_ram_write = rb_intern("cart_ram_write");
  mid_error = rb_intern("error");
  mid_lcd_draw_line = rb_intern("lcd_draw_line");
  mid_audio_read = rb_intern("audio_read");
  mid_audio_write = rb_intern("audio_write");
  mid_serial_tx = rb_intern("serial_tx");
  mid_serial_rx = rb_intern("serial_rx");
  mid_bootrom_read = rb_intern("bootrom_read");

  e_unknown_error = rb_intern("GB_UNKNOWN_ERROR");
  e_invalid_opcode = rb_intern("GB_INVALID_OPCODE");
  e_invalid_read = rb_intern("GB_INVALID_READ");
  e_invalid_write = rb_intern("GB_INVALID_WRITE");
  e_invalid_max = rb_intern("GB_INVALID_MAX");

  joypad_a = rb_intern("a");
  joypad_b = rb_intern("b");
  joypad_start = rb_intern("start");
  joypad_select = rb_intern("select");
  joypad_up = rb_intern("up");
  joypad_down = rb_intern("down");
  joypad_left = rb_intern("left");
  joypad_right = rb_intern("right");

  rb_mPeanutGb = rb_define_module("PeanutGb");
  rb_cEmulator = rb_define_class_under(rb_mPeanutGb, "Emulator", rb_cObject);
  rb_define_alloc_func(rb_cEmulator, gb_emulator_alloc);
  rb_define_method(rb_cEmulator, "initialize", rb_gb_init, 0);

  rb_define_method(rb_cEmulator, "reset", rb_gb_reset, 0);
  rb_define_method(rb_cEmulator, "save_size", rb_gb_get_save_size, 0);
  rb_define_method(rb_cEmulator, "run_frame", rb_gb_run_frame, 0);
  rb_define_method(rb_cEmulator, "colour_hash", rb_gb_colour_hash, 0);
  rb_define_method(rb_cEmulator, "rom_name", rb_gb_get_rom_name, 0);
  rb_define_method(rb_cEmulator, "rtc=", rb_gb_set_rtc, 1);

  rb_define_method(rb_cEmulator, "set_buttons", rb_set_buttons, 1);
  rb_define_method(rb_cEmulator, "interlace", rb_get_interlace, 0);
  rb_define_method(rb_cEmulator, "interlace=", rb_set_interlace, 1);
  rb_define_method(rb_cEmulator, "frame_skip", rb_get_frame_skip, 0);
  rb_define_method(rb_cEmulator, "frame_skip=", rb_set_frame_skip, 1);
}
