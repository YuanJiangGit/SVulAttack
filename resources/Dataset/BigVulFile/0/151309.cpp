std::unique_ptr<TracedValue> InspectorXhrReadyStateChangeEvent::Data(
    ExecutionContext* context,
    XMLHttpRequest* request) {
  std::unique_ptr<TracedValue> value = TracedValue::Create();
  value->SetString("url", request->Url().GetString());
  value->SetInteger("readyState", request->readyState());
  if (LocalFrame* frame = FrameForExecutionContext(context))
    value->SetString("frame", ToHexString(frame));
  SetCallStack(value.get());
  return value;
}
