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

    pc += 2;

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

    if ((opcode & 0xF000)==0x6000)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto kk = opcode & 0x00FF;

        V[x]=kk;


        cout << "V" << x << " = " << hex << kk << endl;
    }

////////////////////////////////////////7//ADD Vx, byte


    if ((opcode & 0xF000)== 0x7000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto kk = opcode & 0x00FF;

        V[x] = V[x] + kk;

        cout << " ADD Vx, byte";

    }

//////////////////////////////////// 8xy0 - LD Vx, Vy Vx=Vy

    if (((opcode & 0xF000)==0x8000) && ((opcode & 0x000F)==0) )
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[x] = V[y];

        cout << "8xy0 - LD Vx" << endl;
    }


///////////////////////////////// 8xy1 - OR Vx, Vy

    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==1)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[x] = V[x] |  V[y];

        cout << "8xy1 - OR Vx, Vy " << endl;
    }
//////////////////////////////// 8xy2 - AND Vx, Vy

    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==2)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[x] = V[x] &  V[y];

        cout << "8xy2 - AND Vx, Vy" << endl;
    }
////////////////////////////// 8xy3 - XOR Vx, Vy

    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==3)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[x] = V[x] ^ V[y];

        cout << "8xy3 - XOR Vx, Vy" << endl;
    }
//////////////////////////////// 8xy4 - ADD Vx, Vy
    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==4)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[x] = V[x] + V[y];//TODO VF flag

        cout << "8xy4 - ADD Vx, Vy" << endl;
    }
    //TODO all 8xy567E

///////////////////////////////////////// 9xy0 - SNE Vx, Vy
    if (((opcode & 0xF000)==0x9000)&&(opcode & 0x000F)==0)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        if (V[x] != V[y])
        {
            pc +=2;//skip instruction
        }
        cout << "9xy0 - SNE Vx, Vy" << endl;
    }

    //TODO Annn
///////////////////////////////////////// Bnnn - JP V0, addr

    if ((opcode & 0xF000)==0xB000)
    {

        auto nnn = opcode & 0x0FFF;

        pc = nnn + V[0];

        cout << "Bnnn - JP V0, addr" << endl;
    }



}

void Chip8::cycle()
{
    // Fetch
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Decode + Execute
    execute();
}

Chip8::Chip8()
{
    pc = 0x200;   // CHIP-8 programs start here
    sp = 0;
    opcode = 0;

    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
}

int main()
{
    Chip8 emu;
    emu.opcode = 0xB230;
    emu.execute();

    return 0;
}
