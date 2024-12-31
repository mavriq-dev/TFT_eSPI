#ifndef _TFT_COMMANDSET_ST7735_H_
#define _TFT_COMMANDSET_ST7735_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7735 : public CommandSequenceFactory {
public:
    // Basic display commands
    CommandSequence createNOP() override { return CommandSequence("NOP").addCommand(0x00); }
    CommandSequence createSWRST() override { return CommandSequence("SWRST").addCommand(0x01); }
    CommandSequence createSLPIN() override { return CommandSequence("SLPIN").addCommand(0x10); }
    CommandSequence createSLPOUT() override { return CommandSequence("SLPOUT").addCommand(0x11); }
    CommandSequence createPTLON() override { return CommandSequence("PTLON").addCommand(0x12); }
    CommandSequence createNORON() override { return CommandSequence("NORON").addCommand(0x13); }
    CommandSequence createINVOFF() override { return CommandSequence("INVOFF").addCommand(0x20); }
    CommandSequence createINVON() override { return CommandSequence("INVON").addCommand(0x21); }
    CommandSequence createGAMSET() override { return CommandSequence("GAMSET").addCommand(0x26); }
    CommandSequence createDISPOFF() override { return CommandSequence("DISPOFF").addCommand(0x28); }
    CommandSequence createDISPON() override { return CommandSequence("DISPON").addCommand(0x29); }
    CommandSequence createCASET() override { return CommandSequence("CASET").addCommand(0x2A); }
    CommandSequence createRASET() override { return CommandSequence("RASET").addCommand(0x2B); }
    CommandSequence createRAMWR() override { return CommandSequence("RAMWR").addCommand(0x2C); }
    CommandSequence createRAMRD() override { return CommandSequence("RAMRD").addCommand(0x2E); }
    CommandSequence createPTLAR() override { return CommandSequence("PTLAR").addCommand(0x30); }
    CommandSequence createTEOFF() override { return CommandSequence("TEOFF").addCommand(0x34); }
    CommandSequence createTEON() override { return CommandSequence("TEON").addCommand(0x35); }
    CommandSequence createMADCTL() override { return CommandSequence("MADCTL").addCommand(0x36); }
    CommandSequence createIDMOFF() override { return CommandSequence("IDMOFF").addCommand(0x38); }
    CommandSequence createIDMON() override { return CommandSequence("IDMON").addCommand(0x39); }
    CommandSequence createCOLMOD() override { return CommandSequence("COLMOD").addCommand(0x3A); }

    // Power control commands
    CommandSequence createVCOMOFF() override { return CommandSequence("VCOMOFF").addCommand(0xBC); }
    CommandSequence createVCOMON() override { return CommandSequence("VCOMON").addCommand(0xBD); }
    CommandSequence createWRCTRL() override { return CommandSequence("WRCTRL").addCommand(0xC0); }
    CommandSequence createPWCTRL1() override { return CommandSequence("PWCTRL1").addCommand(0xC1); }
    CommandSequence createPWCTRL2() override { return CommandSequence("PWCTRL2").addCommand(0xC2); }
    CommandSequence createPWCTRL3() override { return CommandSequence("PWCTRL3").addCommand(0xC3); }
    CommandSequence createPWCTRL4() override { return CommandSequence("PWCTRL4").addCommand(0xC4); }
    CommandSequence createPWCTRL5() override { return CommandSequence("PWCTRL5").addCommand(0xC5); }
    CommandSequence createVCMCTRL1() override { return CommandSequence("VCMCTRL1").addCommand(0xC6); }
    CommandSequence createVCMCTRL2() override { return CommandSequence("VCMCTRL2").addCommand(0xC7); }

    // Gamma control commands
    CommandSequence createGMCTRP1() override { return CommandSequence("GMCTRP1").addCommand(0xE0); }
    CommandSequence createGMCTRN1() override { return CommandSequence("GMCTRN1").addCommand(0xE1); }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7735_H_
