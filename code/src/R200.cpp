#include "R200.hpp"

void R200::init() {
    // send_cmd(R200_Command::SetWorkArea, {0x03}); // Set work area to EU3 (865-868MHz)
    // send_cmd(R200_Command::SetTransmitPower, {0x0A, 0x28}); // Set transmit power to 26 dBm (0x0A28 = 0d2600 dBm)
    // send_cmd(R200_Command::SetQueryParameters, {0x11, 0x20});

    ESP_LOGI(TAG_R200, "R200 initialized on UART%d", R200_UART);
}

esp_err_t R200::send_cmd(const ha_event_t& event) {
    uint8_t cmd = event.cmd;
    uint16_t param_len = event.param_len;

    // Header, Type, Command, ParamLen(2), Params, Checksum, End
    std::vector<uint8_t> frame(5 + param_len + 2);

    frame.at(R200_FrameStructure::HeaderPos) = R200_FrameControl::Header;
    frame.at(R200_FrameStructure::TypePos) = R200_FrameType::Command;
    frame.at(R200_FrameStructure::CommandPos) = cmd;
    frame.at(R200_FrameStructure::ParamLengthMSBPos) = (param_len >> 8) & 0xFF; // ParamLen MSB
    frame.at(R200_FrameStructure::ParamLengthLSBPos) = param_len & 0xFF;        // ParamLen LSB
    for (size_t i = 0; i < param_len; ++i)
        frame.at(R200_FrameStructure::ParamPos + i) = event.params[i];
    frame.at(R200_FrameStructure::ParamPos + param_len) = check_frame(frame, param_len);
    frame.at(R200_FrameStructure::ParamPos + param_len + 1) = R200_FrameControl::End;

    if (uart_write_bytes(R200_UART, frame.data(), sizeof(frame)) < 0) {
        ESP_LOGE(TAG_R200, "Failed to send command 0x%02X", cmd);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG_R200, "Command 0x%02X sent with %d params", cmd, param_len);
    return ESP_OK;
}

esp_err_t R200::receive_data() {
    uint8_t header, type, cmd, sum, end;
    uint16_t param_len;
    std::vector<uint8_t> frame{};

    // reading the first 5 bytes of the frame: Header, Type, Command, ParamLen(2)
    if (uart_read_bytes(R200_UART, frame.data(), 5, pdMS_TO_TICKS(20)) != 5) {
        uart_flush_input(R200_UART);
        ESP_LOGE(TAG_R200, "Unable to read frame's header bytes");
        return ESP_FAIL;
    }

    header = frame.at(R200_FrameStructure::HeaderPos);
    type = frame.at(R200_FrameStructure::TypePos);
    cmd = frame.at(R200_FrameStructure::CommandPos);
    param_len = frame.at(R200_FrameStructure::ParamLengthMSBPos) << 8;
    param_len |= frame.at(R200_FrameStructure::ParamLengthLSBPos);

    if (header != R200_FrameControl::Header) {
        uart_flush_input(R200_UART);
        return ESP_FAIL;
    }

    frame.resize(5 + param_len + 2);
    uart_read_bytes(R200_UART, frame.data() + R200::R200_FrameStructure::ParamPos, param_len + 2, pdMS_TO_TICKS(40));

    sum = frame.at(R200_FrameStructure::ParamPos + param_len);
    end = frame.at(R200_FrameStructure::ParamPos + param_len + 1);

    if (sum != check_frame(frame, param_len)) {
        uart_flush_input(R200_UART);
        ESP_LOGE(TAG_R200, "Invalid frame checksum");
        return ESP_FAIL;
    }

    if (end != R200_FrameControl::End) {
        uart_flush_input(R200_UART);
        return ESP_FAIL;
    }

    //process_frame(type, cmd, fra.data(), param_len);

    return ESP_OK;
}

uint8_t R200::check_frame(const std::vector<uint8_t>& frame, uint16_t param_len) {
    uint8_t checkSum = 0; // we only take the LSB of the sum
    
    // check sum does not include the header and end byte
    for (size_t i = 1; i < R200_FrameStructure::ParamPos + param_len; ++i)
        checkSum += frame.at(i);

    return checkSum;
}

// void printHexByte(char* name, uint8_t value){
//   Serial.print(name);
//   Serial.print(":");
//   Serial.print(value < 0x10 ? "0x0" : "0x");
//   Serial.println(value, HEX);
// }

