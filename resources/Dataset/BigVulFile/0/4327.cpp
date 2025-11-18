PHPAPI zend_bool register_user_shutdown_function(char *function_name, size_t function_len, php_shutdown_function_entry *shutdown_function_entry TSRMLS_DC) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *)) user_shutdown_function_dtor, 0);
	}

	return zend_hash_update(BG(user_shutdown_function_names), function_name, function_len, shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL) != FAILURE;
}
/* }}} */
