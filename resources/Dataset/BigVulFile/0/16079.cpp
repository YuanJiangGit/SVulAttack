void ContainerChunk::replaceChildWithJunk( Chunk* child, bool deleteChild )
{
	chunkVectIter iter = getChild( child );
	if ( iter == this->children.end() ) {
		throw new XMP_Error(kXMPErr_InternalFailure, "replaceChildWithJunk: childChunk not found.");
	}

	*iter = new JunkChunk ( NULL, child->oldSize );
	if ( deleteChild ) delete child;

	this->hasChange = true;
}
