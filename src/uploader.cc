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

Uploader::Uploader(Moat in_moat) : moat_(in_moat),
                                   max_uploading_messages_(DEFAULT_MAX_UPLOADING_MESSAGES)
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
  sse_int err;
  MoatObject* messages;
  MoatObject* collection;
  MoatObjectIterator* it;
  sse_char* json;
  sse_uint json_len;

  queue_->peekMessage(max_uploading_messages_, &messages);
  uploading_ = messages;
  MOAT_OBJECT_DUMP_DEBUG(TAG, uploading_);

  collection = moat_object_new();
  ASSERT(collection);

  it = moat_object_create_iterator(messages);
  ASSERT(it);
  while (moat_object_iterator_has_next(it)) {
    sse_char* key;
    MoatObject* tmp;
    MoatObject* tmp2;

    key = moat_object_iterator_get_next_key(it);
    ASSERT(key);

    err = moat_object_get_object_value(messages, key, &tmp);
    ASSERT(err == SSE_E_OK);

    tmp2 = moat_object_new(); // No need to free
    ASSERT(tmp2);
    err = moat_object_to_json_string(tmp, NULL, &json_len);
    ASSERT(err == SSE_E_OK);
    json = (sse_char*)sse_malloc(json_len);
    ASSERT(json);
    err = moat_object_to_json_string(tmp, json, &json_len);
    ASSERT(err == SSE_E_OK);

    err = moat_object_add_string_value(tmp2, const_cast<sse_char*>("value"), json, json_len, sse_false, sse_false);
    ASSERT(err == SSE_E_OK);
    
    err = moat_object_add_object_value(collection, key, tmp2, sse_true, sse_false);
    if (err != SSE_E_OK) {
      LOG_ERROR("moat_object_add_object_value() failed with [%s].", sse_get_error_string(err));
      ASSERT(err == SSE_E_OK);
    }
  }

  sse_char* urn = moat_create_notification_id_with_moat(moat_,
                                                        const_cast<sse_char*>("sensor-uploader"),
                                                        const_cast<sse_char*>("1.0.0"));
  ASSERT(urn);

  err = moat_send_notification(moat_,
                               urn,
                               NULL,
                               const_cast<sse_char*>("SensedData"),
                               collection, 
                               Uploader::uploadResultProc,
                               this);
  sse_free(urn);
  moat_object_free(collection);
  if (err != SSE_E_OK) {
    LOG_ERROR("moat_send_notification() failed with [%s].", sse_get_error_string(err));
    return err;
  }

  return SSE_E_OK;
}

void
Uploader::onComplete(sse_int in_result)
{
  if (in_result == SSE_E_OK) {
    LOG_INFO("Uploading sensor data has been completed successfully.");

    MoatObjectIterator* it = moat_object_create_iterator(uploading_);
    ASSERT(it);
    while (moat_object_iterator_has_next(it)) {
      sse_char* key = moat_object_iterator_get_next_key(it);
      ASSERT(key);
      LOG_INFO("Delete message = [%s].", key);
      queue_->deleteMessage(key);
    }

  } else {
    LOG_ERROR("Uploading sensor data failed with [%s].", sse_get_error_string(in_result));
  }
  moat_object_free(uploading_);
  uploading_ = NULL;
}

void
Uploader::uploadResultProc(Moat in_moat,
                           sse_char *in_urn,
                           sse_char *in_model_name,
                           sse_int in_request_id,
                           sse_int in_result,
                           sse_pointer in_user_data)
{
  Uploader* uploader;

  uploader = static_cast<Uploader*>(in_user_data);
  ASSERT(uploader);

  uploader->onComplete(in_result);
}
