bool IsFormTextArea(PDFiumPage::Area area, int form_type) {
  if (form_type == FPDF_FORMFIELD_UNKNOWN)
    return false;

  DCHECK_EQ(area, PDFiumPage::FormTypeToArea(form_type));
  return area == PDFiumPage::FORM_TEXT_AREA;
}
