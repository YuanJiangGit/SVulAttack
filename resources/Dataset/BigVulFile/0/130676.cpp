static void classMethodWithOptionalMethod(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "classMethodWithOptional", "TestObject", info.Holder(), info.GetIsolate());
    if (UNLIKELY(info.Length() <= 0)) {
        v8SetReturnValueInt(info, TestObject::classMethodWithOptional());
        return;
    }
    V8TRYCATCH_EXCEPTION_VOID(int, arg, toInt32(info[0], exceptionState), exceptionState);
    v8SetReturnValueInt(info, TestObject::classMethodWithOptional(arg));
}
