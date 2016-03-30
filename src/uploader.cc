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
  sse_int id;
  MoatObject* collection;
  MoatObjectIterator* it;
  sse_char* json;
  sse_uint json_len;

  err = queue_->peekMessage(max_uploading_messages_, &uploading_);
  ASSERT(err == SSE_E_OK);
  MOAT_OBJECT_DUMP_DEBUG(TAG, uploading_);

  collection = moat_object_new();
  ASSERT(collection);

  it = moat_object_create_iterator(uploading_);
  ASSERT(it);
  while (moat_object_iterator_has_next(it)) {
    sse_char* key;
    MoatObject* ref;
    MoatObject* model_object;

    key = moat_object_iterator_get_next_key(it);
    ASSERT(key);

    err = moat_object_get_object_value(uploading_, key, &ref);
    ASSERT(err == SSE_E_OK);

    err = moat_object_to_json_string(ref, NULL, &json_len);
    ASSERT(err == SSE_E_OK);
    json = (sse_char*)sse_malloc(json_len);
    ASSERT(json);
    err = moat_object_to_json_string(ref, json, &json_len);
    ASSERT(err == SSE_E_OK);

    model_object = moat_object_new();
    ASSERT(model_object);
    err = moat_object_add_string_value(model_object,
                                       const_cast<sse_char*>("value"),
                                       json, json_len,
                                       // Might be bug in case of using moat_object_add_string_value()
                                       // with in_dup = sse_true and string is not NULL terminated.
                                       //sse_false, // No need to free json
                                       sse_true,
                                       sse_false);
    ASSERT(err == SSE_E_OK);
    sse_free(json);
    
    err = moat_object_add_object_value(collection,
                                       key,
                                       model_object,
                                       sse_false, // No need to free model_object
                                       sse_false);
    ASSERT(err == SSE_E_OK);
  }
  moat_object_iterator_free(it);
  
  sse_char* urn = moat_create_notification_id_with_moat(moat_,
                                                        const_cast<sse_char*>("sensor-uploader"),
                                                        const_cast<sse_char*>("1.0.0"));
  ASSERT(urn);

  id = moat_send_notification(moat_,
                              urn,
                              NULL,
                              const_cast<sse_char*>("SensedData"),
                              collection, 
                              Uploader::uploadResultProc,
                              this);
  sse_free(urn);
  moat_object_free(collection);
  if (id < 0) {
    LOG_ERROR("moat_send_notification() failed.");
    return SSE_E_GENERIC;
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
