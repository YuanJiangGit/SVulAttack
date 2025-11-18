ALWAYS_INLINE error::Error GLES2DecoderImpl::DoMultiDrawArrays(
    const char* function_name,
    bool instanced,
    GLenum mode,
    const GLint* firsts,
    const GLsizei* counts,
    const GLsizei* primcounts,
    GLsizei drawcount) {
  error::Error error = WillAccessBoundFramebufferForDraw();
  if (error != error::kNoError)
    return error;

  if (!validators_->draw_mode.IsValid(mode)) {
    LOCAL_SET_GL_ERROR_INVALID_ENUM(function_name, mode, "mode");
    return error::kNoError;
  }

  if (drawcount < 0) {
    LOCAL_SET_GL_ERROR(GL_INVALID_VALUE, function_name, "drawcount < 0");
    return error::kNoError;
  }

  if (!CheckBoundDrawFramebufferValid(function_name, true)) {
    return error::kNoError;
  }

  GLuint total_max_vertex_accessed = 0;
  GLsizei total_max_primcount = 0;
  if (!CheckMultiDrawArraysVertices(
          function_name, instanced, firsts, counts, primcounts, drawcount,
          &total_max_vertex_accessed, &total_max_primcount)) {
    return error::kNoError;
  }

  if (total_max_primcount == 0) {
    return error::kNoError;
  }

  GLsizei transform_feedback_vertices = 0;
  if (feature_info_->IsWebGL2OrES3Context()) {
    if (!AttribsTypeMatch()) {
      LOCAL_SET_GL_ERROR(GL_INVALID_OPERATION, function_name,
                         "vertexAttrib function must match shader attrib type");
      return error::kNoError;
    }

    if (!CheckTransformFeedback(function_name, instanced, mode, counts,
                                primcounts, drawcount,
                                &transform_feedback_vertices)) {
      return error::kNoError;
    }

    if (!ValidateUniformBlockBackings(function_name)) {
      return error::kNoError;
    }
  }

  if (!ClearUnclearedTextures()) {
    LOCAL_SET_GL_ERROR(GL_INVALID_VALUE, function_name, "out of memory");
    return error::kNoError;
  }

  bool simulated_attrib_0 = false;
  if (!SimulateAttrib0(function_name, total_max_vertex_accessed,
                       &simulated_attrib_0)) {
    return error::kNoError;
  }
  bool simulated_fixed_attribs = false;
  if (SimulateFixedAttribs(function_name, total_max_vertex_accessed,
                           &simulated_fixed_attribs, total_max_primcount)) {
    bool textures_set;
    if (!PrepareTexturesForRender(&textures_set, function_name)) {
      return error::kNoError;
    }
    ApplyDirtyState();
    if (!ValidateAndAdjustDrawBuffers(function_name)) {
      return error::kNoError;
    }

    GLint draw_id_location = state_.current_program->draw_id_uniform_location();
    for (GLsizei draw_id = 0; draw_id < drawcount; ++draw_id) {
      GLint first = firsts[draw_id];
      GLsizei count = counts[draw_id];
      GLsizei primcount = instanced ? primcounts[draw_id] : 1;
      if (count == 0 || primcount == 0) {
        continue;
      }
      if (draw_id_location >= 0) {
        api()->glUniform1iFn(draw_id_location, draw_id);
      }
      if (!instanced) {
        api()->glDrawArraysFn(mode, first, count);
      } else {
        api()->glDrawArraysInstancedANGLEFn(mode, first, count, primcount);
      }
    }
    if (state_.bound_transform_feedback.get()) {
      state_.bound_transform_feedback->OnVerticesDrawn(
          transform_feedback_vertices);
    }

    if (textures_set) {
      RestoreStateForTextures();
    }
    if (simulated_fixed_attribs) {
      RestoreStateForSimulatedFixedAttribs();
    }
  }
  if (simulated_attrib_0) {
    RestoreStateForAttrib(0, false);
  }
  return error::kNoError;
}
