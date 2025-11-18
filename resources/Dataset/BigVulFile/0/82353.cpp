int _jsvTraceGetLowestLevel(JsVar *var, JsVar *searchVar) {
  if (var == searchVar) return 0;
  int found = -1;

  if (var->flags & JSV_IS_RECURSING)
    return -1;
  var->flags |= JSV_IS_RECURSING;

  if (jsvHasSingleChild(var) && jsvGetFirstChild(var)) {
    JsVar *child = jsvLock(jsvGetFirstChild(var));
    int f = _jsvTraceGetLowestLevel(child, searchVar);
    jsvUnLock(child);
    if (f>=0 && (found<0 || f<found)) found=f+1;
  }
  if (jsvHasChildren(var)) {
    JsVarRef childRef = jsvGetFirstChild(var);
    while (childRef) {
      JsVar *child = jsvLock(childRef);
      int f = _jsvTraceGetLowestLevel(child, searchVar);
      if (f>=0 && (found<0 || f<found)) found=f+1;
      childRef = jsvGetNextSibling(child);
      jsvUnLock(child);
    }
  }

  var->flags &= ~JSV_IS_RECURSING;

  return found; // searchRef not found
}
