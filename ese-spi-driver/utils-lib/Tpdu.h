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

#ifndef TPDU_H_
#define TPDU_H_

//*********************************** Includes *********************************

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "Atp.h"

//************************************ Defines *********************************
#define NAD_OFFSET_IN_TPDU 0
#define PCB_OFFSET_IN_TPDU 1
#define LEN_OFFSET_IN_TPDU 2
#define DATA_OFFSET_IN_TPDU 3

#define TPDU_MAX_LENGTH 259
#define TPDU_PROLOGUE_LENGTH 3
#define TPDU_MAX_DATA_LENGTH 254
#define TPDU_CRC_LENGTH 2
#define TPDU_LRC_LENGTH 1

//************************************ Structs *********************************
typedef struct {
  char nad;
  char pcb;
  uint8_t len;
  char data[254];
  uint16_t checksum;
} Tpdu;

typedef enum { IBlock, RBlock, SBlock } TpduType;

typedef enum { ErrorFree, ChecksumError, OtherErrors } RBlockType;

//************************************ Functions *******************************

/**
 * Forms a byte array representing the given TPDU.
 *
 * @param structTpdu The TPDU struct to be converted to byte array.
 * @param baTpdu The memory position where to store the formed byte array.
 *
 * @return The length of the formed array, -1 if there is an error.
 */
int Tpdu_toByteArray(Tpdu* structTpdu, char* baTpdu);

/**
 * Checks that the checksum in the TPDU is as expected.
 *
 * @param tpdu The TPDU whose checksum needs to be checked.
 *
 * @return true if checksum is ok, false otherwise.
 */
bool Tpdu_isChecksumOk(Tpdu* tpdu);

/**
 * Forms a TPDU with the specified fields.
 *
 * @param nad The NAD byte of the TPDU.
 * @param pac The PCB byte of the TPDU.
 * @param len The length of the data.
 * @param data The data of the TPDU.
 * @pram tpdu The memory position where the formed TPDU will be stored.
 *
 * @return 0 if everything went ok, -1 otherwise.
 */
int Tpdu_formTpdu(char nad, char pcb, uint8_t len, char* data, Tpdu* tpdu);

/**
 * Returns the checksum value in the form of a byte array.
 *
 * @param tpdu The TPDU struct from where to get the checksum value.
 * @param checksumBytes The memory position where to store the result.
 */
void Tpdu_getChecksumBytes(Tpdu* tpdu, char* checksumBytes);

/**
 * Gets the value of the checksum stored in the array.
 *
 * @param array The array that contains the checksum.
 * @param checksumStartPosition The position where the checksum starts in array.
 *
 * @return The value of the checksum.
 */
uint16_t Tpdu_getChecksumValue(char* array, int checksumStartPosition,
                               ChecksumType checksumType);

/**
 * Returns the type of the TPDU.
 *
 * @param the tpdu the type has to be get.
 *
 * @return The TPDU type of the tpdu.
 */
TpduType Tpdu_getType(Tpdu* tpdu);

/**
 * Converts a TPDU into a hex string.
 *
 * @param tpdu The TPDU to be converted to a string.
 * @param hexStringBuffer The output buffer.
 */
void Tpdu_toHexString(Tpdu* tpdu, char* hexStringBuffer);

#endif /* TPDU_H_ */
