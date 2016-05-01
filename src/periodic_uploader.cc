/*
 * LEGAL NOTICE
 *
 * Copyright (C) 2015 InventIt Inc. All rights reserved.
 *
 * This source code, product and/or document is protected under licenses
 * restricting its use, copying, distribution, and decompilation.
 * No part of this source code, product or document may be reproduced in
 * any form by any means without prior written authorization of InventIt Inc.
 * and its licensors, if any.
 *
 * InventIt Inc.
 * 9F KOJIMACHI CP BUILDING
 * 4-4-7 Kojimachi, Chiyoda-ku, Tokyo 102-0083
 * JAPAN
 * http://www.yourinventit.com/
 */

#include <servicesync/moat.h>
#include "periodic_uploader.h"

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

PeriodicUploader::PeriodicUploader(Moat in_moat,
                                   MoatObject& in_config) : Uploader(in_moat),
                                                            interval_(DEFAULT_INTERVAL),
                                                            timer_(NULL),
                                                            timer_id_(-1)
{
  sse_int err;
  sse_int64 interval;

  err = moat_object_get_int64_value(&in_config,
                                     const_cast<sse_char*>("interval"),
                                     &interval);
  if (err == SSE_E_INVAL) {
    LOG_WARN("moat_object_get_object_value() failed with [%s].", sse_get_error_string(err));
    sse_double tmp;
    err = moat_object_get_double_value(&in_config,
                                       const_cast<sse_char*>("interval"),
                                       &tmp);
    ASSERT(err == SSE_E_OK);
    interval_ = static_cast<sse_int>(tmp);
  } else if (err == SSE_E_OK) {
    interval_ = static_cast<sse_int>(interval);
  }
  LOG_INFO("Periodic uploader interval = [%d].", interval_);

  timer_ = moat_timer_new();
  ASSERT(timer_);

  timer_id_ = moat_timer_set(timer_,
                             interval_,
                             PeriodicUploader::onTimerExpired,
                             this);
  ASSERT(timer_id_ > 0);

}

PeriodicUploader::~PeriodicUploader()
{
  moat_timer_free(timer_);
}

sse_bool
PeriodicUploader::onTimerExpired(sse_int in_timer_id, sse_pointer in_user_data)
{
  PeriodicUploader* self = static_cast<PeriodicUploader*>(in_user_data);
  ASSERT(self);

  //FIXME
  //return sse_false;

  LOG_INFO("Timer has been expired.");
  self->upload();
  return sse_true;
}

