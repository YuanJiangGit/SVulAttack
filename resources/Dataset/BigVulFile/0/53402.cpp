iperf_json_finish(struct iperf_test *test)
{
    char *str;

    /* Include server output */
    if (test->json_server_output) {
	cJSON_AddItemToObject(test->json_top, "server_output_json", test->json_server_output);
    }
    if (test->server_output_text) {
	cJSON_AddStringToObject(test->json_top, "server_output_text", test->server_output_text);
    }
    str = cJSON_Print(test->json_top);
    if (str == NULL)
        return -1;
    fputs(str, stdout);
    putchar('\n');
    fflush(stdout);
    free(str);
    cJSON_Delete(test->json_top);
    test->json_top = test->json_start = test->json_connected = test->json_intervals = test->json_server_output = test->json_end = NULL;
    return 0;
}
