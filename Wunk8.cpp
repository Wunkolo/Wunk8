#include "Wunk8.hpp"

#include <fstream>
#include <algorithm>

namespace Wunk8
{
Chip8::Chip8(uint32_t Seed)
	:
	Seed(Seed),
	RandEng(Seed)
{
	Reset();
}

Chip8::~Chip8()
{
}

void Chip8::Reset()
{
	// Ram/Rom
	std::fill(std::begin(Memory.Data), std::end(Memory.Data), 0);

	// Load FontSet into memory
	static constexpr uint8_t Chip8Font[] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	std::copy_n(
		std::begin(Chip8Font),
		sizeof(Chip8Font),
		std::begin(Memory.Data));

	// Registers
	// Program counter starts at 0x200
	Registers = { { 0 }, 0, 0x200, 0 };

	// Stack
	std::fill(std::begin(Stack), std::end(Stack), 0);

	// Display
	std::fill(
		std::begin(Display.Screen),
		std::end(Display.Screen),
		0);

	Timer.Delay = Timer.Sound = 0;
	Keyboard.KeyStates = 0;
}

bool Chip8::LoadGame(const std::string &FileName)
{
	if( !FileName.empty() )
	{
		std::ifstream fIn(
			FileName,
			std::ios::binary | std::ios::ate
		);

		if( fIn.good() )
		{
			size_t Length = static_cast<size_t>(fIn.tellg());
			Length = std::min(sizeof(Memory.Data) - 0x200, Length);
			fIn.seekg(0, std::ios::beg);
			fIn.read(
				reinterpret_cast<char*>(Memory.Data) + 0x200,
				Length
			);
			fIn.close();
			return true;
		}
	}
	return false;
}

bool Chip8::LoadGame(const void *Data, size_t Length)
{
	if( Data )
	{
		std::copy_n(
			static_cast<const uint8_t*>(Data),
			Length,
			std::begin(Memory.Data) + 0x200
		);
	}
	return true;
}

bool Chip8::Tick(const std::chrono::milliseconds DeltaTime)
{
	uint16_t Opcode = Memory.Data[Registers.PC++] << 8;
	Opcode |= Memory.Data[Registers.PC++];
	switch( Opcode >> 12 )
	{
	case 0x0:
	{
		switch( Opcode & 0xFFF )
		{
		case 0xE0: // CLS : Clear screen
		{
			std::fill_n(
				std::begin(Display.Screen),
				sizeof(Display.Screen),
				0
			);
			break;
		}
		case 0xEE: // RET : Return from Subroutine
		{
			Registers.PC = Stack[0xF & --Registers.SP];
			break;
		}
		}
		break;
	}
	case 0x1: // JP addr
	{
		Registers.PC = Opcode & 0x0FFF;
		break;
	}
	case 0x2: // CALL addr
	{
		Stack[0xF & Registers.SP++] = Registers.PC;
		Registers.PC = Opcode & 0x0FFF;
		break;
	}
	case 0x3: // SE : Skip if Equal immediate
	{
		// Keep things branchless
		Registers.PC += 2 * (Registers.V[(Opcode >> 8) & 0xF] == (Opcode & 0xFF));
		break;
	}
	case 0x4: // SNE : Skip if not Equal immediate
	{
		Registers.PC += 2 * (Registers.V[(Opcode >> 8) & 0xF] != (Opcode & 0xFF));
		break;
	}
	case 0x5: // SE : Skip if registers equal
	{
		Registers.PC += 2 * (Registers.V[(Opcode >> 8) & 0xF] == Registers.V[(Opcode >> 4) & 0xF]);
		break;
	}
	case 0x6: // LD : Load immediate
	{
		Registers.V[(Opcode >> 8) & 0xF] = Opcode & 0xFF;
		break;
	}
	case 0x7: // ADD: increment immediate
	{
		Registers.V[(Opcode >> 8) & 0xF] += Opcode & 0xFF;
		break;
	}
	case 0x8: // Register operators
	{
		uint8_t *Dest, *Operand;
		Dest = &(Registers.V[(Opcode >> 8) & 0xF]);
		Operand = &(Registers.V[(Opcode >> 4) & 0xF]);
		switch( Opcode & 0xF )
		{
		case 0: // LD : Load register
		{
			*Dest = *Operand;
			break;
		}
		case 1: // OR
		{
			*Dest = *Dest | *Operand;
			break;
		}
		case 2: // AND
		{
			*Dest = *Dest & *Operand;
			break;
		}
		case 3: // XOR
		{
			*Dest = *Dest ^ *Operand;
			break;
		}
		case 4: // ADD /CARRY
		{
			Registers.V[0xF] = (
				(static_cast<size_t>(*Dest) + static_cast<size_t>(*Operand)) > 0xFF
				);
			*Dest += *Operand;
			break;
		}
		case 5: // SUB /BORROW
		{
			Registers.V[0xF] = (static_cast<size_t>(*Dest) > static_cast<size_t>(*Operand));
			*Dest -= *Operand;
			break;
		}
		case 6: // SHR
		{
			Registers.V[0xF] = *Dest & 1;
			*Dest >>= 1;
			break;
		}
		case 7: // SUBN
		{
			Registers.V[0xF] = (static_cast<size_t>(*Operand) > static_cast<size_t>(*Dest));
			*Operand -= *Dest;
			break;
		}
		case 0xE: // SHL
		{
			Registers.V[0xF] = (*Dest & 0x80) >> 7;
			*Dest <<= 1;
			break;
		}
		}
		break;
	}
	case 0x9: // SNE : Skip if not Equal
	{
		Registers.PC += 2 * (Registers.V[(Opcode >> 8) & 0xF] != Registers.V[(Opcode >> 4) & 0xF]);
		break;
	}
	case 0xA: // LD I : Assign Index register
	{
		Registers.I = Opcode & 0x0FFF;
		break;
	}
	case 0xB: // JMP : Relative to V0
	{
		Registers.PC = Registers.V[0] + (Opcode & 0xFFF);
		break;
	}
	case 0xC: // Random number generator
	{
		Registers.V[(Opcode >> 8) & 0xF] = std::uniform_int_distribution<size_t>(0, 0xFF)(RandEng);
		Registers.V[(Opcode >> 8) & 0xF] &= (Opcode & 0xFF);
		break;
	}
	case 0xD: // Draw 8xN sprite at (x,y) with collision flag
	{
		uint8_t SX = Registers.V[(Opcode >> 8) & 0xF];
		uint8_t SY = Registers.V[(Opcode >> 4) & 0xF];
		uint8_t Height = Opcode & 0xF;
		Registers.V[0xF] = 0;
		for( size_t Y = 0; Y < Height; Y++ )
		{
			uint8_t Pixel = Memory.Data[Registers.I + Y];
			for( size_t X = 0; X < 8; X++ )
			{
				if( Pixel & (0x80 >> X) )
				{
					if( Display.Screen[X + SX + ((Y + SY) * Width)] )
					{
						// Collision
						Registers.V[0xF] = 1;
					}
					Display.Screen[X + SX + ((Y + SY) * Width)] ^= 1;
				}
			}
		}

		DeltaFrame = true;
		break;
	}
	case 0xE: // Key press conditionals
	{
		uint8_t Key = (Opcode & 0xF00) >> 8;
		switch( Opcode & 0xFF )
		{
		case 0x9E: // SKP : Skip if key is pressed
		{
			Registers.PC += 2 * ((Keyboard.KeyStates >> Key) & 1);
			break;
		}
		case 0xA1: // SKNP : Skip if key is not pressed
		{
			Registers.PC += 2 * (((Keyboard.KeyStates >> Key) & 1) ^ 1);
			break;
		}
		}
		break;
	}
	case 0xF: //
	{
		uint8_t *Arg = &(Registers.V[(Opcode >> 8) & 0xF]);
		switch( Opcode & 0xFF )
		{
		case 0x07: // LD : Load Delay Timer
		{
			*Arg = Timer.Delay;
			break;
		}
		case 0x0A: // LD : Load upon Keypress
		{
			// honk
			printf("honk");
			break;
		}
		case 0x15: // LD : Set Delay Timer
		{
			Timer.Delay = *Arg;
			break;
		}
		case 0x18: // LD: Set Sound Timer
		{
			Timer.Sound = *Arg;
			break;
		}
		case 0x1E: // ADD : Increment Index
		{
			Registers.I += *Arg;
			break;
		}
		case 0x29: // LD : Set Index to Letter Sprite address
		{
			Registers.I = *Arg * 5;
			break;
		}
		case 0x33: // LD : Store BDC representation of VX at Index
		{
			Memory.Data[Registers.I] = *Arg / 100;
			Memory.Data[Registers.I + 1] = (*Arg / 10) % 10;
			Memory.Data[Registers.I + 2] = *Arg % 10;
			break;
		}
		case 0x55: // LD : Stores all General Registers V0 to VX at Index
		{
			std::copy_n(
				std::begin(Registers.V),
				(Opcode >> 8) & 0xF,
				&Memory.Data[Registers.I]);
			break;
		}
		case 0x65: // LD : Read all General Registers V0 to VX from Index
		{
			std::copy_n(
				&(Memory.Data[Registers.I]),
				((Opcode >> 8) & 0xF) + 1,
				std::begin(Registers.V)
			);
			break;
		}
		default:
			break;
		}
		break;
	}
	}

	// Update timers
	if( Timer.Delay )
	{
		Timer.Delay--;
	}
	if( Timer.Sound )
	{
		Timer.Sound--;
		Timer.Sound || putchar(0x7);// bell character
	}
	return true;
}
}