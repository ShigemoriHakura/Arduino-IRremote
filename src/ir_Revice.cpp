#include <Arduino.h>

//#define DEBUG // Activate this for lots of lovely debug output from this decoder.
#include "IRremoteInt.h" // evaluates the DEBUG for DEBUG_PRINT

#define REVICE_UNIT              560

#define REVICE_HEADER_MARK       (12 * REVICE_UNIT) // The length of the Header:Mark
#define REVICE_HEADER_SPACE      (1 * REVICE_UNIT)  // The length of the Header:Space

#define REVICE_ONE_SPACE         (3 * REVICE_UNIT)  // The length of a Bit:Space for 1's
#define REVICE_ZERO_SPACE        REVICE_UNIT        // The length of a Bit:Space for 0's


//+=============================================================================
//
void IRsend::sendRevice(int Dec) {
    enableIROut(38);

    int code[30] = {};
    boolean Bin[] = {0, 0, 0, 0, 0, 0, 0, 0};
    code[0] = 6000;
    code[1] = 500;

    for (int i = 7 ; i >= 0 ; i--) {
      if (pow(2, i) <= Dec) {
        Dec = Dec - pow(2, i);
        Bin[8 - (i + 1)] = 1;
      } else {
      }
    }

    int ii = 2;
    for (int j = 0; j < 8; j++) {
      if (Bin[j] == 0) {
        code[ii] = 1500;
        ii ++;
        code[ii] = 500;
        ii ++;
      } else {
        code[ii] = 500;
        ii ++;
        code[ii] = 1500;
        ii ++;
      }
    }
    code[ii] = 1500;

    uint16_t acode[40] = {};
    for (int iii = 0; iii < 19; iii++) {
      acode[iii] = (uint16_t)code[iii];
    }

    sendRaw(acode, 20, 38);
}

bool IRrecv::decodeRevice() {
    //Revice的数组是18位
    if (decodedIRData.rawDataPtr->rawlen != 18) {
        return false;
    }

    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], REVICE_HEADER_MARK) || !matchSpace(decodedIRData.rawDataPtr->rawbuf[2], REVICE_HEADER_SPACE)) {
        DEBUG_PRINT("REVICE: ");
        DEBUG_PRINTLN("Header mark or space length is wrong");
        return false;
    }

    uint8_t tBitIndex;
    uint32_t tDecodedRawData = 0;
    for (tBitIndex = 3; tBitIndex < decodedIRData.rawDataPtr->rawlen - 1; tBitIndex++) {
        uint8_t tStartLevel = decodedIRData.rawDataPtr->rawbuf[tBitIndex];
        tBitIndex++;
        uint8_t tEndLevel = decodedIRData.rawDataPtr->rawbuf[tBitIndex];

        if (matchMark(tStartLevel, REVICE_ONE_SPACE) && matchMark(tEndLevel, REVICE_ZERO_SPACE)) {
            tDecodedRawData = (tDecodedRawData << 1) | 0;
        } else if (matchMark(tStartLevel, REVICE_ZERO_SPACE) && matchMark(tEndLevel, REVICE_ONE_SPACE)) {
            tDecodedRawData = (tDecodedRawData << 1) | 1;
        } else {
            DEBUG_PRINT(F("REVICE: "));
            DEBUG_PRINT(tBitIndex);
            DEBUG_PRINT(":");
            DEBUG_PRINT(tStartLevel);
            DEBUG_PRINT("-");
            DEBUG_PRINT(tEndLevel);
            DEBUG_PRINTLN(F(" Decode failed"));
            return false;
        }
    }

    if(matchMark(decodedIRData.rawDataPtr->rawbuf[17], REVICE_ONE_SPACE)){
      tDecodedRawData = (tDecodedRawData << 1) | 0;
    }else{
      tDecodedRawData = (tDecodedRawData << 1) | 1;
    }

    decodedIRData.address = tDecodedRawData;
    decodedIRData.numberOfBits = 18;
    decodedIRData.protocol = REVICE;

    return true;
}

bool IRrecv::decodeReviceMSB(decode_results *aResults){
    if (aResults->rawlen != 18) {
        return false;
    }

    if (!matchMark(aResults->rawbuf[1], REVICE_HEADER_MARK) || !matchSpace(aResults->rawbuf[2], REVICE_HEADER_SPACE)) {
        DEBUG_PRINT("REVICE: ");
        DEBUG_PRINTLN("Header mark or space length is wrong");
        return false;
    }

    uint8_t tBitIndex;
    uint32_t tDecodedRawData = 0;
    for (tBitIndex = 3; tBitIndex < aResults->rawlen - 1; tBitIndex++) {
        uint8_t tStartLevel = aResults->rawbuf[tBitIndex];
        tBitIndex++;
        uint8_t tEndLevel = aResults->rawbuf[tBitIndex];

        if (matchMark(tStartLevel, REVICE_ONE_SPACE) && matchMark(tEndLevel, REVICE_ZERO_SPACE)) {
            tDecodedRawData = (tDecodedRawData << 1) | 0;
        } else if (matchMark(tStartLevel, REVICE_ZERO_SPACE) && matchMark(tEndLevel, REVICE_ONE_SPACE)) {
            tDecodedRawData = (tDecodedRawData << 1) | 1;
        } else {
            DEBUG_PRINT(tBitIndex);
            DEBUG_PRINT(":");
            DEBUG_PRINT(tStartLevel);
            DEBUG_PRINT("-");
            DEBUG_PRINT(tEndLevel);
            DEBUG_PRINT(F("REVICE: "));
            DEBUG_PRINTLN(F("Decode failed"));
            return false;
        }
    }

    if(matchMark(aResults->rawbuf[17], REVICE_ONE_SPACE)){
      tDecodedRawData = (tDecodedRawData << 1) | 0;
    }else{
      tDecodedRawData = (tDecodedRawData << 1) | 1;
    }

    decodedIRData.address = tDecodedRawData;
    decodedIRData.numberOfBits = 18;
    decodedIRData.protocol = REVICE;

    return true;
}