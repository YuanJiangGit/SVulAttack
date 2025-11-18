bdfPadToTerminal(FontPtr pFont)
{
    BitmapFontPtr  bitmapFont;
    BitmapExtraPtr bitmapExtra;
    int         i;
    int         new_size;
    CharInfoRec new;
    int         w,
                h;

    bitmapFont = (BitmapFontPtr) pFont->fontPrivate;

    bzero(&new, sizeof(CharInfoRec));
    new.metrics.ascent = pFont->info.fontAscent;
    new.metrics.descent = pFont->info.fontDescent;
    new.metrics.leftSideBearing = 0;
    new.metrics.rightSideBearing = pFont->info.minbounds.characterWidth;
    new.metrics.characterWidth = new.metrics.rightSideBearing;
    new_size = BYTES_FOR_GLYPH(&new, pFont->glyph);

    for (i = 0; i < bitmapFont->num_chars; i++) {
	new.bits = malloc(new_size);
	if (!new.bits) {
          bdfError("Couldn't allocate bits (%d)\n", new_size);
	    return FALSE;
	}
	FontCharReshape(pFont, &bitmapFont->metrics[i], &new);
        new.metrics.attributes = bitmapFont->metrics[i].metrics.attributes;
	free(bitmapFont->metrics[i].bits);
	bitmapFont->metrics[i] = new;
    }
    bitmapExtra = bitmapFont->bitmapExtra;
    if (bitmapExtra) {
	w = GLYPHWIDTHPIXELS(&new);
	h = GLYPHHEIGHTPIXELS(&new);
	for (i = 0; i < GLYPHPADOPTIONS; i++)
	    bitmapExtra->bitmapsSizes[i] = bitmapFont->num_chars *
		(BYTES_PER_ROW(w, 1 << i) * h);
    }
    return TRUE;
}
