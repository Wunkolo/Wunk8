#pragma once
#include <string>
#include <stdint.h>
#include <chrono>
#include <random>

namespace Wunk8
{
class Chip8
{
public:
	Chip8(uint32_t Seed = 0);
	~Chip8();

	// Sets a default Chip8 Processor state
	void Reset();

	// Loads a Chip8 Program from a file
	bool LoadGame(const std::string& FileName);

	// Loads a Chip8 Program from memory
	bool LoadGame(const void* Data, size_t Length);

	// Simulates complete cycles for the designated amount of time
	bool Tick(const std::chrono::duration<double, std::chrono::seconds::period> DeltaTime);

	// Input
	inline void KeyDown(uint16_t Key)
	{
		Keyboard.KeyStates |= Key;
	}
	inline void KeyUp(uint16_t Key)
	{
		Keyboard.KeyStates &= ~(Key);
	}

	// Gets Current Screen
	const uint8_t* GetScreen() const
	{
		return &Display.Screen[0];
	};

	bool QueryFrame()
	{
		if( DeltaFrame )
		{
			DeltaFrame = false;
			return true;
		}
		return false;
	}

private:
	// Seed used for random number generation
	uint32_t Seed;
	std::mt19937 RandEng;

	bool DeltaFrame;

	// RAM/ROM space:
	// 0xFFF(4096) bytes of Total Ram
	// 0x000 to 0x1FF(512 bytes)	: Reserved for Interpretor
	// 0x200 						: Start of most Chip-8 Programs
	// 0x600						: Start of ETI 660 Chip-8 programs
	struct
	{
		uint8_t Data[0xFFF];
	} Memory;

	struct
	{
		// General registers:
		// 16 general purpose 8 bit registers
		// Usually referred to as Vx.
		// x being a hexidecimal digit 0-F.
		uint8_t V[16];

		// Index register:
		// Typically used to store memory addresses
		// Only lowest 12 bits are used(0x7FF mask)
		uint16_t I;

		// Program Counter:
		// Stores the currently executing address
		uint16_t PC;

		// Stack Pointer:
		// Pointer to the top-most level of the stack.
		uint16_t SP;
	} Registers;

	// Stack:
	// Stack has a maximum depth of 16 subroutines
	uint16_t Stack[16];

	// Keyboard
	// Array of 16 binary flags for each key
	// Keypad layout:
	// 1 2 3 C
	// 4 5 6 D
	// 7 8 9 E
	// A 0 B F
	union
	{
		uint16_t KeyStates;
		struct
		{
			bool
				Key1 : 1, Key2 : 1, Key3 : 1, KeyC : 1,
				Key4 : 1, Key5 : 1, Key6 : 1, KeyD : 1,
				Key7 : 1, Key8 : 1, Key9 : 1, KeyE : 1,
				KeyA : 1, Key0 : 1, KeyB : 1, KeyF : 1;
		};
	} Keyboard;

	// Display:
	// 64x32 screen(2048 pixels) at 1bpp.
	// Top-left of display at coordinate (0,0)
	// --------------------------
	// |(0,0)             (63,0)|
	// |                        |
	// |                        |
	// |                        |
	// |                        |
	// |(0,31)           (63,31)|
	// --------------------------
	struct
	{
		uint8_t Screen[64 * 32];
	} Display;

	// Timers:
	// Timers count down to zero when set to a
	// non-zero value.
	struct
	{
		// Delay Timer decrements by 1 at a
		// rate of 60 hz.
		uint8_t Delay;
		// Sound Timer decrements by 1 at a
		// rate of 60 hz. A sound is to play
		// when 0 is reached.
		uint8_t Sound;
	} Timer;
};
}