#include "windows.h"
#include "tools.h"
#define THREAD_COUNT 4 

internal i32 Running;
typedef struct win32_offscreen_buffer {
    BITMAPINFO Info;
    void *Memory;
    i32 Pitch;
    i32 Width;
    i32 Height;
    i32 BytesPerPixel;
}win32_offscreen_buffer;
internal win32_offscreen_buffer GlobalBackBuffer;

typedef struct win32_window_dimension
{
    i32 Width;
    i32 Height;
} win32_window_dimension;

win32_window_dimension GetWindowDimension(HWND Window)
{
    win32_window_dimension dim;
    RECT ClientRect;
    GetClientRect(Window,&ClientRect);
    dim.Width = ClientRect.right - ClientRect.left;
    dim.Height = ClientRect.bottom - ClientRect.top;
    
    return dim;
}
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer,i32 Width, i32 Height)

{
    if(Buffer->Memory)

    {

        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);

    }



    Buffer->Width = Width;

    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;



    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);

    Buffer->Info.bmiHeader.biWidth = Buffer->Width;

    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;

    Buffer->Info.bmiHeader.biPlanes = 1;

    Buffer->Info.bmiHeader.biBitCount = 32;

    Buffer->Info.bmiHeader.biCompression = BI_RGB;



	i32 BitmapMemorySize = Buffer->Width*Buffer->Height*Buffer->BytesPerPixel;

	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    assert(Buffer->Memory);
    Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
}




LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{

    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_SIZE:
        {
            win32_window_dimension Dimension = GetWindowDimension(Window);

            Win32ResizeDIBSection(&GlobalBackBuffer,Dimension.Width, Dimension.Height);

        } break;
        case WM_CLOSE:
        {
            Running = FALSE;
        } break;
        case WM_ACTIVATEAPP:

        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            u32 VKCode = WParam;
            i32 WasDown = ((LParam & (1 << 30))!=0);
            i32 IsDown = ((LParam & (1 << 31))==0);
            if (TRUE){ //WasDown!=IsDown
                if (VKCode == 'W')
                {
                    //YOffset -=4;
                }
                else if (VKCode == 'S')
                {
                    //YOffset += 4;
                }
                else if (VKCode == 'A')
                {
                }
                else if (VKCode == 'D')
                {
                }
                else if (VKCode == 'E')
                {
                }
                else if (VKCode == 'Q')
                {
                }
                else if (VKCode == VK_UP)
                {
                }
                else if (VKCode == VK_DOWN)
                {
                }
                else if (VKCode == VK_RIGHT)
                {
                }
                else if (VKCode == VK_ESCAPE)
                {
                }
                else if (VKCode == VK_SPACE)
                {
                }

                i32 AltKeyWasDown = ((LParam & (1 << 29)!=0));
                if ((VKCode == VK_F4) && AltKeyWasDown)
                {
                    Running = FALSE;
                }
            }
        }break;
        case WM_DESTROY:
        {
            Running = FALSE;
        } break;
        case WM_PAINT:

        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            i32 X = Paint.rcPaint.left;
            i32 Y = Paint.rcPaint.top;
            i32 Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            i32 Width = Paint.rcPaint.right - Paint.rcPaint.left;
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&GlobalBackBuffer,DeviceContext,Dimension.Width,Dimension.Height, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return(Result);
}


Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,HDC DeviceContext, i32 WindowWidth, i32 WindowHeight, i32 X, i32 Y, i32 Width, i32 Height)
{
    StretchDIBits(DeviceContext,

                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */

                  0, 0, Buffer->Width, Buffer->Height,

                  0, 0, WindowWidth, WindowHeight,

                  Buffer->Memory,

                  &Buffer->Info,

                  DIB_RGB_COLORS, SRCCOPY);
}

internal void
RenderWeirdGradient(win32_offscreen_buffer *Buffer,i32 XOffset, i32 YOffset)
{

    i32 Width = Buffer->Width;

    i32 Height = Buffer->Height;

	u8 *Row = (u8 *) Buffer->Memory;

	for(i32 Y=0; Y < Buffer->Height; Y++)
	{
		u32 *Pixel = (u32 *) Row;
		for(i32 X=0; X < Buffer->Width; X++)
		{

			u8 Blue = (X + XOffset);
			u8 Green = (Y + YOffset);
			*Pixel++ = ((Green << 8) | Blue);
		}
		Row = Row + Buffer->Pitch;
	}
}

INT WINAPI WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
  LARGE_INTEGER PerfCounterFrequency;
  QueryPerformanceFrequency(&PerfCounterFrequency);

  WNDCLASS WindowClass = {0};

  WindowClass.style = CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";



  if(RegisterClassA(&WindowClass))

  {

      HWND Window =
           CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "Software Rasterizer",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
      if(Window)
      {
          i32 XOffset = 0;



          //NOTE(ilias): sound test
          i32 SamplesPerSecond = 48000;
          int ToneHz = 256;
          u32 RunningSampleIndex= 0;
          i32 SquareWaveCounter = 0;
          i32 SquareWavePeriod = SamplesPerSecond/ToneHz;
          i32 HalfSquareWavePeriod = SquareWavePeriod / 2;
          i32 BytesPerSample = sizeof(int16_t) * 2;
          i32 SecondaryBufferSize = SamplesPerSecond * BytesPerSample;

          Running = TRUE;

          LARGE_INTEGER LastCounter;
          QueryPerformanceCounter(&LastCounter);

          i64 LastCycleCount = __rdtsc();

          while(Running)
          {
              MSG Message;
              while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
              {
                  if(Message.message == WM_QUIT)
                  {
                      Running = FALSE;
                  }
                  TranslateMessage(&Message);
                  DispatchMessageA(&Message);
              }
              HDC DeviceContext = GetDC(Window);

              win32_window_dimension Dimension = GetWindowDimension(Window);
              RenderWeirdGradient(&GlobalBackBuffer,XOffset, 0);
              Win32DisplayBufferInWindow(&GlobalBackBuffer,DeviceContext, Dimension.Width,Dimension.Height, 0, 0, Dimension.Width, Dimension.Height);

              ReleaseDC(Window, DeviceContext);



              ++XOffset;

              LARGE_INTEGER EndCounter;
              QueryPerformanceCounter(&EndCounter);

              i64 EndCycleCount = __rdtsc();

              i64 CyclesElapsed = EndCycleCount - LastCycleCount; 
              i64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
              i32 MSPerFrame = (1000*CounterElapsed) / PerfCounterFrequency.QuadPart;

              i64 FPS = PerfCounterFrequency.QuadPart / CounterElapsed;

              //NOTE(ilias): must open fucking devenv
              char Buffer[256];
              wsprintf(Buffer, "Miliseconds/frame = %dms, %dFPS, %d cycles/frame\n", MSPerFrame, FPS,CyclesElapsed);
              OutputDebugStringA(Buffer);
              LastCycleCount = EndCycleCount;
              LastCounter.QuadPart = EndCounter.QuadPart;

          }
      }
      else
      {
      }
  }

  else
  {
  }



    return(0);
}
