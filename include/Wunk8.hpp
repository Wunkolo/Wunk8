#pragma once
#include <cstdint>
#include <chrono>
#include <random>

namespace Wunk8
{
class Chip8
{
public:
	explicit Chip8(std::uint32_t Seed = 0);
	~Chip8();

	// Sets a default Chip8 Processor state
	void Reset();

	// Loads a Chip8 Program from a file
	bool LoadGame(const std::string& FileName);

	// Loads a Chip8 Program from memory
	bool LoadGame(const void* Data, std::size_t Length);

	// Simulates complete cycles for the designated amount of time
	bool Tick(const std::chrono::milliseconds DeltaTime);

	// Input
	void KeyDown(std::uint16_t Key)
	{
		Keyboard.KeyStates |= Key;
	}

	void KeyUp(std::uint16_t Key)
	{
		Keyboard.KeyStates &= ~(Key);
	}

	// Gets Current Screen
	const std::uint8_t* GetScreen() const
	{
		return &Display.Screen[0];
	};

	static constexpr std::size_t Width = 64;
	static constexpr std::size_t Height = 32;

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
	std::uint32_t Seed;
	std::mt19937 RandEng;

	bool DeltaFrame;

	// RAM/ROM space:
	// 0xFFF(4096) bytes of Total Ram
	// 0x000 to 0x1FF(512 bytes)	: Reserved for Interpretor
	// 0x200 						: Start of most Chip-8 Programs
	// 0x600						: Start of ETI 660 Chip-8 programs
	struct
	{
		std::uint8_t Data[0xFFF];
	} Memory;

	struct
	{
		// General registers:
		// 16 general purpose 8 bit registers
		// Usually referred to as Vx.
		// x being a hexidecimal digit 0-F.
		std::uint8_t V[16];

		// Index register:
		// Typically used to store memory addresses
		// Only lowest 12 bits are used(0x7FF mask)
		std::uint16_t I;

		// Program Counter:
		// Stores the currently executing address
		std::uint16_t PC;

		// Stack Pointer:
		// Pointer to the top-most level of the stack.
		std::uint16_t SP;
	} Registers;

	// Stack:
	// Stack has a maximum depth of 16 subroutines
	std::uint16_t Stack[16];

	// Keyboard
	// Array of 16 binary flags for each key
	// Keypad layout:
	// 1 2 3 C
	// 4 5 6 D
	// 7 8 9 E
	// A 0 B F
	union
	{
		std::uint16_t KeyStates;

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
		std::uint8_t Screen[Width * Height];
	} Display;

	// Timers:
	// Timers count down to zero when set to a
	// non-zero value.
	struct
	{
		// Delay Timer decrements by 1 at a
		// rate of 60 hz.
		std::uint8_t Delay;
		// Sound Timer decrements by 1 at a
		// rate of 60 hz. A sound is to play
		// when 0 is reached.
		std::uint8_t Sound;
	} Timer;

	// 16 ms per tick
	static constexpr std::chrono::milliseconds TimerRate = std::chrono::milliseconds(16);
};
}
