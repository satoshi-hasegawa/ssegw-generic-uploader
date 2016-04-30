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

#ifndef __QUEUE_H__
#define __QUEUE_H__

class MessageQueue {
 public:
  MessageQueue();
  virtual ~MessageQueue();

  virtual sse_int peekMessage(sse_uint in_num, MoatObject** out_messages);
  virtual sse_int pushMessage(MoatObject* in_object);
  virtual sse_int deleteMessage(const sse_char* in_uuid);
  virtual sse_uint countMessage();

 private:
  MoatObject* messages_;
  sse_uint capacity_;
  const static sse_uint DEFAULT_CAPACITY = 32;
};

#endif /* __QUEUE_H__ */
