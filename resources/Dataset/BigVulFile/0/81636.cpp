close_connection(struct mg_connection *conn)
{
#if defined(USE_SERVER_STATS)
	conn->conn_state = 6; /* to close */
#endif

#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	if (conn->lua_websocket_state) {
		lua_websocket_close(conn, conn->lua_websocket_state);
		conn->lua_websocket_state = NULL;
	}
#endif

	mg_lock_connection(conn);

	/* Set close flag, so keep-alive loops will stop */
	conn->must_close = 1;

	/* call the connection_close callback if assigned */
	if (conn->phys_ctx->callbacks.connection_close != NULL) {
		if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
			conn->phys_ctx->callbacks.connection_close(conn);
		}
	}

	/* Reset user data, after close callback is called.
	 * Do not reuse it. If the user needs a destructor,
	 * it must be done in the connection_close callback. */
	mg_set_user_connection_data(conn, NULL);


#if defined(USE_SERVER_STATS)
	conn->conn_state = 7; /* closing */
#endif

#if !defined(NO_SSL)
	if (conn->ssl != NULL) {
		/* Run SSL_shutdown twice to ensure completely close SSL connection
		 */
		SSL_shutdown(conn->ssl);
		SSL_free(conn->ssl);
/* Avoid CRYPTO_cleanup_all_ex_data(); See discussion:
 * https://wiki.openssl.org/index.php/Talk:Library_Initialization */
#if !defined(OPENSSL_API_1_1)
		ERR_remove_state(0);
#endif
		conn->ssl = NULL;
	}
#endif
	if (conn->client.sock != INVALID_SOCKET) {
		close_socket_gracefully(conn);
		conn->client.sock = INVALID_SOCKET;
	}

	if (conn->host) {
		mg_free((void *)conn->host);
		conn->host = NULL;
	}

	mg_unlock_connection(conn);

#if defined(USE_SERVER_STATS)
	conn->conn_state = 8; /* closed */
#endif
}
