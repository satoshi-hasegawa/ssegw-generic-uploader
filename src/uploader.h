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

#ifndef __UPLOADER_H__
#define __UPLOADER_H__

#include "sensor_listener.h"
class MessageQueue;

class Uploader : public SensorListener {
 public:
  Uploader();
  virtual ~Uploader();
  virtual sse_int callback(MoatObject* in_event);
  virtual sse_int upload();
  virtual void onComplete();
  static void uploadResultProc(Moat in_moat,
                               sse_char *in_urn,
                               sse_char *in_model_name,
                               sse_int in_request_id,
                               sse_int in_result,
                               sse_pointer in_user_data);
  sse_uint getMaxUplodingMessages() const { return max_uploading_messages_; }

 private:
  MessageQueue* queue_;
  MoatObject* uploading_;
  sse_uint max_uploading_messages_;
  static const sse_uint DEFAULT_MAX_UPLOADING_MESSAGES = 32;
};

#endif /* __UPLOADER_H__ */
