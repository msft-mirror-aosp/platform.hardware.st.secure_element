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
#define LOG_TAG "StEse-SpiLayerInterface"
#include "SpiLayerInterface.h"
#include <sys/time.h>
#include "SpiLayerComm.h"
#include "SpiLayerDriver.h"
#include "T1protocol.h"
#include "android_logmsg.h"
#include "utils-lib/Atp.h"
#include <errno.h>
#include <string.h>

#define SPI_BITS_PER_WORD 8
#define SPI_MODE SPI_MODE_0

#define KHZ_TO_HZ 1000

/*******************************************************************************
**
** Function         SpiLayerInterface_init
**
** Description      Initialize the SPI link access
**
** Parameters       tSpiDriver     - hardware information
**
** Returns          0 if connection could be initialized, -1 otherwise.
**
*******************************************************************************/
int SpiLayerInterface_init(SpiDriver_config_t* tSpiDriver) {
  STLOG_HAL_D("Initializing SPI Driver interface...");

  // Configure the SPI before start the data exchange with the eSE
  char* spiDevPath = tSpiDriver->pDevName;
  if (SpiLayerDriver_open(spiDevPath) != 0) {
    // Error configuring the SPI bus
    STLOG_HAL_E("Error configuring the SPI bus.");
    return -1;
  }
  // Check if ATP file exists
  // If it exists, it means that ATP was previously read and hence we do not
  // need to read it again
  STLOG_HAL_D("check ATP file presence : %s", ATP_FILE_PATH);
  if (fopen(ATP_FILE_PATH, "rb")) {
    STLOG_HAL_V("ATP file exists.");
    // ATP is stored in the file: load the config from the file and return.
    SpiLayerComm_readAtpFromFile();
    return 0;
  } else {
    STLOG_HAL_V("Cannot open ATP file: %d, %s", errno, strerror(errno));
  }

  // First of all, read the ATP from the slave
  if (SpiLayerComm_readAtp() != 0) {
    // Error reading the ATP
    STLOG_HAL_E("Error reading the ATP.");
    return -1;
  }

  STLOG_HAL_D("SPI bus working at ATP.msf =  %i KHz", ATP.msf);
  /*  if(ATP.msf < actualFreq) {
        actualFreq = ATP.msf;
        if(SpiLayerDriver_setMaxFreqSpeed(actualFreq * KHZ_TO_HZ) != 0) {
            ALOGE("Error setting frequency.");
            return -1;
        }
    }*/

  // pollInterval = PreferenceHelper_getPollInterval();
  // ALOGD(TAG, "Poll interval read from preferences: %d nanoseconds",
  // pollInterval);

  STLOG_HAL_D("SPI Driver interface initialized.");
  return 0;
}

/*******************************************************************************
**
** Function         SpiLayerInterface_transcieveTpdu
**
** Description       Sends a TPDU to the SE, waits for the response
**                   and returns it.
**
** Parameters       cmdTpdu    -The TPDU to be sent.
**                  respTpdu   -The memory position where to store the response.
**                  numberOfBwt-The maximum number of BWT to wait.
**
** Returns          bytesRead if data was read, 0 if timeout expired with
**                  no response, -1 otherwise
**
*******************************************************************************/
int SpiLayerInterface_transcieveTpdu(Tpdu* cmdTpdu, Tpdu* respTpdu,
                                     int numberOfBwt) {
  // Send the incoming Tpdu to the slave
  if (SpiLayerComm_writeTpdu(cmdTpdu) < 0) {
    return -1;
  }

  if (numberOfBwt <= 0) {
    STLOG_HAL_W("Buffer overflow happened, restoring numberOfBwt");
    numberOfBwt = DEFAULT_NBWT;
  }
  // Wait for response
  int result = SpiLayerComm_waitForResponse(respTpdu, numberOfBwt);

  // Unable to receive the response from slave
  if (result == -1) {
    return -1;
  } else if (result == -2) {
    // 0 bytes read
    return 0;
  }

  // Read the response
  int bytesRead = SpiLayerComm_readTpdu(respTpdu);
  if (bytesRead < 0) {
    STLOG_HAL_E("Error when reading from SPI interface (%d).", bytesRead);
    return -1;
  }
  STLOG_HAL_D("%d bytes read from SPI interface", bytesRead);

  uint8_t buffer[(5 + respTpdu->len)];
  uint16_t length = Tpdu_toByteArray(respTpdu, buffer);
  if (length > 0) {
    DispHal("Rx", buffer, length);
  }
  return bytesRead;
}

/*******************************************************************************
**
** Function         SpiLayerInterface_close
**
** Description      Close the device
**
** Parameters       pDevHandle - device handle
**
** Returns          None
**
*******************************************************************************/
void SpiLayerInterface_close(void* pDevHandle) {
  if (NULL != pDevHandle) {
    STLOG_HAL_D("SpiLayerInterface_close");
    SpiLayerDriver_close();
  }
}
