/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef ULONGLONG hcl_deadline;
#else
#include <time.h>
typedef struct timespec hcl_deadline;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hcl_status_code {
    HCL_STATUS_OK = 0,
    HCL_STATUS_TIMEOUT,
    HCL_STATUS_TRANSPORT_ERROR,
    HCL_STATUS_PROTOCOL_ERROR,
    HCL_STATUS_UNSUPPORTED,
    HCL_STATUS_INVALID_ARGUMENT,
    HCL_STATUS_NOT_CONNECTED,
    HCL_STATUS_OUT_OF_RANGE,
    HCL_STATUS_ALLOCATION_FAILED,
    HCL_STATUS_INTERNAL_ERROR
} hcl_status_code;

typedef struct hcl_register_context {
    uint32_t base_address;
    uint32_t offset;
    uint32_t value;
} hcl_register_context;

typedef struct hcl_status {
    hcl_status_code code;
    hcl_register_context ctx;
    bool has_context;
} hcl_status;

static inline hcl_register_context hcl_make_register_context(uint32_t base_address,
                                                             uint32_t offset,
                                                             uint32_t value) {
    hcl_register_context register_context = {base_address, offset, value};
    return register_context;
}

static inline hcl_status hcl_status_ok(void) {
    hcl_status status = {HCL_STATUS_OK, {0u, 0u, 0u}, false};
    return status;
}

static inline hcl_status hcl_status_error(hcl_status_code code,
                                          const hcl_register_context* context) {
    hcl_status status = {code, {0u, 0u, 0u}, false};
    if (context != NULL) {
        status.ctx = *context;
        status.has_context = true;
    }
    return status;
}

static inline bool hcl_status_is_ok(const hcl_status* status) {
    return status != NULL && status->code == HCL_STATUS_OK;
}

static inline void hcl_delay_ms(uint32_t milliseconds) {
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    struct timespec delay;
    delay.tv_sec = (time_t)(milliseconds / 1000u);
    delay.tv_nsec = (long)((milliseconds % 1000u) * 1000000L);
    nanosleep(&delay, NULL);
#endif
}

static inline void hcl_compute_deadline(uint32_t timeout_ms, hcl_deadline* out) {
#ifdef _WIN32
    *out = GetTickCount64() + (ULONGLONG)timeout_ms;
#else
    clock_gettime(CLOCK_MONOTONIC, out);
    out->tv_sec += (time_t)(timeout_ms / 1000u);
    out->tv_nsec += (long)((timeout_ms % 1000u) * 1000000L);
    if (out->tv_nsec >= 1000000000L) {
        out->tv_sec += 1;
        out->tv_nsec -= 1000000000L;
    }
#endif
}

static inline bool hcl_deadline_exceeded(const hcl_deadline* deadline) {
#ifdef _WIN32
    return GetTickCount64() >= *deadline;
#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec > deadline->tv_sec ||
           (now.tv_sec == deadline->tv_sec && now.tv_nsec >= deadline->tv_nsec);
#endif
}

#ifdef __cplusplus
}
#endif