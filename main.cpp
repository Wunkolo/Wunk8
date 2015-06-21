#include <iostream>
#include "Wunk8.hpp"
#include <thread>

// Blech....
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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::string RenderScreen(const uint8_t* Buffer,
	size_t Width = 64, size_t Height = 32);

int main()
{
	Wunk8::Chip8 Processor;
	Processor.LoadGame("PONG");
	size_t Frame = 0;
	//std::cin.get();

	while( Processor.Tick(std::chrono::milliseconds(16)) )
	{
		//printf("\t\t\tTick! (%u)\n", Tick++);
		if( Processor.QueryFrame() )
		{
			Frame++;
			// Quickly generate an image sequence to string together later
			stbi_write_png((std::to_string(Frame) + ".png").c_str(), 64, 32, 1, Processor.GetScreen(), 64);
			//clrscreen();
			//std::cout << RenderScreen(Processor.GetScreen(), 64, 32);
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