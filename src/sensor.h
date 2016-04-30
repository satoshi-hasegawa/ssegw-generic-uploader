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

#ifndef __SENSOR_H__
#define __SENSOR_H__

class SensorListener;

class Sensor {
 public:
  Sensor();
  Sensor(MoatObject &config) {}
  virtual ~Sensor();
  virtual sse_int start() { return SSE_E_OK;  }
  virtual sse_int stop() { return SSE_E_OK; }
  virtual sse_int addListener(SensorListener* in_listener);
  virtual sse_int removeListener(SensorListener* in_listener);
  virtual sse_int onEvent(MoatObject* in_event);

 protected:
  SSESList* listeners_;
};

#endif //__SENSOR_H__
