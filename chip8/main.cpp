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
    uint16_t pc; //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    uint8_t V[16];// why do I have this TODO
    uint16_t opcode;
    uint8_t sp; //maybe change to 16

    uint16_t stack[16];

    void execute();
};

void Chip8::execute()
{

    switch(opcode)
    {
////////////////////////////////CLS - TODO pattern
        case 0x00E0://to finish
            cout << "CLS"<< endl;

            break;
    }

    //////////////////////////////////RET
    if ((opcode & 0xFFFF)==0x00EE)
    {

        pc = stack[--sp];
        cout << "Ret" << endl;
        cout << pc << endl;
        cout << sp << endl;
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
        // push return address
        stack[sp] = pc;
        sp++; //TODO ensure increment is not before

        pc = addr;
        cout << "PC at 0x" << addr << endl;

    }
//////////////////////////////////////////SE Vx, byte
    if ((opcode & 0xF000)==0x3000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto kk = opcode & 0x00FF;

        if(V[x]==kk)
        {
            pc += 2;
            cout << "SE V" << x << ", " << kk << "V" << x << " = " << V[x] << endl;
        }
    }

/////////////////////////////////////////SNE Vx, byte

    if ((opcode & 0xF000)==0x4000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto kk = opcode & 0x00FF;

        if(V[x]!=kk)
        {
            pc += 2;
            cout << "SE V" << x << ", " << kk << "V" << x << " = " << V[x] << endl;
        }
    }
/////////////////////////////////////////////////SE Vx, Vy

    if ((opcode & 0xF000)==0x5000)//maybe mask final bit for 0 - nvm
    {
        auto x = (opcode & 0x0F00)>>8;
        auto y = (opcode & 0x00F0)>>4;

        if(V[x]==V[y])
        {
            pc += 2;
        }

    }
///////////////////////////////////////LD Vx, byte
}

int main()
{
    Chip8 emu;
    emu.opcode = 0x3333;
    emu.execute();

    return 0;
}
