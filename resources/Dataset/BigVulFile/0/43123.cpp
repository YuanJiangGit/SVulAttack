static int vhost_scsi_queue_status(struct se_cmd *se_cmd)
{
	struct vhost_scsi_cmd *cmd = container_of(se_cmd,
				struct vhost_scsi_cmd, tvc_se_cmd);
	vhost_scsi_complete_cmd(cmd);
	return 0;
}
