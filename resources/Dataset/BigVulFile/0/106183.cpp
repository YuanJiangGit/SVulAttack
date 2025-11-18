JSValue jsTestObjWithScriptArgumentsAndCallStackAttribute(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(slotBase));
    RefPtr<ScriptCallStack> callStack(createScriptCallStackForInspector(exec));
    TestObj* impl = static_cast<TestObj*>(castedThis->impl());
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->withScriptArgumentsAndCallStackAttribute(callStack)));
    return result;
}
