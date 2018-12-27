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
#define LOG_TAG "StEse-SpiLayerDriver"
#include "SpiLayerDriver.h"
#include <sys/time.h>
#include "android_logmsg.h"

#include "utils-lib/Utils.h"

int spiDeviceId;
int currentMode;
struct timeval lastRxTxTime;

/*******************************************************************************
**
** Function         SpiLayerDriver_open
**
** Description      Open the spi device driver.
**
** Parameters       spiDevPath - Spi device path.
**
** Returns          0 if everything is ok, -1 otherwise.
**
*******************************************************************************/
int SpiLayerDriver_open(char* spiDevPath) {
  char* spiDeviceName = spiDevPath;
  STLOG_HAL_D("%s : Enter ", __func__);
  // Open the master spi device and save the spi device identifier
  spiDeviceId = open(spiDeviceName, O_RDWR | O_NOCTTY);
  STLOG_HAL_D(" spiDeviceId: %d", spiDeviceId);
  if (spiDeviceId < 0) {
    return -1;
  }
  currentMode = MODE_RX;
  gettimeofday(&lastRxTxTime, 0);

  return 0;
}

/*******************************************************************************
**
** Function         SpiLayerDriver_close
**
** Description      Close the spi device driver.
**
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
void SpiLayerDriver_close() {
  if (spiDeviceId > 0) {
    close(spiDeviceId);
  }
}

/*******************************************************************************
**
** Function         SpiLayerDriver_read
**
** Description      Reads bytesToRead bytes from the SPI interface.
**
** Parameters       rxBuffer    - Buffer to store recieved datas.
**                  bytesToRead - Expected number of bytes to be read.
**
** Returns          The amount of bytes read from the slave, -1 if something
**                  failed.
**
*******************************************************************************/
int SpiLayerDriver_read(char* rxBuffer, unsigned int bytesToRead) {
  if (currentMode != MODE_RX) {
    currentMode = MODE_RX;
    STLOG_HAL_V(" Last TX: %ld,%ld", lastRxTxTime.tv_sec, lastRxTxTime.tv_usec);
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);
    STLOG_HAL_V("     Now: %ld,%ld", currentTime.tv_sec, currentTime.tv_usec);
    int elapsedTime = Utils_getElapsedTimeInMs(lastRxTxTime, currentTime);
    if (elapsedTime < MIN_TIME_BETWEEN_MODE_SWITCH) {
      int waitTime = MIN_TIME_BETWEEN_MODE_SWITCH - elapsedTime;
      STLOG_HAL_V("Waiting %d ms to switch from TX to RX", waitTime);
      usleep(waitTime * 1000);
    }
    gettimeofday(&currentTime, 0);
    STLOG_HAL_V("Start RX: %ld,%ld", currentTime.tv_sec, currentTime.tv_usec);
  }
  int rc = read(spiDeviceId, rxBuffer, bytesToRead);
  gettimeofday(&lastRxTxTime, 0);
  if (bytesToRead == 1 && rxBuffer[0] != 0 && rxBuffer[0] != 0x12) {
    STLOG_HAL_D("Unexpected byte read from SPI: 0x%02X 0x%02X 0x%02X",
                rxBuffer[0], rxBuffer[1], rxBuffer[2]);
  }
  return rc;
}

/*******************************************************************************
**
** Function         SpiLayerDriver_write
**
** Description      Write txBufferLength bytes to the SPI interface.
**
** Parameters       txBuffer       - Buffer to transmit.
**                  txBufferLength - Number of bytes to be written.
**
** Returns          The amount of bytes written to the slave, -1 if something
**                  failed.
**
*******************************************************************************/
int SpiLayerDriver_write(char* txBuffer, unsigned int txBufferLength) {
  if (currentMode != MODE_TX) {
    currentMode = MODE_TX;
    STLOG_HAL_V(" Last RX: %ld,%ld", lastRxTxTime.tv_sec, lastRxTxTime.tv_usec);
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);
    STLOG_HAL_V("     Now: %ld,%ld", currentTime.tv_sec, currentTime.tv_usec);
    int elapsedTime = Utils_getElapsedTimeInMs(lastRxTxTime, currentTime);
    if (elapsedTime < MIN_TIME_BETWEEN_MODE_SWITCH) {
      int waitTime = MIN_TIME_BETWEEN_MODE_SWITCH - elapsedTime;
      STLOG_HAL_V("Waiting %d ms to switch from RX to TX", waitTime);
      usleep(waitTime * 1000);
    }
    gettimeofday(&currentTime, 0);
    STLOG_HAL_V("Start TX: %ld,%ld", currentTime.tv_sec, currentTime.tv_usec);
  }
  char hexString[txBufferLength * 3];
  Utils_charArrayToHexString(txBuffer, txBufferLength, hexString);
  STLOG_HAL_D("SpiLayerDriver_write: spiTx > %s", hexString);
  int rc = write(spiDeviceId, txBuffer, txBufferLength);
  gettimeofday(&lastRxTxTime, 0);
  return rc;
}
