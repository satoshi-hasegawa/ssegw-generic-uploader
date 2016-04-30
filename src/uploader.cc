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
#include "queue.h"
#include "uploader.h"

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

Uploader::Uploader() : max_uploading_messages_(DEFAULT_MAX_UPLOADING_MESSAGES)
{
  try {
    queue_ = new MessageQueue;
  }
  catch (int e) {
    ASSERT(!"Creating a message queue failed.");
  }
}

Uploader::~Uploader()
{
  delete queue_;
}

sse_int
Uploader::callback(MoatObject* in_event)
{
  MOAT_OBJECT_DUMP_DEBUG(TAG, in_event);
  queue_->pushMessage(in_event);
  return SSE_E_OK;
}

sse_int
Uploader::upload()
{
  MoatObject* object;

  queue_->peekMessage(max_uploading_messages_, &object);
  uploading_ = object;
  MOAT_OBJECT_DUMP_DEBUG(TAG, uploading_);

  return SSE_E_GENERIC;
}

void
Uploader::onComplete()
{
  //FIXME
}

void
Uploader::uploadResultProc(Moat in_moat,
                           sse_char *in_urn,
                           sse_char *in_model_name,
                           sse_int in_request_id,
                           sse_int in_result,
                           sse_pointer in_user_data)
{
  //FIXME
}
