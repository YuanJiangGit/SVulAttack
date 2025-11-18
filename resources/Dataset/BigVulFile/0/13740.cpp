ZEND_API int add_next_index_bool(zval *arg, int b) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */
