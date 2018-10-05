/******************************************************************************
 *
 *  Copyright (C) 2018 ST Microelectronics S.A.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 ******************************************************************************/

/**
 * \addtogroup spi_libese
 * \brief ESE Lib layer interface to application
 * @{ */

#ifndef _STESEAPI_H_
#define _STESEAPI_H_

#include <stdint.h>

/**
 * \ingroup spi_libese
 * \brief Ese data buffer
 *
 */
typedef struct StEse_data {
  uint8_t len;    /*!< length of the buffer */
  uint8_t* p_data; /*!< pointer to a buffer */
} StEse_data;

typedef enum {
  ESESTATUS_SUCCESS,
  ESESTATUS_FAILED,
  ESESTATUS_INVALID_STATE = (0x0011),

  ESESTATUS_NOT_INITIALISED = (0x0031),

  ESESTATUS_ALREADY_INITIALISED = (0x0032),

  ESESTATUS_FEATURE_NOT_SUPPORTED = (0x0033),

  ESESTATUS_CONNECTION_SUCCESS = (0x0046),

  ESESTATUS_CONNECTION_FAILED = (0x0047),
  ESESTATUS_BUSY = (0x006F),

  ESESTATUS_INVALID_REMOTE_DEVICE = (0x001D),

  ESESTATUS_READ_FAILED = (0x0014),

  ESESTATUS_WRITE_FAILED = (0x0015),
  ESESTATUS_UNKNOWN_ERROR = (0x00FE),

  ESESTATUS_INVALID_PARAMETER = (0x00FF),
} ESESTATUS;

typedef enum {
  ESE_STATUS_CLOSE = 0x00,
  ESE_STATUS_BUSY,
  ESE_STATUS_RECOVERY,
  ESE_STATUS_IDLE,
  ESE_STATUS_OPEN,
} SpiEse_status;

/* SPI Control structure */
typedef struct ese_Context {
  SpiEse_status EseLibStatus; /* Indicate if Ese Lib is open or closed */
  void* pDevHandle;

  uint8_t p_read_buff[260];
  uint16_t cmd_len;
  uint8_t p_cmd_data[260];
} ese_Context_t;

/******************************************************************************
 * \ingroup spi_libese
 *StEse_init
 * \brief  It Initializes protocol stack instance variables
 *
 * \retval This function return ESESTATUS_SUCCES (0) in case of success
 *         In case of failure returns other failure value.
 *
 ******************************************************************************/
ESESTATUS StEse_init();



/******************************************************************************
 * \ingroup StEse_spiIoctl
 *
 * \brief  This function is used to communicate from nfc-hal to ese-hal
 *
 * \retval This function return ESESTATUS_SUCCES (0) in case of success
 *         In case of failure returns other failure value.
 *
 ******************************************************************************/
ESESTATUS StEse_spiIoctl(uint64_t ioctlType, void* p_data);




/**
 * \ingroup StEse_Transceive
 * \brief This function prepares the C-APDU, send to ESE and then receives the
 *response from ESE,
 *         decode it and returns data.
 *
 * \param[in]       phStEse_data: Command to ESE
 * \param[out]     phStEse_data: Response from ESE (Returned data to be freed
 *after copying)
 *
 * \retval ESESTATUS_SUCCESS On Success ESESTATUS_SUCCESS else proper error code
 *
 */

ESESTATUS StEse_Transceive(StEse_data* pCmd, StEse_data* pRsp);

/******************************************************************************
 * \ingroup StEse_deInit
 *
 * \brief  This function is called by Jni/phStEse_close during the
 *         de-initialization of the ESE. It de-initializes protocol stack
 *instance variables
 *
 * \retval This function return ESESTATUS_SUCCES (0) in case of success
 *         In case of failure returns other failure value.
 *
 ******************************************************************************/
ESESTATUS StEse_deInit(void);

/**
 * \ingroup StEse_close
 * \brief This function close the ESE interface and free all resources.
 *
 * \param[in]       void
 *
 * \retval ESESTATUS_SUCCESS Always return ESESTATUS_SUCCESS (0).
 *
 */

bool StEseApi_isOpen();

ESESTATUS StEse_close(void);


#endif /* _STESEAPI_H_ */
