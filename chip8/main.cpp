#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
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
    uint16_t pc;
    uint8_t V[16];
    uint16_t opcode;
    uint8_t sp; //maybe change to 16
    uint16_t I;

    uint16_t stack[16];

    // Additional CHIP-8 state
    uint8_t gfx[64 * 32];   // Display buffer
    uint8_t delay_timer{};
    uint8_t sound_timer{};
    uint8_t key[16]{};      // Keypad state

    bool drawFlag{false};

    Chip8();
    void cycle();
    void updateTimers();
    bool loadRom(const std::string& path);
    void drawConsole() const;

    void execute();
};

void Chip8::execute()
{

    pc += 2;

    switch(opcode)
    {
////////////////////////////////CLS - TODO pattern
        case 0x00E0://to finish
            // Clear display buffer
            std::memset(gfx, 0, sizeof(gfx));
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
            cout << "SNE V" << x << ", " << kk << " V" << x << " = " << static_cast<int>(V[x]) << endl;
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

        V[x] = static_cast<uint8_t>(V[x] + kk);

        cout << "ADD V" << static_cast<int>(x) << ", " << static_cast<int>(kk) << endl;

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

        uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
        V[0xF] = (sum > 0xFF) ? 1 : 0;
        V[x] = static_cast<uint8_t>(sum & 0xFF);

        cout << "8xy4 - ADD Vx, Vy" << endl;
    }
    // 8xy5, 8xy7, 8xy6, 8xyE
/////////////////////////////////////8xy5 - SUB Vx, Vy
    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==5)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[0xF] = (V[x] > V[y]) ? 1 : 0;
        V[x] = static_cast<uint8_t>(V[x] - V[y]);

        cout << "8xy5 - SUB Vx, Vy" << endl;
    }

/////////////////////////////////// 8xy7 - SUBN Vx, Vy
    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==7)
    {

        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;

        V[0xF] = (V[y] > V[x]) ? 1 : 0;
        V[x] = static_cast<uint8_t>(V[y] - V[x]);

        cout << "8xy7 - SUBN Vx, Vy" << endl;
    }

/////////////////////////////////// 8xy6 - SHR Vx {, Vy}
    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==6)
    {

        auto x = (opcode & 0x0F00) >> 8;

        V[0xF] = V[x] & 0x1u;
        V[x] = static_cast<uint8_t>(V[x] >> 1);

        cout << "8xy6 - SHR Vx {, Vy}" << endl;
    }

/////////////////////////////////// 8xyE - SHL Vx {, Vy}
    if (((opcode & 0xF000)==0x8000)&&(opcode & 0x000F)==0xE)
    {

        auto x = (opcode & 0x0F00) >> 8;

        V[0xF] = (V[x] & 0x80u) >> 7;
        V[x] = static_cast<uint8_t>(V[x] << 1);

        cout << "8xyE - SHL Vx {, Vy}" << endl;
    }

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

///////////////////////////////Annn - LD I, addr

    if ((opcode & 0xF000)==0xA000)
    {

        auto nnn = opcode & 0x0FFF;
        I = nnn; // shifting - no already lwoer bits
        cout << "Annn - LD I, addr" << endl;
    }

///////////////////////////// Cxkk - RND Vx, byte
    if ((opcode & 0xF000) == 0xC000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto kk = opcode & 0x00FF;

        uint8_t rnd = static_cast<uint8_t>(std::rand() & 0xFF);
        V[x] = rnd & kk;

        cout << "Cxkk - RND V" << static_cast<int>(x) << ", byte" << endl;
    }

///////////////////////////// Dxyn - DRW Vx, Vy, nibble
    if ((opcode & 0xF000) == 0xD000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto y = (opcode & 0x00F0) >> 4;
        auto n = opcode & 0x000F;

        uint8_t xPos = V[x] % 64;
        uint8_t yPos = V[y] % 32;

        V[0xF] = 0;

        for (int row = 0; row < n; ++row)
        {
            uint8_t spriteByte = memory[I + row];
            for (int col = 0; col < 8; ++col)
            {
                if ((spriteByte & (0x80u >> col)) != 0)
                {
                    uint16_t idx = (yPos + row) * 64 + (xPos + col);
                    if (idx >= sizeof(gfx))
                        continue;

                    if (gfx[idx] == 1)
                    {
                        V[0xF] = 1;
                    }
                    gfx[idx] ^= 1;
                }
            }
        }

        drawFlag = true;

        cout << "Dxyn - DRW Vx, Vy, nibble" << endl;
    }

///////////////////////////// Ex9E / ExA1 - SKP / SKNP Vx
    if ((opcode & 0xF0FF) == 0xE09E)
    {
        auto x = (opcode & 0x0F00) >> 8;
        if (key[V[x]] != 0)
        {
            pc += 2;
        }
        cout << "Ex9E - SKP Vx" << endl;
    }
    if ((opcode & 0xF0FF) == 0xE0A1)
    {
        auto x = (opcode & 0x0F00) >> 8;
        if (key[V[x]] == 0)
        {
            pc += 2;
        }
        cout << "ExA1 - SKNP Vx" << endl;
    }


