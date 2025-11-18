static void DestroyXMLTreeRoot(XMLTreeInfo *xml_info)
{
  char
    **attributes;

  register ssize_t
    i;

  ssize_t
    j;

  XMLTreeRoot
    *root;

  assert(xml_info != (XMLTreeInfo *) NULL);
  assert((xml_info->signature == MagickSignature) ||
         (((XMLTreeRoot *) xml_info)->signature == MagickSignature));
  if (xml_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (xml_info->parent != (XMLTreeInfo *) NULL)
    return;
  /*
    Free root tag allocations.
  */
  root=(XMLTreeRoot *) xml_info;
  for (i=NumberPredefinedEntities; root->entities[i] != (char *) NULL; i+=2)
    root->entities[i+1]=DestroyString(root->entities[i+1]);
  root->entities=(char **) RelinquishMagickMemory(root->entities);
  for (i=0; root->attributes[i] != (char **) NULL; i++)
  {
    attributes=root->attributes[i];
    if (attributes[0] != (char *) NULL)
      attributes[0]=DestroyString(attributes[0]);
    for (j=1; attributes[j] != (char *) NULL; j+=3)
    {
      if (attributes[j] != (char *) NULL)
        attributes[j]=DestroyString(attributes[j]);
      if (attributes[j+1] != (char *) NULL)
        attributes[j+1]=DestroyString(attributes[j+1]);
      if (attributes[j+2] != (char *) NULL)
        attributes[j+2]=DestroyString(attributes[j+2]);
    }
    attributes=(char **) RelinquishMagickMemory(attributes);
  }
  if (root->attributes[0] != (char **) NULL)
    root->attributes=(char ***) RelinquishMagickMemory(root->attributes);
  if (root->processing_instructions[0] != (char **) NULL)
    {
      for (i=0; root->processing_instructions[i] != (char **) NULL; i++)
      {
        for (j=0; root->processing_instructions[i][j] != (char *) NULL; j++)
          root->processing_instructions[i][j]=DestroyString(
            root->processing_instructions[i][j]);
        root->processing_instructions[i][j+1]=DestroyString(
          root->processing_instructions[i][j+1]);
        root->processing_instructions[i]=(char **) RelinquishMagickMemory(
          root->processing_instructions[i]);
      }
      root->processing_instructions=(char ***) RelinquishMagickMemory(
        root->processing_instructions);
    }
}
