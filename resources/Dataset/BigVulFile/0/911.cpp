void CairoImageOutputDev::saveImage(CairoImage *image)
{ 
  if (numImages >= size) {
	  size += 16;
	  images = (CairoImage **) greallocn (images, size, sizeof (CairoImage *));
  }
  images[numImages++] = image;
}	
