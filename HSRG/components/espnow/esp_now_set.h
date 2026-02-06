#pragma once

#include "freertos/queue.h"
#include <inttypes.h>
#include "esp_config_basic.h"
#include "esp_now.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_now_set.h"
#include "esp_mac.h"
#include "esp_crc.h"

// espnowQueue 외부 변수 선언
extern QueueHandle_t espnowQueue;

/**
 * @brief init wifi func
 * @param[in] None
 * @retval None
 */
void wifi_init();

/**
 * @brief parameters transfer func
 * @param[in] const esp_now_recv_info_t *recv_info, const uint8_t *data, int len
 * @retval None
 */
void espnow_recv_cb_idf5(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);

/** 
 * @brief receive the data for slave esp
 * @param[in] const uint8_t *mac_addr, const uint8_t *data, int data_len
 * @retval None
 * @note need to know MAC addr (and config)
 */
void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len);

/**
 * @brief parse the espnow snet data
 * @param[in] uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic
 * @retval int
 */
int espnow_data_parse(uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic);

/**
 * @brief ESP now main task
 * @param[in] void *pvParameter
 * @retval None
 */
void espnow_task(void *pvParameter);

/**
 * @brief deinit the espnowQueue
 * @param[in] None
 * @retval None
 */
void espnow_deinit(void);

/**
 * @brief set up basic espnow config
 * @param[in] None
 * @retval void
 * @note enroll the cb func to register 
 */
void espnow_init(void);