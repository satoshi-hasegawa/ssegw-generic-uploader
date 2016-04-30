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

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <servicesync/moat.h>
#include <sseutils.h>
#include "external_process_sensor.h"

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

static void
on_recv_data(MoatIOWatcher *in_watcher,
             sse_pointer in_user_data,
             sse_int in_desc,
             sse_int in_event_flags)
{
  sse_int err;
  struct sockaddr_in si;
  socklen_t si_len = sizeof(si);
  sse_char buf[ExternalProcessSensor::RECV_BUFF_SIZE];
  sse_int recv_len;
  MoatObject* obj;

  sse_memset(buf, 0, sizeof(buf));
  recv_len = recvfrom(in_desc, buf, sizeof(buf) -1, 0, (struct sockaddr *)&si, &si_len);
  if (recv_len == -1) {
    LOG_ERROR("recvfrom() failed.");
    return;
  }
  LOG_INFO("Received packet from %s:%d", inet_ntoa(si.sin_addr), ntohs(si.sin_port));

  err = moat_json_string_to_moat_object(buf, sse_strlen(buf), &obj, NULL);
  if (err != SSE_E_OK) {
    LOG_ERROR("moat_json_string_to_moat_object() failed with [%s].", sse_get_error_string(err));
    return;
  }
  MOAT_OBJECT_DUMP_INFO(TAG, obj);

  return;
}

ExternalProcessSensor::ExternalProcessSensor(MoatObject &config) : watcher_(NULL),
                                                                   sock_(-1),
                                                                   port_(DEFAULT_PORT_NO),
                                                                   command_path_(NULL)
{
  sse_int ret;
  struct sockaddr_in si;
  sse_int64 port;
  sse_char* bin_path;
  sse_uint bin_path_len;

  // Load configurations
  ret = moat_object_get_int64_value(&config, const_cast<sse_char*>("port"), &port);
  if (ret != SSE_E_OK) {
    LOG_WARN("moat_object_get_int64_value() failed with [%s].", sse_get_error_string(ret));
    sse_double tmp;
    ret = moat_object_get_double_value(&config, const_cast<sse_char*>("port"), &tmp);
    ASSERT(ret == SSE_E_OK);
    port_ = static_cast<sse_int>(tmp);
  } else {
    port_ = static_cast<sse_int>(port);
  }
  LOG_DEBUG("Port number = [%d].", port_);

  ret = moat_object_get_string_value(&config, const_cast<sse_char*>("bin"), &bin_path, &bin_path_len);
  ASSERT(ret == SSE_E_OK);
  command_path_ = sse_strndup(bin_path, bin_path_len);
  LOG_DEBUG("Binary path = [%s].", command_path_);
  

  // Open socket for communicating with child process.
  sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ASSERT(sock_ != -1);

  sse_memset(&si, 0, sizeof(si));
  si.sin_family = AF_INET;
  si.sin_port = htons(port_);
  si.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = bind(sock_, (struct sockaddr*)&si, sizeof(si));
  ASSERT(ret != -1);

  watcher_ = moat_io_watcher_new(sock_, on_recv_data, NULL, MOAT_IO_FLAG_READ);
  ASSERT(watcher_);
  ret = moat_io_watcher_start(watcher_);
  ASSERT(ret == SSE_E_OK);
}

ExternalProcessSensor::~ExternalProcessSensor()
{
  if (sock_ > -1) ::close(sock_);
  if (watcher_) moat_io_watcher_free(watcher_);
}

sse_int
ExternalProcessSensor::start()
{
  sse_char port[32];

  LOG_INFO("Starting [%s]", command_path_);
  command_.SetCommand(command_path_);
  command_.AddArgument("-p");
  sse_itoa(port_, port);
  command_.AddArgument(port);
  command_.Execute();
  return SSE_E_OK;
}

sse_int
ExternalProcessSensor::stop()
{
  //FIXME
  return SSE_E_OK;
}

