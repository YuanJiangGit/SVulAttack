void setJSTestObjLongLongSequenceAttr(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSTestObj* castedThis = jsCast<JSTestObj*>(thisObject);
    TestObj* impl = static_cast<TestObj*>(castedThis->impl());
    impl->setLongLongSequenceAttr(toNativeArray<long long>(exec, value));
}
