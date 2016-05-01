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

#ifndef __PERIODIC_UPLOADER_H__
#define __PERIODIC_UPLOADER_H__

#include "uploader.h"

class PeriodicUploader : public Uploader {
 public:
  PeriodicUploader(Moat in_moat, MoatObject& in_config);
  virtual ~PeriodicUploader();
  const sse_uint getInterval() const { return interval_; }
  void setInterval(sse_uint in_interval) { interval_ = in_interval; }
  static sse_bool onTimerExpired(sse_int in_timer_id, sse_pointer in_user_data);

 private:
  sse_uint interval_;
  MoatTimer* timer_;
  sse_int timer_id_;
  static const sse_uint DEFAULT_INTERVAL = 60;
};

#endif //__PERIODIC_UPLOADER_H__
