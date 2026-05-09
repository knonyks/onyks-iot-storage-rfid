#pragma once
#include "config.hpp"

#include "uart.hpp"

class R200 {
    private:
        UART uart;

        
        // const uint8_t blankUid[12];
        // uint8_t _buffer[RX_BUFFER_LENGTH];

        // uint8_t calculateCheckSum(uint8_t *buffer);
        // uint16_t arrayToUint16(uint8_t *array);
        // bool parseReceivedData();
        // bool dataIsValid();
        // bool receiveData(unsigned long timeOut = 500);
        // void dumpReceiveBufferToSerial();
        // uint8_t flush();
    public:
        std::vector<uint8_t> tags;
        
        R200() : uart{R200_BAUD}, tags{} {};

        void init();
        void send_command(const uint8_t command, const std::vector<uint8_t>& params);
        

        // void loop();
        // void poll();
        // void setMultiplePollingMode(bool enable=true);
        // void dumpModuleInfo();
        // bool dataAvailable();

        // //bool newCardPresent();
        // //bool isCardPresent();
        // void dumpUIDToSerial();



    // Commands sent to the reader, and responses received back, are sent as data frames, e.g.
    // Header | Type | Command | ParamLength (2bytes) | Parameter(s) | Checksum | End
    //   AA   |  00  |   07    |      00 03           |   04 02 05   |    15    | DD
    //
    // Frames always start with the header value 0xAA
    // Type indicates a command to the reader (0x00), or a response (0x01), or notification (0x02) back from it
    // Command is the instruction to be performed, or the response from that instruction
    // ParamLength gives 2-byte (MSB then LSB) number of parameters being passed in the frame
    // Params may be zero or more
    // Checksum is the LSB of the sum of bytes from the type to the last instruction parameter (i.e. excluding Frame Header)
    // Frames always end with the tail value 0xDD

    // Position of elements in the frame definition, as offset from the header
    enum R200_FrameStructure : uint8_t {
        HeaderPos = 0x00,
        TypePos = 0x01,
        CommandPos = 0x02,
        ParamLengthMSBPos = 0x03,
        ParamLengthLSBPos = 0x04,
        ParamPos = 0x05
    };

    enum R200_FrameControl : uint8_t {
        Header = 0xAA,
        End = 0xDD,
    };

    enum R200_FrameType : uint8_t {
        Command = 0x00,
        Response = 0x01,
        Notification = 0x02,
    };

    enum R200_Command : uint8_t {
        GetModuleInfo = 0x03,
        SinglePollInstruction = 0x22,
        MultiplePollInstruction = 0x27,
        StopMultiplePoll = 0x28,
        SetSelectParameter = 0x0C,
        GetSelectParameter = 0x0B,
        SetSendSelectInstruction = 0x12,
        ReadLabel = 0x39,
        WriteLabel = 0x49,
        LockLabel = 0x82,
        KillTag = 0x65,
        GetQueryParameters = 0x0D,
        SetQueryParameters= 0x0E,
        SetWorkArea = 0x07,
        SetWorkingChannel = 0xAB,
        GetWorkingChannel = 0xAA,
        SetAutoFrequencyHopping = 0xAD,
        AcquireTransmitPower = 0xB7,
        SetTransmitPower = 0xB6,
        SetTransmitContinuousCarrier = 0xB0,
        GetReceiverDemodulatorParameters = 0xF1,
        SetReceiverDemodulatorParameters = 0xF0,
        TestRFInputBlockingSignal = 0xF2,
        TestChannelRSSI = 0xF3,
        ControlIOPort = 0x1A,
        ModuleSleep = 0x17,
        SetModuleIdleSleepTime = 0x1D,
        ExecutionFailure = 0xFF,
    };

    enum R200_ErrorCode : uint8_t {
        CommandError = 0x17,
        FHSSFail = 0x20,
        InventoryFail = 0x15,
        AccessFail = 0x16,
        ReadFail = 0x09,
        WriteFail = 0x10,
        LockFail = 0x13,
        KillFail = 0x12,
    };
};