void btif_av_move_idle(RawAddress bd_addr) {
 /* inform the application that ACL is disconnected and move to idle state */
 btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);
  BTIF_TRACE_WARNING("%s: ACL Disconnected state %d bd_addr=%s peer_bda=%s",
                     __func__, state, bd_addr.ToString().c_str(),
                     btif_av_cb.peer_bda.ToString().c_str());

 if (state == BTIF_AV_STATE_OPENING && (bd_addr == btif_av_cb.peer_bda)) {
    BTIF_TRACE_DEBUG(
 "%s: Moving State from Opening to Idle due to ACL disconnect",
        __func__);
    btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
 &(btif_av_cb.peer_bda));
    btif_sm_change_state(btif_av_cb.sm_handle, BTIF_AV_STATE_IDLE);
 }
}
