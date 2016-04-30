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

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

MessageQueue::MessageQueue() : messages_(NULL),
                               capacity_(DEFAULT_CAPACITY)
{
  messages_ = moat_object_new();
  ASSERT(messages_);
  LOG_DEBUG("Message queue was created, MaxNum = [%d].", capacity_);
}

MessageQueue::~MessageQueue()
{
  if (messages_) {
    moat_object_free(messages_);
    messages_ = NULL;
  }
}

sse_int
MessageQueue::peekMessage(sse_uint in_num, MoatObject** out_messages)
{
  sse_int err;
  MoatObject* object;
  MoatObject* messages;
  MoatObjectIterator* it;

  messages = moat_object_new();
  ASSERT(messages);

  it = moat_object_create_iterator(messages_);
  ASSERT(it);

  for (sse_uint i = in_num; (in_num > 0) && moat_object_iterator_has_next(it); i--) {
    sse_char* key = moat_object_iterator_get_next_key(it);
    ASSERT(key);

    err = moat_object_get_object_value(messages_, key, &object);
    ASSERT(err == SSE_E_OK);

    err = moat_object_add_object_value(messages, key, object, sse_true, sse_false);
    ASSERT(err == SSE_E_OK);
  }
  *out_messages = messages;
  MOAT_OBJECT_DUMP_DEBUG(TAG, *out_messages);
  return SSE_E_OK;
}

sse_int
MessageQueue::pushMessage(MoatObject* in_object)
{
  sse_int err;
  sse_char* uuid;

  ASSERT(in_object);

  if (capacity_ >= countMessage()) {
    LOG_WARN("Message queue is full. (%d/%d)", countMessage(), capacity_);
    return SSE_E_AGAIN;
  }

  uuid = moat_uuid_create_string();
  ASSERT(uuid);

  err = moat_object_add_object_value(messages_,
                                     uuid,
                                     in_object,
                                     sse_true,
                                     sse_false);
  ASSERT(err == SSE_E_OK);

  LOG_DEBUG("Pushed a message into the queue.uuid = [%s].", uuid);
  MOAT_OBJECT_DUMP_DEBUG(TAG, in_object);

  sse_free(uuid);
  return SSE_E_OK;
}

sse_int
MessageQueue::deleteMessage(const sse_char* in_uuid)
{
  ASSERT(in_uuid);
  moat_object_remove_value(messages_, const_cast<sse_char*>(in_uuid));
  LOG_DEBUG("Delete message = [%s].", in_uuid);
  return SSE_E_OK;
}

sse_uint
MessageQueue::countMessage()
{
  return moat_object_get_length(messages_);
}
