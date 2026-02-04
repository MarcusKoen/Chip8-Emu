#include <iostream>
#include <stdint-gcc.h>
using namespace std;
/*
struct OPCODES
{

    uint16_t CLS = 0x00E0;

};
*/

class Chip8
{
public:

    uint8_t memory[4096]; // Why 8 bit when 2^8 = 256 < 4096 bits
    uint16_t pc = 0x200; //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    uint8_t V[16];
    uint16_t opcode;

    void execute();
};

void Chip8::execute()
{

    switch(opcode)
    {

        case 0x00E0://to finish
            cout << "CLS"<< endl;
            break;
    }
////////////////////////////////////////JP
    if ((opcode & 0xF000) == 0x1000)//JP
        {
            auto addr = (opcode & 0x0FFF);
            pc = addr;
            cout << "JP to 0x"<< std::hex << std::uppercase << addr << endl;
            cout << "PC at 0x" <<std::hex << std::uppercase << pc << endl;
        }
//////////////////////////////////////////////CALL

    if ((opcode & 0xF000)==0x2000)
    {
        auto addr = opcode & 0x0FFF;
        cout << "Call routine at 0x" << std::hex << std::uppercase << addr<< endl; //todo stack pointer and PC

        //stack pointer
        pc = addr;
        cout << "PC at 0x" << addr << endl;

    }

}

int main()
{
    Chip8 emu;
    emu.opcode = 0x2FFF;
    emu.execute();

    return 0;
}