// void printHexBytes(char* name, uint8_t *value, uint8_t len){
//   Serial.print(name);
//   Serial.print(":");
//   Serial.print("0x");
//   for(int i=0; i<len; i++){
//     Serial.print(value[i] < 0x10 ? "0" : "");
//     Serial.print(value[i], HEX);
//   }
//   Serial.println("");
// }

// void printHexWord(char* name, uint8_t MSB, uint8_t LSB){
//   Serial.print(name);
//   Serial.print(":");
//   Serial.print(MSB < 0x10 ? "0x0" : "0x");
//   Serial.println(MSB, HEX);
//   Serial.print(LSB < 0x10 ? "0" : "");
//   Serial.println(LSB, HEX);
// }

// void R200::loop(){
//   // Has any new data been received?      
//   if(dataAvailable()){
//     // Attempt to receive a full frame of data
//     if(receiveData()){
//       if(dataIsValid()){
//         // If a full frame of data has been received, parse it
//         // TODO For reasons that I absolutely cannot fathom, this section does not work if moved into
//         // a separate function....
//         // parseReceivedData();
//         switch(_buffer[R200_CommandPos]){
//           case CMD_GetModuleInfo:
//             for (uint8_t i=0; i<RX_BUFFER_LENGTH-8; i++) {
//               Serial.print((char)_buffer[6 + i]);
//               // Stop when then only two bytes left are the CRC and FrameEnd marker
//               if (_buffer[8 + i] == R200_FrameEnd) {
//                 break;
//               }
//             }
//             Serial.println("");
//             break;
//           case CMD_SinglePollInstruction:
//             // Example successful response
//             // AA 02 22 00 11 C7 30 00 E2 80 68 90 00 00 50 0E 88 C6 A4 A7 11 9B 29 DD 
//             // AA:Frame Header
//             // 02:Instruction Code
//             // 22:Command Parameter
//             // 00 11:Instruction data length (0x11 = 17 bytes)
//             // C7：RSSI Signal Strength
//             // 30 00: Label PC code (factory reg code)
//             // E2 80 68 90 00 00 50 0E 88 C6 A4 A7：EPC code
//             // 11 9B:CRC check
//             // 29: Verification
//             // DD: End of frame
//             #ifdef DEBUG
//               printHexByte("RSSI", _buffer[6]);
//               printHexWord("PC", _buffer[7], _buffer[8]);
//               printHexBytes("EPC(", &_buffer[9], 12);
//             #endif
//             if(memcmp(uid, &_buffer[9], 12) != 0) {
//               memcpy(uid, &_buffer[9], 12);
//               #ifdef DEBUG
//                 Serial.print("New card detected : ");
//                 dumpUIDToSerial();
//                 Serial.println("");
//               #endif
//             }
//             else {
//               #ifdef DEBUG
//                 Serial.print("Same card still present : ");
//                 dumpUIDToSerial();
//                 Serial.println("");
//               #endif
//             }
//             #ifdef DEBUG
//               printHexWord("CRC", _buffer[20], _buffer[21]);
//             #endif
//             break;
//           case CMD_ExecutionFailure:
//             switch(_buffer[R200_ParamPos]){
//               case ERR_CommandError:
//                 Serial.println("Command error");
//                 break;
//               case ERR_InventoryFail:
//                 // This is not necessarily a "failure" - it just means that there are no cards in range
//                 // Serial.print("No card detected!");
//                 // If there was previously a uid
//                 if(memcmp(uid, blankUid, sizeof uid) != 0) {
//                   #ifdef DEBUG
//                     Serial.print("Card removed : ");
//                     dumpUIDToSerial();
//                     Serial.println("");
//                   #endif
//                   memset(uid, 0, sizeof uid);
//                 }
//                 break;
//               case ERR_AccessFail:
//                 // Serial.println("Access Fail");
//                 break;
//               case ERR_ReadFail:
//                 // Serial.println("Read fail");
//                 break;
//               case ERR_WriteFail:
//                 // Serial.println("Write fail");
//                 break;
//               default:
//                 // Serial.print("Fail code ");
//                 // Serial.println(_buffer[R200_ParamPos], HEX);
//                 break;
//             }
//             break;
//         }
//       }
//     }
//   }
// }



