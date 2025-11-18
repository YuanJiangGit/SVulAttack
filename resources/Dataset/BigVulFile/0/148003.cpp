void V8TestObject::UseToImpl4ArgumentsCheckingIfPossibleWithNullableArgMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  RUNTIME_CALL_TIMER_SCOPE_DISABLED_BY_DEFAULT(info.GetIsolate(), "Blink_TestObject_useToImpl4ArgumentsCheckingIfPossibleWithNullableArg");

  test_object_v8_internal::UseToImpl4ArgumentsCheckingIfPossibleWithNullableArgMethod(info);
}
