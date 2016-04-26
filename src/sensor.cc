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
#include "sensor.h"
#include "sensor_listener.h"

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

Sensor::Sensor() : listeners_(NULL)
{
}

Sensor::~Sensor()
{
  //TODO free listeners
}

sse_int 
Sensor::onEvent(MoatObject* in_event)
{
  LOG_DEBUG("On event [%p]", in_event);

  for (SSESList* it = listeners_; it != NULL; it = sse_slist_next(it)) {
    LOG_INFO("exec callback");
    SensorListener* listener = static_cast<SensorListener*>(sse_slist_data(it));
    listener->callback(in_event);
  }
  return SSE_E_OK;
}

sse_int
Sensor::addListener(SensorListener* in_listener)
{
  SSESList* p = sse_slist_add(listeners_, in_listener);
  if (p == NULL) {
    LOG_ERROR("Adding a link to the link list of event listeners has been failed.");
    return SSE_E_GENERIC;
  }
  listeners_ = p;
  return SSE_E_OK;
}

sse_int
Sensor::removeListener(SensorListener* in_listener)
{
  //TODO
  return SSE_E_OK;
}
