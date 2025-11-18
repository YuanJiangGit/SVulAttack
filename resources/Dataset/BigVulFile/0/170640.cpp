int AgcSetParameter (preproc_effect_t *effect, void *pParam, void *pValue)
{
 int status = 0;
 uint32_t param = *(uint32_t *)pParam;
    t_agc_settings *pProperties = (t_agc_settings *)pValue;
    webrtc::GainControl *agc = static_cast<webrtc::GainControl *>(effect->engine);

 switch (param) {
 case AGC_PARAM_TARGET_LEVEL:
        ALOGV("AgcSetParameter() target level %d milliBels", *(int16_t *)pValue);
        status = agc->set_target_level_dbfs(-(*(int16_t *)pValue / 100));
 break;
 case AGC_PARAM_COMP_GAIN:
        ALOGV("AgcSetParameter() comp gain %d milliBels", *(int16_t *)pValue);
        status = agc->set_compression_gain_db(*(int16_t *)pValue / 100);
 break;
 case AGC_PARAM_LIMITER_ENA:
        ALOGV("AgcSetParameter() limiter enabled %s", *(bool *)pValue ? "true" : "false");
        status = agc->enable_limiter(*(bool *)pValue);
 break;
 case AGC_PARAM_PROPERTIES:
        ALOGV("AgcSetParameter() properties level %d, gain %d limiter %d",
             pProperties->targetLevel,
             pProperties->compGain,
             pProperties->limiterEnabled);
        status = agc->set_target_level_dbfs(-(pProperties->targetLevel / 100));
 if (status != 0) break;
        status = agc->set_compression_gain_db(pProperties->compGain / 100);
 if (status != 0) break;
        status = agc->enable_limiter(pProperties->limiterEnabled);
 break;
 default:
        ALOGW("AgcSetParameter() unknown param %08x value %08x", param, *(uint32_t *)pValue);
        status = -EINVAL;
 break;
 }

    ALOGV("AgcSetParameter() done status %d", status);

 return status;
}
