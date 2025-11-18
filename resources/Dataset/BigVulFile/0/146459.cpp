void WebGLRenderingContextBase::clearColor(GLfloat r,
                                           GLfloat g,
                                           GLfloat b,
                                           GLfloat a) {
  if (isContextLost())
    return;
  if (std::isnan(r))
    r = 0;
  if (std::isnan(g))
    g = 0;
  if (std::isnan(b))
    b = 0;
  if (std::isnan(a))
    a = 1;
  clear_color_[0] = r;
  clear_color_[1] = g;
  clear_color_[2] = b;
  clear_color_[3] = a;
  ContextGL()->ClearColor(r, g, b, a);
}
