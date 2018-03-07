#include <iostream>
#include <memory>
#include <thread>

#include "Wunk8.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if defined(_WIN32)
#define SG_DEFINE
#define SG_W32
#include "sg.hpp"
#endif

int main(int argc, char* argv[])
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
		return EXIT_FAILURE;
	}
	std::cout << "Done!" << std::endl;

#if defined(_WIN32)
	sg_init("Wunk8", Wunk8::Chip8::Width * 8, Wunk8::Chip8::Height * 8);
#endif

	std::unique_ptr<uint32_t[]> Screen(new uint32_t[Wunk8::Chip8::Width * Wunk8::Chip8::Height]);

	size_t Frame = 0;
	while( Console.Tick(std::chrono::milliseconds(16)) )
	{
		if( Console.QueryFrame() )
		{
			Frame++;
			for( size_t i = 0; i < (Wunk8::Chip8::Width * Wunk8::Chip8::Height); i++ )
			{
				Screen[i] = Console.GetScreen()[i] ? 0xFFFFFFFF : 0xFF000000;
			}
			stbi_write_png((std::to_string(Frame) + ".png").c_str(), 64, 32, 4, Screen.get(), 64 * 4);
#if defined(_WIN32)
			sg_paint(
				Screen.get(),
				Wunk8::Chip8::Width,
				Wunk8::Chip8::Height
			);
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
#if defined(_WIN32)
		sg_event Event;
		if( sg_poll(&Event) )
		{
			if( Event.type == SG_ev_keydown )
			{
				switch( Event.key )
				{
				case '1':
				{
					Console.KeyDown(1);
					break;
				}
				case '2':
				{
					Console.KeyDown(1 << 1);
					break;
				}
				case '3':
				{
					Console.KeyDown(1 << 2);
					break;
				}
				case '4':
				{
					Console.KeyDown(1 << 3);
					break;
				}
				case 'q':
				{
					Console.KeyDown(1 << 4);
					break;
				}
				case 'w':
				{
					Console.KeyDown(1 << 5);
					break;
				}
				case 'e':
				{
					Console.KeyDown(1 << 6);
					break;
				}
				case 'r':
				{
					Console.KeyDown(1 << 7);
					break;
				}
				case 'a':
				{
					Console.KeyDown(1 << 8);
					break;
				}
				case 's':
				{
					Console.KeyDown(1 << 9);
					break;
				}
				case 'd':
				{
					Console.KeyDown(1 << 10);
					break;
				}
				case 'f':
				{
					Console.KeyDown(1 << 11);
					break;
				}
				case 'z':
				{
					Console.KeyDown(1 << 12);
					break;
				}
				case 'x':
				{
					Console.KeyDown(1 << 13);
					break;
				}
				case 'c':
				{
					Console.KeyDown(1 << 14);
					break;
				}
				case 'v':
				{
					Console.KeyDown(1 << 15);
					break;
				}
				}
			}
			if( Event.type == SG_ev_keyup )
			{
				switch( Event.key )
				{
				case '1':
				{
					Console.KeyUp(1);
					break;
				}
				case '2':
				{
					Console.KeyUp(1 << 1);
					break;
				}
				case '3':
				{
					Console.KeyUp(1 << 2);
					break;
				}
				case '4':
				{
					Console.KeyUp(1 << 3);
					break;
				}
				case 'q':
				{
					Console.KeyUp(1 << 4);
					break;
				}
				case 'w':
				{
					Console.KeyUp(1 << 5);
					break;
				}
				case 'e':
				{
					Console.KeyUp(1 << 6);
					break;
				}
				case 'r':
				{
					Console.KeyUp(1 << 7);
					break;
				}
				case 'a':
				{
					Console.KeyUp(1 << 8);
					break;
				}
				case 's':
				{
					Console.KeyUp(1 << 9);
					break;
				}
				case 'd':
				{
					Console.KeyUp(1 << 10);
					break;
				}
				case 'f':
				{
					Console.KeyUp(1 << 11);
					break;
				}
				case 'z':
				{
					Console.KeyUp(1 << 12);
					break;
				}
				case 'x':
				{
					Console.KeyUp(1 << 13);
					break;
				}
				case 'c':
				{
					Console.KeyUp(1 << 14);
					break;
				}
				case 'v':
				{
					Console.KeyUp(1 << 15);
					break;
				}
				}
			}
		}
#endif
	}

	Screen.reset();
#if defined(_WIN32)
	sg_exit();
#endif

	return EXIT_SUCCESS;
}
