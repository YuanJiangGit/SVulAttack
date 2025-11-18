static u16 cellSize(MemPage *pPage, int iCell){
  return pPage->xCellSize(pPage, findCell(pPage, iCell));
}
