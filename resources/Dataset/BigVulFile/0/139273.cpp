VREyeParameters* VRDisplay::getEyeParameters(const String& which_eye) {
  switch (StringToVREye(which_eye)) {
    case kVREyeLeft:
      return eye_parameters_left_;
    case kVREyeRight:
      return eye_parameters_right_;
    default:
      return nullptr;
  }
 }
