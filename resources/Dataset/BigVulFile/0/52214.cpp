 PHP_FUNCTION(locale_parse)
{
    const char* loc_name        = NULL;
    int         loc_name_len    = 0;
    int         grOffset    	= 0;

    intl_error_reset( NULL TSRMLS_CC );

    if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s",
        &loc_name, &loc_name_len ) == FAILURE)
    {
        intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
             "locale_parse: unable to parse input params", 0 TSRMLS_CC );

        RETURN_FALSE;
    }

    if(loc_name_len == 0) {
        loc_name = intl_locale_get_default(TSRMLS_C);
    }

	array_init( return_value );

	grOffset =  findOffset( LOC_GRANDFATHERED , loc_name );
	if( grOffset >= 0 ){
		add_assoc_string( return_value , LOC_GRANDFATHERED_LANG_TAG , estrdup(loc_name) ,FALSE );
	}
	else{
		/* Not grandfathered */
		add_array_entry( loc_name , return_value , LOC_LANG_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_SCRIPT_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_REGION_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_VARIANT_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_PRIVATE_TAG TSRMLS_CC);
	}
}
