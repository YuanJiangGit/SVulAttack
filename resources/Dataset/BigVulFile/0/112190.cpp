DictionaryValue* SessionSpecificsToValue(
    const sync_pb::SessionSpecifics& proto) {
  DictionaryValue* value = new DictionaryValue();
  SET_STR(session_tag);
  SET(header, SessionHeaderToValue);
  SET(tab, SessionTabToValue);
  SET_INT32(tab_node_id);
  return value;
}
