gc_step_ratio_set(mrb_state *mrb, mrb_value obj)
{
  mrb_int ratio;

  mrb_get_args(mrb, "i", &ratio);
  mrb->gc.step_ratio = ratio;
  return mrb_nil_value();
}
