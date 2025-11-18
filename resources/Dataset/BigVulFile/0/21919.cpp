int drm_mode_setcrtc(struct drm_device *dev, void *data,
		     struct drm_file *file_priv)
{
	struct drm_mode_config *config = &dev->mode_config;
	struct drm_mode_crtc *crtc_req = data;
	struct drm_mode_object *obj;
	struct drm_crtc *crtc, *crtcfb;
	struct drm_connector **connector_set = NULL, *connector;
	struct drm_framebuffer *fb = NULL;
	struct drm_display_mode *mode = NULL;
	struct drm_mode_set set;
	uint32_t __user *set_connectors_ptr;
	int ret = 0;
	int i;

	if (!drm_core_check_feature(dev, DRIVER_MODESET))
		return -EINVAL;

	mutex_lock(&dev->mode_config.mutex);
	obj = drm_mode_object_find(dev, crtc_req->crtc_id,
				   DRM_MODE_OBJECT_CRTC);
	if (!obj) {
		DRM_DEBUG_KMS("Unknown CRTC ID %d\n", crtc_req->crtc_id);
		ret = -EINVAL;
		goto out;
	}
	crtc = obj_to_crtc(obj);
	DRM_DEBUG_KMS("[CRTC:%d]\n", crtc->base.id);

	if (crtc_req->mode_valid) {
		/* If we have a mode we need a framebuffer. */
		/* If we pass -1, set the mode with the currently bound fb */
		if (crtc_req->fb_id == -1) {
			list_for_each_entry(crtcfb,
					    &dev->mode_config.crtc_list, head) {
				if (crtcfb == crtc) {
					DRM_DEBUG_KMS("Using current fb for "
							"setmode\n");
					fb = crtc->fb;
				}
			}
		} else {
			obj = drm_mode_object_find(dev, crtc_req->fb_id,
						   DRM_MODE_OBJECT_FB);
			if (!obj) {
				DRM_DEBUG_KMS("Unknown FB ID%d\n",
						crtc_req->fb_id);
				ret = -EINVAL;
				goto out;
			}
			fb = obj_to_fb(obj);
		}

		mode = drm_mode_create(dev);
		drm_crtc_convert_umode(mode, &crtc_req->mode);
		drm_mode_set_crtcinfo(mode, CRTC_INTERLACE_HALVE_V);
	}

	if (crtc_req->count_connectors == 0 && mode) {
		DRM_DEBUG_KMS("Count connectors is 0 but mode set\n");
		ret = -EINVAL;
		goto out;
	}

	if (crtc_req->count_connectors > 0 && (!mode || !fb)) {
		DRM_DEBUG_KMS("Count connectors is %d but no mode or fb set\n",
			  crtc_req->count_connectors);
		ret = -EINVAL;
		goto out;
	}

	if (crtc_req->count_connectors > 0) {
		u32 out_id;

		/* Avoid unbounded kernel memory allocation */
		if (crtc_req->count_connectors > config->num_connector) {
			ret = -EINVAL;
			goto out;
		}

		connector_set = kmalloc(crtc_req->count_connectors *
					sizeof(struct drm_connector *),
					GFP_KERNEL);
		if (!connector_set) {
			ret = -ENOMEM;
			goto out;
		}

		for (i = 0; i < crtc_req->count_connectors; i++) {
			set_connectors_ptr = (uint32_t *)(unsigned long)crtc_req->set_connectors_ptr;
			if (get_user(out_id, &set_connectors_ptr[i])) {
				ret = -EFAULT;
				goto out;
			}

			obj = drm_mode_object_find(dev, out_id,
						   DRM_MODE_OBJECT_CONNECTOR);
			if (!obj) {
				DRM_DEBUG_KMS("Connector id %d unknown\n",
						out_id);
				ret = -EINVAL;
				goto out;
			}
			connector = obj_to_connector(obj);
			DRM_DEBUG_KMS("[CONNECTOR:%d:%s]\n",
					connector->base.id,
					drm_get_connector_name(connector));

			connector_set[i] = connector;
		}
	}

	set.crtc = crtc;
	set.x = crtc_req->x;
	set.y = crtc_req->y;
	set.mode = mode;
	set.connectors = connector_set;
	set.num_connectors = crtc_req->count_connectors;
	set.fb = fb;
	ret = crtc->funcs->set_config(&set);

out:
	kfree(connector_set);
	mutex_unlock(&dev->mode_config.mutex);
	return ret;
}
