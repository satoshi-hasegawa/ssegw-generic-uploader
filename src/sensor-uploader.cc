#include <servicesync/moat.h>
#include "external_process_sensor.h"
#include "periodic_uploader.h"

const sse_char MODEL_NAME_SENSED_DATA[] = "SensedData";

// workaround for "failed to dlsym() [./<PACKAGE_ID>.so: undefined symbol: moat_app_main]"
extern "C" {
  sse_int moat_app_main(sse_int in_argc, sse_char *argv[]);
}

#define TAG "Uploader"
#define LOG_ERROR(format, ...) MOAT_LOG_ERROR(TAG, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  MOAT_LOG_WARN(TAG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  MOAT_LOG_INFO(TAG, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MOAT_LOG_DEBUG(TAG, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) MOAT_LOG_TRACE(TAG, format, ##__VA_ARGS__)
#include <stdlib.h>
#define ASSERT(cond) if(!(cond)) { LOG_ERROR("ASSERTION FAILED:" #cond); abort(); }

sse_int
moat_app_main(sse_int in_argc, sse_char *argv[])
{
  Moat moat = NULL;
  sse_int err = SSE_E_OK;
  ModelMapper mapper;
  MoatObject* config;
  MoatObject* config_sensor;
  MoatObject* config_uploader;
  const sse_char* config_file_path = "config.json";

  err = moat_init(argv[0], &moat);
  if (err != SSE_E_OK) {
    goto error_exit;
  }

  /* Load configurations */
  err = moat_json_file_to_moat_object(const_cast<sse_char*>(config_file_path),
                                      &config,
                                      NULL);
  if (err != SSE_E_OK) {
    LOG_ERROR("Configuration file, %s was not found.", config_file_path);
    goto error_exit;
  }
  MOAT_OBJECT_DUMP_DEBUG(TAG, config);

  err = moat_object_get_object_value(config,
                                     const_cast<sse_char*>("dummy_sensor"),
                                     &config_sensor);
  if (err != SSE_E_OK) {
    LOG_ERROR("Configuration for dummy_sonsor was not found.");
    goto error_exit;
  }

  err = moat_object_get_object_value(config,
                                     const_cast<sse_char*>("uploader"),
                                     &config_uploader);
  if (err != SSE_E_OK) {
    LOG_ERROR("Configuration for uploader was not found.");
    goto error_exit;
  }


  /* setup event handlers, timers, etc */
  /* register models */
  mapper.AddProc = NULL;
  mapper.RemoveProc = NULL;
  mapper.UpdateProc = NULL;
  mapper.UpdateFieldsProc = NULL;
  mapper.FindAllUidsProc = NULL;
  mapper.FindByUidProc = NULL;
  mapper.CountProc = NULL;
  err = moat_register_model(moat, const_cast<sse_char*>(MODEL_NAME_SENSED_DATA), &mapper, NULL); // FIXME
  if (err != SSE_E_OK) {
    LOG_ERROR("failed to register model.");
    goto error_exit;
  }

  {
    ExternalProcessSensor sensor(*config_sensor);
    PeriodicUploader uploader(moat, *config_uploader);

    sensor.addListener(&uploader);
    sensor.start();

    err = moat_run(moat);
    if (err != SSE_E_OK) {
      goto error_exit;
    }
  }

  /* unregister models */
  moat_unregister_model(moat, const_cast<sse_char*>(MODEL_NAME_SENSED_DATA));
  /* teardown */

  moat_destroy(moat);
  return SSE_E_OK;

 error_exit:
  if (moat != NULL) {
    moat_destroy(moat);
  }
  return err;
}
