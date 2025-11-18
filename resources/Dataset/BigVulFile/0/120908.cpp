  FactoryForIsolatedApp(
      const ProfileIOData* profile_io_data,
      const StoragePartitionDescriptor& partition_descriptor,
      ChromeURLRequestContextGetter* main_context,
      scoped_ptr<ProtocolHandlerRegistry::JobInterceptorFactory>
          protocol_handler_interceptor,
      content::ProtocolHandlerMap* protocol_handlers)
      : profile_io_data_(profile_io_data),
        partition_descriptor_(partition_descriptor),
        main_request_context_getter_(main_context),
        protocol_handler_interceptor_(protocol_handler_interceptor.Pass()) {
    std::swap(protocol_handlers_, *protocol_handlers);
  }
