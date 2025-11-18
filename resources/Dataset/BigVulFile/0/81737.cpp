int ff_mpv_export_qp_table(MpegEncContext *s, AVFrame *f, Picture *p, int qp_type)
{
    AVBufferRef *ref = av_buffer_ref(p->qscale_table_buf);
    int offset = 2*s->mb_stride + 1;
    if(!ref)
        return AVERROR(ENOMEM);
    av_assert0(ref->size >= offset + s->mb_stride * ((f->height+15)/16));
    ref->size -= offset;
    ref->data += offset;
    return av_frame_set_qp_table(f, ref, s->mb_stride, qp_type);
}
