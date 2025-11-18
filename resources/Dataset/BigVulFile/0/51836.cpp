dissect_usb_video_streaming_interface_descriptor(proto_tree *parent_tree, tvbuff_t *tvb,
                                                 guint8 descriptor_len)
{
    proto_tree  *tree;
    int          offset = 0;
    const gchar *subtype_str;
    guint8       subtype;

    subtype = tvb_get_guint8(tvb, offset+2);

    subtype_str = val_to_str_ext(subtype, &vs_if_descriptor_subtypes_ext, "Unknown (0x%x)");
    tree = proto_tree_add_subtree_format(parent_tree, tvb, offset, descriptor_len,
            ett_descriptor_video_streaming, NULL, "VIDEO STREAMING INTERFACE DESCRIPTOR [%s]",
            subtype_str);

    dissect_usb_descriptor_header(tree, tvb, offset, &vid_descriptor_type_vals_ext);
    proto_tree_add_item(tree, hf_usb_vid_streaming_ifdesc_subtype, tvb, offset+2, 1, ENC_LITTLE_ENDIAN);
    offset += 3;

    switch (subtype)
    {
        case VS_INPUT_HEADER:
            offset = dissect_usb_video_streaming_input_header(tree, tvb, offset);
            break;

        case VS_FORMAT_UNCOMPRESSED:
        case VS_FORMAT_MJPEG:
        case VS_FORMAT_FRAME_BASED:
            offset = dissect_usb_video_format(tree, tvb, offset, subtype);
            break;

        /* @todo MPEG2, H.264, VP8, Still Image Frame */
        /* @todo Obsolete UVC-1.0 descriptors? */

        case VS_FRAME_UNCOMPRESSED:
        case VS_FRAME_MJPEG:
        case VS_FRAME_FRAME_BASED:
            offset = dissect_usb_video_frame(tree, tvb, offset, subtype);
            break;

        case VS_COLORFORMAT:
            offset = dissect_usb_video_colorformat(tree, tvb, offset);
            break;

        default:
            break;
    }

    /* Soak up descriptor bytes beyond those we know how to dissect */
    if (offset < descriptor_len)
        proto_tree_add_item(tree, hf_usb_vid_descriptor_data, tvb, offset, descriptor_len-offset, ENC_NA);

    return descriptor_len;
}
