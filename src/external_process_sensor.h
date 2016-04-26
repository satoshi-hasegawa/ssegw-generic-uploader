#ifndef __EXTERNAL_PROCESS_SENSOR_H__
#define __EXTERNAL_PROCESS_SENSOR_H__

#include "sensor.h"

class ExternalProcessSensor {
 public:
  ExternalProcessSensor(sse_int in_port = DEFAULT_PORT_NO);
  virtual ~ExternalProcessSensor();

 private:
  MoatIOWatcher* watcher_;
  sse_int sock_;
  sse_int port_;

 public:
  static const sse_int DEFAULT_PORT_NO = 10999;
  static const sse_size RECV_BUFF_SIZE = 1024;
};

#endif // __EXTERNAL_PROCESS_SENSOR_H__
