#include <iostream>
#include "Wunk8.hpp"
#define _GLIBCXX_USE_NANOSLEEP
#include <thread>

#if defined _WIN32 || defined _WIN64
#include <windows.h>
void clrscreen()
{
	DWORD n;							/* Number of characters written */
	COORD coord = { 0 };				/* Top left screen position */
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(h, &csbi);
	DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &n);
	GetConsoleScreenBufferInfo(h, &csbi);
	FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);
	SetConsoleCursorPosition(h, coord);
}
#elif defined __unix__
void clrscreen()
{
	puts("\033[2J\033[1;1H");
}
#else
void clrscreen()
{
}
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::string RenderScreen(const uint8_t* Buffer,
	size_t Width = 64, size_t Height = 32);

int main(int argc, char *argv[])
{
	if( argc != 2 )
	{
		std::cout << "Usage: " << argv[0] << ' ' << "(Chip8 ROM file)" << std::endl;
		return 0;
	};

	Wunk8::Chip8 Console;

	std::cout << "Loading chip8 rom: " << argv[1] << "..." << std::endl;

	if( !Console.LoadGame(std::string(argv[1])) )
	{
		std::cout << "Failed!" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Done!" << std::endl;

	size_t Frame = 0;
	while( Console.Tick(std::chrono::milliseconds(16)) )
	{
		if( Console.QueryFrame() )
		{
			Frame++;
			// Quickly generate an image sequence to string together later
			//stbi_write_png((std::to_string(Frame) + ".png").c_str(), 64, 32, 1, Console.GetScreen(), 64);
			clrscreen();
			std::cout << RenderScreen(Console.GetScreen(), 64, 32);
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	return 0;
}

std::string RenderScreen(const uint8_t* Buffer,
	size_t Width, size_t Height)
{
	std::string Result;
	size_t ColumnCount = 0;
	Result += '\xDA' + std::string(Width, '\xC4') + '\xBF' + '\n' + '\xB3';
	for( size_t i = 0; i < (Width*Height); i++ )
	{
		Result += Buffer[i] ? '\xDB' : '\x20';
		if( (i + 1) % Width == 0 )
		{
			Result += "\xB3\n\xB3";
		}
	}
	Result += "\b""\xC0" + std::string(Width, '\xC4') + '\xD9' + '\n';
	return Result;
}

// Binary image
//std::string RenderScreen(const uint8_t* Buffer,
//	size_t Width, size_t Height)
//{
//	std::string Result;
//	size_t ColumnCount = 0;
//	Result += '\xDA' + std::string(Width, '\xC4') + '\xBF' + '\n' + '\xB3';
//	for( size_t i = 0; i < (Width*Height) / 8; i++ )
//	{
//		for( size_t j = 0; j < 8; j++ )
//		{
//			Result += (Buffer[i] & (0x80 >> j)) ? '\xDB' : '\x20';
//			if( (ColumnCount++ + 1) % Width == 0 )
//			{
//				Result += "\xB3\n\xB3";
//			}
//		}
//	}
//	Result += "\b""\xC0" + std::string(Width, '\xC4') + '\xD9' + '\n';
//	return Result;
//}
