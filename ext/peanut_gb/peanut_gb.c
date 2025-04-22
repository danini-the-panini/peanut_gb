#include "peanut_gb.h"

VALUE rb_mPeanutGb;

RUBY_FUNC_EXPORTED void
Init_peanut_gb(void)
{
  rb_mPeanutGb = rb_define_module("PeanutGb");
}
