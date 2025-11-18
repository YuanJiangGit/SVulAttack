void Layer::SetBlendMode(SkXfermode::Mode blend_mode) {
  DCHECK(IsPropertyChangeAllowed());
  if (blend_mode_ == blend_mode)
    return;

  switch (blend_mode) {
    case SkXfermode::kSrcOver_Mode:
    case SkXfermode::kScreen_Mode:
    case SkXfermode::kOverlay_Mode:
    case SkXfermode::kDarken_Mode:
    case SkXfermode::kLighten_Mode:
    case SkXfermode::kColorDodge_Mode:
    case SkXfermode::kColorBurn_Mode:
    case SkXfermode::kHardLight_Mode:
    case SkXfermode::kSoftLight_Mode:
    case SkXfermode::kDifference_Mode:
    case SkXfermode::kExclusion_Mode:
    case SkXfermode::kMultiply_Mode:
    case SkXfermode::kHue_Mode:
    case SkXfermode::kSaturation_Mode:
    case SkXfermode::kColor_Mode:
    case SkXfermode::kLuminosity_Mode:
      break;
    case SkXfermode::kClear_Mode:
    case SkXfermode::kSrc_Mode:
    case SkXfermode::kDst_Mode:
    case SkXfermode::kDstOver_Mode:
    case SkXfermode::kSrcIn_Mode:
    case SkXfermode::kDstIn_Mode:
    case SkXfermode::kSrcOut_Mode:
    case SkXfermode::kDstOut_Mode:
    case SkXfermode::kSrcATop_Mode:
    case SkXfermode::kDstATop_Mode:
    case SkXfermode::kXor_Mode:
    case SkXfermode::kPlus_Mode:
    case SkXfermode::kModulate_Mode:
      NOTREACHED();
      return;
  }

  blend_mode_ = blend_mode;
  SetNeedsCommit();
  SetNeedsFilterContextIfNeeded();
}
