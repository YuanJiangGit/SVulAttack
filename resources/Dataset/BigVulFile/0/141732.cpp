std::unique_ptr<V8StackTraceImpl> V8Debugger::createStackTrace(v8::Local<v8::StackTrace> stackTrace)
{
    int contextGroupId = m_isolate->InContext() ? getGroupId(m_isolate->GetCurrentContext()) : 0;
    return V8StackTraceImpl::create(this, contextGroupId, stackTrace, V8StackTraceImpl::maxCallStackSizeToCapture);
}