// // Parse data that has been placed in the receive buffer
// bool R200::parseReceivedData() {
//   switch(_buffer[R200_CommandPos]){
//     case CMD_GetModuleInfo:
//       break;
//     case CMD_SinglePollInstruction:
//       for(uint8_t i=8; i<20; i++) {
//         uid[i-8] = _buffer[i];
//       };
//       //memcpy(uid, _buffer+9, 12);
//       break;
//     case CMD_MultiplePollInstruction:
//       for(uint8_t i=8; i<20; i++) {
//         uid[i-8] = _buffer[i];
//       };
//       //memcpy(uid, _buffer+9, 12);
//       break;
//     case CMD_ExecutionFailure:
//       break;
//     default:
//       break;
//   }
// }

// /*
//  * Note that Arduino Serial.flush() method does not clear the incoming serial buffer - only the outgoing!
//  */
// uint8_t R200::flush(){
//   uint8_t bytesDiscarded = 0;
//   while(_serial->available()){
//     _serial->read();
//     bytesDiscarded++;
//   }
//   return bytesDiscarded;
// }

// // Read incoming serial data sent by the reader
// // This could either be a response to a command sent, or a notification
// // (e.g. when set to automatic polling mode)
// // Returns true if a complete frame of data is read within the allotted timeout
// bool R200::receiveData(unsigned long timeOut){
//   //Serial.println("Receiving Data");
//   unsigned long startTime = millis();
//   uint8_t bytesReceived = 0;
//   // Clear the buffer
//   //memset(_buffer, 0, sizeof _buffer);
//   for (int i = 0; i < RX_BUFFER_LENGTH; i++) { _buffer[i] = 0; }
//   while ((millis() - startTime) < timeOut) {
//     while (_serial->available()) {
//       uint8_t b = _serial->read();
//       if(bytesReceived > RX_BUFFER_LENGTH - 1) {
//         Serial.print("Error: Max Buffer Length Exceeded!");
//         flush();
//         return false;
//       }
//       else {
//       _buffer[bytesReceived] = b;
//       }
//       bytesReceived++;
//       if (b == R200_FrameEnd) { break; }
//     }
//   }
//   if (bytesReceived > 1 && _buffer[0] == R200_FrameHeader && _buffer[bytesReceived - 1] == R200_FrameEnd) {
//       return true;
//   } else {
//       return false;
//   }
//   return false;
// }

// void R200::poll(){
//   uint8_t commandFrame[7] = {0};
//   commandFrame[0] = R200_FrameHeader;
//   commandFrame[1] = FrameType_Command;
//   commandFrame[2] = CMD_SinglePollInstruction;
//   commandFrame[3] = 0x00; // ParamLen MSB
//   commandFrame[4] = 0x00; // ParamLen LSB
//   commandFrame[5] = 0x22;  // Checksum
//   commandFrame[6] = R200_FrameEnd;
//   _serial->write(commandFrame, 7);
// }

// void R200::setMultiplePollingMode(bool enable){
//   if(enable){
//     uint8_t commandFrame[10] = {0};
//     commandFrame[0] = R200_FrameHeader;
//     commandFrame[1] = FrameType_Command; //(0x00)
//     commandFrame[2] = CMD_MultiplePollInstruction; //0x27
//     commandFrame[3] = 0x00; // ParamLen MSB
//     commandFrame[4] = 0x03; // ParamLen LSB
//     commandFrame[5] = 0x22;  // Param (Reserved? Always 0x22 for this command)
//     commandFrame[6] = 0xFF;  // Param (Count of polls, MSB)
//     commandFrame[7] = 0xFF;  // Param (Count of polls, LSB)
//     commandFrame[8] = 0x4A; // LSB of commandFrame[2] + commandFrame[3] + commandFrame[4] + commandFrame[5] + commandFrame[6] + commandFrame[7] (full value is 0x024A)
//     commandFrame[9] = R200_FrameEnd;
//     _serial->write(commandFrame, 10);
//   }
//   else {
//     uint8_t commandFrame[7] = {0};
//     commandFrame[0] = R200_FrameHeader;
//     commandFrame[1] = FrameType_Command; //(0x00)
//     commandFrame[2] = CMD_StopMultiplePoll; //0x28
//     commandFrame[3] = 0x00; // ParamLen MSB
//     commandFrame[4] = 0x00; // ParamLen LSB
//     commandFrame[5] = 0x28; // LSB of commandFrame[2] + commandFrame[3] + commandFrame[4]
//     commandFrame[6] = R200_FrameEnd;
//     _serial->write(commandFrame, 7);
//   }
// }

// uint16_t R200::arrayToUint16(uint8_t *array){
//   uint16_t value = *array;
//   value <<=8;
//   value += *(array+1);
//   return value;
// }