static void png_handle_row(PNGDecContext *s)
{
    uint8_t *ptr, *last_row;
    int got_line;

    if (!s->interlace_type) {
        ptr = s->image_buf + s->image_linesize * (s->y + s->y_offset) + s->x_offset * s->bpp;
        if (s->y == 0)
            last_row = s->last_row;
        else
            last_row = ptr - s->image_linesize;

        png_filter_row(&s->dsp, ptr, s->crow_buf[0], s->crow_buf + 1,
                       last_row, s->row_size, s->bpp);
        /* loco lags by 1 row so that it doesn't interfere with top prediction */
        if (s->filter_type == PNG_FILTER_TYPE_LOCO && s->y > 0) {
            if (s->bit_depth == 16) {
                deloco_rgb16((uint16_t *)(ptr - s->image_linesize), s->row_size / 2,
                             s->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
            } else {
                deloco_rgb8(ptr - s->image_linesize, s->row_size,
                            s->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
            }
        }
        s->y++;
        if (s->y == s->cur_h) {
            s->state |= PNG_ALLIMAGE;
            if (s->filter_type == PNG_FILTER_TYPE_LOCO) {
                if (s->bit_depth == 16) {
                    deloco_rgb16((uint16_t *)ptr, s->row_size / 2,
                                 s->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
                } else {
                    deloco_rgb8(ptr, s->row_size,
                                s->color_type == PNG_COLOR_TYPE_RGB_ALPHA);
                }
            }
        }
    } else {
        got_line = 0;
        for (;;) {
            ptr = s->image_buf + s->image_linesize * (s->y + s->y_offset) + s->x_offset * s->bpp;
            if ((ff_png_pass_ymask[s->pass] << (s->y & 7)) & 0x80) {
                /* if we already read one row, it is time to stop to
                 * wait for the next one */
                if (got_line)
                    break;
                png_filter_row(&s->dsp, s->tmp_row, s->crow_buf[0], s->crow_buf + 1,
                               s->last_row, s->pass_row_size, s->bpp);
                FFSWAP(uint8_t *, s->last_row, s->tmp_row);
                FFSWAP(unsigned int, s->last_row_size, s->tmp_row_size);
                got_line = 1;
            }
            if ((png_pass_dsp_ymask[s->pass] << (s->y & 7)) & 0x80) {
                png_put_interlaced_row(ptr, s->cur_w, s->bits_per_pixel, s->pass,
                                       s->color_type, s->last_row);
            }
            s->y++;
            if (s->y == s->cur_h) {
                memset(s->last_row, 0, s->row_size);
                for (;;) {
                    if (s->pass == NB_PASSES - 1) {
                        s->state |= PNG_ALLIMAGE;
                        goto the_end;
                    } else {
                        s->pass++;
                        s->y = 0;
                        s->pass_row_size = ff_png_pass_row_size(s->pass,
                                                                s->bits_per_pixel,
                                                                s->cur_w);
                        s->crow_size = s->pass_row_size + 1;
                        if (s->pass_row_size != 0)
                            break;
                        /* skip pass if empty row */
                    }
                }
            }
        }
the_end:;
    }
}
