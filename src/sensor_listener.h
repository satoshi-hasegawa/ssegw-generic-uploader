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

#ifndef __SENSOR_LISTENER_H__
#define __SENSOR_LISTENER_H__

class SensorListener {
 public:
  SensorListener() {}
  virtual ~SensorListener(){}
  virtual sse_int callback(MoatObject* in_event) { return SSE_E_OK; }
};

#endif // __SENSOR_LISTENER_H__
