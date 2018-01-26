#ifndef LRT_RCOMM_RCOMM_H
#define LRT_RCOMM_RCOMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct RCommHandle RCommHandle;

typedef enum RCommStatus {
  RCommStatus_Ok,
  RCommStatus_NoCounterpart,
  RCommStatus_ConnectionFailed,
  RCommStatus__Count
} RCommStatus;

const char* RCommStatusNames[RCommStatus__Count] = { "Okay",
                                                     "No Counterpart",
                                                     "Connection Failed" };

const char*
rcomm_status_msg(RCommStatus status);

RCommHandle*
rcomm_handle_create();

RCommStatus
rcomm_handle_free(RCommHandle* handle);

RCommStatus
rcomm_handle_connect(RCommHandle* handle, const char* pipe);

RCommStatus
rcomm_handle_disconnect(RCommHandle* handle);

// Registry Interface
// ==================

void
rcomm_handle_subscribe(RCommHandle* handle, uint16_t type, uint16_t property);
void
rcomm_handle_unsubscribe(RCommHandle* handle, uint16_t type, uint16_t property);
void
rcomm_handle_request(RCommHandle* handle, uint16_t type, uint16_t property);

int8_t
rcomm_handle_get_Int8(RCommHandle* handle, uint16_t property);
int16_t
rcomm_handle_get_Int16(RCommHandle* handle, uint16_t property);
int32_t
rcomm_handle_get_Int32(RCommHandle* handle, uint16_t property);
int64_t
rcomm_handle_get_Int64(RCommHandle* handle, uint16_t property);
uint8_t
rcomm_handle_get_Uint8(RCommHandle* handle, uint16_t property);
uint16_t
rcomm_handle_get_Uint16(RCommHandle* handle, uint16_t property);
uint32_t
rcomm_handle_get_Uint32(RCommHandle* handle, uint16_t property);
uint64_t
rcomm_handle_get_Uint64(RCommHandle* handle, uint16_t property);
float
rcomm_handle_get_Float(RCommHandle* handle, uint16_t property);
double
rcomm_handle_get_Double(RCommHandle* handle, uint16_t property);
bool
rcomm_handle_get_Bool(RCommHandle* handle, uint16_t property);

void
rcomm_handle_set_Int8(RCommHandle* handle, uint16_t property, int8_t value);
void
rcomm_handle_set_Int16(RCommHandle* handle, uint16_t property, int16_t value);
void
rcomm_handle_set_Int32(RCommHandle* handle, uint16_t property, int32_t value);
void
rcomm_handle_set_Int64(RCommHandle* handle, uint16_t property, int64_t value);
void
rcomm_handle_set_Uint8(RCommHandle* handle, uint16_t property, uint8_t value);
void
rcomm_handle_set_Uint16(RCommHandle* handle, uint16_t property, uint16_t value);
void
rcomm_handle_set_Uint32(RCommHandle* handle, uint16_t property, uint32_t value);
void
rcomm_handle_set_Uint64(RCommHandle* handle, uint16_t property, uint64_t value);
void
rcomm_handle_set_Float(RCommHandle* handle, uint16_t property, float value);
void
rcomm_handle_set_Double(RCommHandle* handle, uint16_t property, double value);
void
rcomm_handle_set_Bool(RCommHandle* handle, uint16_t property, bool value);

#ifdef __cplusplus
}
#include <RRegistry/Registry.hpp>
#include <memory>

std::shared_ptr<lrt::rregistry::Registry>
rcomm_handle_get_registry(RCommHandle* handle);

#endif

#endif
