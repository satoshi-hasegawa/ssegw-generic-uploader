#ifndef __EXTERNAL_PROCESS_SENSOR_H__
#define __EXTERNAL_PROCESS_SENSOR_H__

#include <sseutils.h>
#include "sensor.h"

class ExternalProcessSensor : public Sensor {
 public:
  ExternalProcessSensor(MoatObject &config);
  virtual ~ExternalProcessSensor();
  virtual sse_int start();
  virtual sse_int stop();

 private:
  MoatIOWatcher* watcher_;
  sse_int sock_;
  sse_int port_;
  const sse_char* command_path_;
  TSseUtilCommandShell command_;

 public:
  static const sse_int DEFAULT_PORT_NO = 10999;
  static const sse_size RECV_BUFF_SIZE = 1024;
};

#endif // __EXTERNAL_PROCESS_SENSOR_H__
