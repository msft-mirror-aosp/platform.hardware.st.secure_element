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

#include "Tpdu.h"

#include "Iso13239CRC.h"

/*******************************************************************************
**
** Function        Tpdu_toByteArray
**
** Description     Forms a byte array representing the given TPDU.
**
** Parameters      structTpdu - TPDU struct to be converted to byte array.
**                 baTpdu     - Memory position where to store the formed
**                              byte array.
**
** Returns        length of the formed array, -1 if there is an error.
**
*******************************************************************************/
int Tpdu_toByteArray(Tpdu *structTpdu, char *baTpdu) {
  // NAD - Copy the nad into the nad array position
  baTpdu[NAD_OFFSET_IN_TPDU] = structTpdu->nad;

  // PCB - Copy the pcb into the pcb array position
  baTpdu[PCB_OFFSET_IN_TPDU] = structTpdu->pcb;

  // Length - Copy the length into the length array position
  baTpdu[LEN_OFFSET_IN_TPDU] = structTpdu->len;

  // Data - Copy the data into the data array position
  uint8_t i;
  for (i = 0; i < structTpdu->len; i++) {
    baTpdu[DATA_OFFSET_IN_TPDU + i] = structTpdu->data[i];
  }

  // Compute where the checksum shall be stored.
  uint16_t checksumOffsetInTpdu = DATA_OFFSET_IN_TPDU + structTpdu->len;

  int length;
  char checksum[2];
  switch (ATP.checksumType) {
    case LRC:
      Tpdu_getChecksumBytes(structTpdu, checksum);
      baTpdu[checksumOffsetInTpdu] = checksum[0];
      length = checksumOffsetInTpdu + 1;
      break;
    case CRC:
      Tpdu_getChecksumBytes(structTpdu, checksum);
      baTpdu[checksumOffsetInTpdu] = checksum[0];
      baTpdu[checksumOffsetInTpdu + 1] = checksum[1];
      length = checksumOffsetInTpdu + 2;
      break;
  }

  return length;
}

/*******************************************************************************
**
** Function        Tpdu_isChecksumOk
**
** Description     Checks that the checksum in the TPDU is as expected.
**
** Parameters      tpdu - TPDU whose checksum needs to be checked.
**
** Returns        true if checksum is ok, false otherwise.
**
*******************************************************************************/
bool Tpdu_isChecksumOk(Tpdu *tpdu) {
  switch (ATP.checksumType) {
    case LRC:
      // TODO: implement
      return -1;
    case CRC:;
      char buffer[TPDU_MAX_LENGTH];
      Tpdu_toByteArray(tpdu, buffer);
      if (tpdu->checksum ==
          computeCrc(buffer, (TPDU_PROLOGUE_LENGTH + tpdu->len))) {
        return 0;
      } else {
        return -1;
      }
  }
}

/*******************************************************************************
**
** Function        Tpdu_formTpdu
**
** Description     Forms a TPDU with the specified fields.
**
** Parameters      nad   - NAD byte of the TPDU.
**                 pcb   - PCB byte of the TPDU.
**                 len   - Length of the data
**                 data  - data of the TPDU
**                 tpdu  - output TPDU struct
**
** Returns         0 if everything went ok, -1 otherwise.
**
*******************************************************************************/
int Tpdu_formTpdu(char nad, char pcb, uint8_t len, char *data, Tpdu *tpdu) {
  uint8_t i;

  // NAD - Copy the incoming nad into the tpdu nad
  tpdu->nad = nad;
  // PCB - Copy the incoming pcb into the tpdu pcb
  tpdu->pcb = pcb;
  // Length - Copy the incoming len into the tpdu len
  tpdu->len = len;

  // Data - Copy the incoming data into the tpdu data
  // TODO: check if it will ever be overwritten
  for (i = 0; i < len; i++) {
    tpdu->data[i] = data[i];
  }
  // tpdu->data = data;
  // Checksum - Calculate the checksum according to the prologue + data fields
  // and copy into the tpdu checksum
  switch (ATP.checksumType) {
    case LRC:
      // TODO: implement
      return -1;
    case CRC:
      // Set temporally checksum to 0 to be able to convert the tpdu struct
      // to an array
      tpdu->checksum = 0;

      // Create a buffer to store the tpdu to compute the CRC.
      char buffer[TPDU_MAX_LENGTH];
      Tpdu_toByteArray(tpdu, buffer);

      // Calculate the crc
      tpdu->checksum = computeCrc(buffer, (TPDU_PROLOGUE_LENGTH + tpdu->len));
      break;
  }

  return 0;
}

/*******************************************************************************
**
** Function        Tpdu_getChecksumBytes
**
** Description     Get the checksum value in the form of a byte array.
**
** Parameters      tpdu          - TPDU from where to get the checksum value.
**                 checksumBytes - mem postion whre to store the result.
**
** Returns         void
**
*******************************************************************************/
void Tpdu_getChecksumBytes(Tpdu *tpdu, char *checksumBytes) {
  switch (ATP.checksumType) {
    case LRC:
      checksumBytes[0] = (char)tpdu->checksum;
      break;
    case CRC:
      checksumBytes[0] = (char)tpdu->checksum;
      checksumBytes[1] = (char)(tpdu->checksum >> 8);
      break;
  }
}

/*******************************************************************************
**
** Function        Tpdu_getChecksumValue
**
** Description     Gets the value of the checksum stored in the array.
**
** Parameters      array          - array that contains the checksum.
**                 checksumStartPosition
**                 checksumType  -Checksum type (LRC or CRC)
**
** Returns         checksum value
**
*******************************************************************************/
uint16_t Tpdu_getChecksumValue(char *array, int checksumStartPosition,
                               ChecksumType checksumType) {
  switch (checksumType) {
    case LRC:
      return (uint16_t)array[checksumStartPosition];
    case CRC:
      return (uint16_t)((unsigned char)array[checksumStartPosition + 1] << 8) |
             (unsigned char)array[checksumStartPosition];
  }
}

/*******************************************************************************
**
** Function        Tpdu_getType
**
** Description     Returns the type of the TPDU.
**
** Parameters      tpdu   - the  tpdu the type has to be get.
**
** Returns         TPDU type (I-Block, R-Block or S-Block)
**
*******************************************************************************/
TpduType Tpdu_getType(Tpdu *tpdu) {
  if ((tpdu->pcb & 0x80) == 0x00) {
    return IBlock;
  } else if ((tpdu->pcb & 0xC0) == 0x80) {
    return RBlock;
  } else {
    return SBlock;
  }
}

/*******************************************************************************
**
** Function        Tpdu_toHexString
**
** Description     Converts the TPDU in hex string buffer.
**
** Parameters      tpdu            - input tpdu
**                 hexStringBuffer - output hex buffer
**
**
** Returns         void
**
*******************************************************************************/
void Tpdu_toHexString(Tpdu *tpdu, char *hexStringBuffer) {
  char buffer[tpdu->len + 5];
  int length = Tpdu_toByteArray(tpdu, buffer);
  char *ptr = hexStringBuffer;
  int i;
  for (i = 0; i < length; i++) {
    ptr += sprintf(ptr, "%02X ", buffer[i]);
  }
}