///////////////////////////// Fx07, Fx0A, Fx15, Fx18, Fx1E, Fx29, Fx33, Fx55, Fx65
    if ((opcode & 0xF000) == 0xF000)
    {
        auto x = (opcode & 0x0F00) >> 8;
        auto lastByte = opcode & 0x00FF;

        switch (lastByte)
        {
            case 0x07: // LD Vx, DT
                V[x] = delay_timer;
                cout << "Fx07 - LD Vx, DT" << endl;
                break;
            case 0x0A: // LD Vx, K
            {
                bool key_pressed = false;
                for (int i = 0; i < 16; ++i)
                {
                    if (key[i] != 0)
                    {
                        V[x] = static_cast<uint8_t>(i);
                        key_pressed = true;
                        break;
                    }
                }
                // If no key pressed, repeat this instruction
                if (!key_pressed)
                {
                    pc -= 2;
                }
                cout << "Fx0A - LD Vx, K" << endl;
                break;
            }
            case 0x15: // LD DT, Vx
                delay_timer = V[x];
                cout << "Fx15 - LD DT, Vx" << endl;
                break;
            case 0x18: // LD ST, Vx
                sound_timer = V[x];
                cout << "Fx18 - LD ST, Vx" << endl;
                break;
            case 0x1E: // ADD I, Vx
                I = static_cast<uint16_t>(I + V[x]);
                cout << "Fx1E - ADD I, Vx" << endl;
                break;
            case 0x29: // LD F, Vx
                // Each font character is 5 bytes starting at 0x000.
                I = static_cast<uint16_t>(V[x] * 5);
                cout << "Fx29 - LD F, Vx" << endl;
                break;
            case 0x33: // LD B, Vx
            {
                uint8_t value = V[x];
                memory[I]     = value / 100;
                memory[I + 1] = (value / 10) % 10;
                memory[I + 2] = value % 10;
                cout << "Fx33 - LD B, Vx" << endl;
                break;
            }
            case 0x55: // LD [I], Vx
                for (uint8_t i = 0; i <= x; ++i)
                {
                    memory[I + i] = V[i];
                }
                cout << "Fx55 - LD [I], Vx" << endl;
                break;
            case 0x65: // LD Vx, [I]
                for (uint8_t i = 0; i <= x; ++i)
                {
                    V[i] = memory[I + i];
                }
                cout << "Fx65 - LD Vx, [I]" << endl;
                break;
            default:
                // Unhandled Fx** opcode
                break;
        }
    }

/////////////////////////////
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

    std::memset(memory, 0, sizeof(memory));
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));
    std::memset(gfx, 0, sizeof(gfx));

    delay_timer = 0;
    sound_timer = 0;
    drawFlag = false;
}

void Chip8::updateTimers()
{
    if (delay_timer > 0)
    {
        --delay_timer;
    }
    if (sound_timer > 0)
    {
        --sound_timer;
        if (sound_timer == 0)
        {
            std::cout << "BEEP!" << std::endl;
        }
    }
}

bool Chip8::loadRom(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Failed to open ROM: " << path << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > (4096 - 0x200))
    {
        std::cerr << "ROM too large to fit in memory" << std::endl;
        return false;
    }

    std::vector<char> buffer(static_cast<size_t>(size));
    if (!file.read(buffer.data(), size))
    {
        std::cerr << "Failed to read ROM data" << std::endl;
        return false;
    }

    for (std::size_t i = 0; i < static_cast<std::size_t>(size); ++i)
    {
        memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
    }

    pc = 0x200;
    return true;
}

void Chip8::drawConsole() const
{
    // Simple console renderer for 64x32 display
    std::system("cls"); // clear screen (Windows)

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            uint8_t pixel = gfx[y * 64 + x];
            std::cout << (pixel ? '#' : ' ');
        }
        std::cout << '\n';
    }

    std::cout.flush();
}

int main(int argc, char* argv[])
{
    std::string romPath;
    if (argc >= 2)
    {
        romPath = argv[1];
    }
    else
    {
        std::cout << "Usage: chip8 <rom_path>\n";
        return 1;
    }

    Chip8 emu;
    if (!emu.loadRom(romPath))
    {
        return 1;
    }

    // Simple main loop: ~500 Hz CPU, 60 Hz timers
    const int cyclesPerSecond = 500;
    const auto cycleDelay = std::chrono::microseconds(1'000'000 / cyclesPerSecond);
    auto lastTimerTick = std::chrono::high_resolution_clock::now();

    while (true)
    {
        emu.cycle();

        // Update timers at ~60 Hz
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimerTick).count();
        if (elapsedMs >= 16) // ~60Hz
        {
            emu.updateTimers();
            lastTimerTick = now;
        }

        // Draw if needed
        if (emu.drawFlag)
        {
            emu.drawConsole();
            emu.drawFlag = false;
        }

        std::this_thread::sleep_for(cycleDelay);
    }

    return 0;
}
