#include "windows.h"
#include <stdlib.h>
#include  "rast.h"
#define THREAD_COUNT 4 
char info_log[512];
Platform p;
i32 YOffset;
char window_name[512];

typedef struct win32_offscreen_buffer {
    BITMAPINFO Info;
    void *data;
    i32 pitch;
    i32 width;
    i32 height;
    i32 bytes_per_pixel;
}win32_offscreen_buffer;
internal win32_offscreen_buffer back_buffer;

typedef struct win32_WinDim
{
    i32 width;
    i32 height;
} win32_WinDim;

internal win32_WinDim win32_get_win_dim(HWND Window)
{
    win32_WinDim dim;
    RECT ClientRect;
    GetClientRect(Window,&ClientRect);
    dim.width = ClientRect.right - ClientRect.left;
    dim.height = ClientRect.bottom - ClientRect.top;
    
    return dim;
}
Win32ResizeDIBSection(win32_offscreen_buffer *buf,i32 width, i32 height)

{
    if(buf->data)
    {

        VirtualFree(buf->data, 0, MEM_RELEASE);

    }



    buf->width = width;

    buf->height = height;
    buf->bytes_per_pixel = 4;
    buf->Info.bmiHeader.biSize = sizeof(buf->Info.bmiHeader);
    buf->Info.bmiHeader.biWidth = buf->width;
    buf->Info.bmiHeader.biHeight = -buf->height;
    buf->Info.bmiHeader.biPlanes = 1;
    buf->Info.bmiHeader.biBitCount = 32;
    buf->Info.bmiHeader.biCompression = BI_RGB;



	i32 BitmapdataSize = buf->width*buf->height*buf->bytes_per_pixel;

	buf->data = VirtualAlloc(0, BitmapdataSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    assert(buf->data);
    buf->pitch = buf->width*buf->bytes_per_pixel;
}


internal i32 win32_paint_buffer_in_window(win32_offscreen_buffer *buf,HDC DeviceContext, i32 window_width, i32 window_height, i32 X, i32 Y, i32 width, i32 height)
{
    StretchDIBits(DeviceContext,

                  /*
                  X, Y, width, height,
                  X, Y, width, height,
                  */

                  0, 0, buf->width, buf->height,

                  0, 0, window_width, window_height,

                  buf->data,

                  &buf->Info,

                  DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK
win32_callback(HWND Window, UINT msg, WPARAM w_param, LPARAM l_param)
{

    LRESULT Result = 0;
    
    switch(msg)
    {
        case WM_SIZE:
        {
            win32_WinDim dim = win32_get_win_dim(Window);
            p.window_width = dim.width;
            p.window_height = dim.height;

            Win32ResizeDIBSection(&back_buffer,dim.width, dim.height);

        } break;
        case WM_CLOSE:
        {
            p.exit = TRUE;
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
            u64 vkey_code = w_param;
            i8 was_down = ((l_param & (1 << 30)) != 0);
            i8 is_down = ((l_param & (1UL << 31)) == 0);

            u64 key_input =0;

            if ((vkey_code >= 'A' && vkey_code <= 'Z') || (vkey_code >='0' && vkey_code <= '9')){
                key_input = (vkey_code >='A' && vkey_code <= 'Z') ? KEY_A + (vkey_code - 'A') : KEY_0 + (vkey_code - '0');
            }else {
                if (vkey_code == VK_F4){
                    key_input = KEY_F4;
                }else if (vkey_code == VK_SPACE){
                    key_input = KEY_SPACE;
                }else if (vkey_code == VK_MENU)
                {
                    key_input= KEY_ALT;
                }else if (vkey_code == VK_TAB)
                 {
                     key_input = KEY_TAB;
                 }
                else if (vkey_code == VK_LEFT)
                {
                    key_input = KEY_LEFT;
                }
                else if (vkey_code == VK_RIGHT)
                {
                    key_input = KEY_RIGHT;
                }
                else if (vkey_code == VK_UP)
                {
                    key_input = KEY_UP;
                }
                else if (vkey_code == VK_DOWN)
                {
                    key_input = KEY_DOWN;
                }
                else if (vkey_code == VK_CONTROL)
                {
                    key_input = KEY_CTRL;
                }

               //handle more keys
            }
            if (is_down){
               if (p.key_down[key_input] == 0)
               {
                   p.key_pressed[key_input] = 1;
               }
               p.key_down[key_input] = 1;
               p.last_key = (i32)key_input;
               if(p.key_down[KEY_ALT] && key_input == KEY_F4)
               {
                   p.exit = TRUE;
               }
            }else 
            {
                p.key_down[key_input] = 0;
                p.key_pressed[key_input] = 0;
            }
        }break;
        case WM_DESTROY:
        {
            p.exit = TRUE;
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT pnt;
            HDC DeviceContext = BeginPaint(Window, &pnt);
            i32 x = pnt.rcPaint.left;
            i32 y = pnt.rcPaint.top;
            i32 height = pnt.rcPaint.bottom - pnt.rcPaint.top;
            i32 width = pnt.rcPaint.right - pnt.rcPaint.left;
            win32_WinDim dim = win32_get_win_dim(Window);
            win32_paint_buffer_in_window(&back_buffer,DeviceContext,dim.width,dim.height, x, y, width, height);
            EndPaint(Window, &pnt);
        } break;
        default:
        {
            Result = DefWindowProc(Window, msg, w_param, l_param);
        } break;
    }

    return(Result);
}



//just writes to the buffer
internal void
render_weird_effect(win32_offscreen_buffer *buf,i32 XOffset, i32 YOffset)
{

    i32 width = buf->width;

    i32 height = buf->height;

	u8 *row = (u8 *) buf->data;

	for(i32 Y=0; Y < buf->height; Y++)
	{
		u32 *pixel = (u32 *) row;
		for(i32 X=0; X < buf->width; X++)
		{

			u8 Blue = (X + XOffset);
			u8 Green = (Y + YOffset);
			*pixel++ = ((Green << 8) | Blue);
		}
		row = row + buf->pitch;
	}
}


internal void
render_main_fbo(win32_offscreen_buffer *buf)
{

    i32 width = buf->width;

    i32 height = buf->height;

	u8 *row = (u8 *) buf->data;

	for(i32 Y=buf->height-1; Y >= 0; --Y)
	{
		u32 *pixel = (u32 *) row;
		for(i32 X=0; X < buf->width; ++X)
		{	
			u8 Red = minimum((u32)255, (u32)(rc.data[4 * (Y*rc.render_width + X) + 0] * 255.f));
			u8 Green = minimum((u32)255,(u32)(rc.data[4 * (Y*rc.render_width + X) + 1] * 255.f));
			u8 Blue = minimum((u32)255,(u32)(rc.data[4 * (Y*rc.render_width + X) + 2] * 255.f));
			*pixel++ = ((Red << 16) | ((Green << 8) | Blue));
		}
		row = row + buf->pitch;
	}
}

INT WINAPI WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
  
  LARGE_INTEGER PerfCounterFrequency;
  QueryPerformanceFrequency(&PerfCounterFrequency);

  WNDCLASS WindowClass = {0};

  WindowClass.style = CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = win32_callback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = ":)";



  if(RegisterClassA(&WindowClass))

  {

      HWND Window =
           CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "Software Rasterizer ",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
	  init();
      if(Window)
      {
          i32 XOffset = 0;



          p.exit = FALSE;

          LARGE_INTEGER LastCounter;
          QueryPerformanceCounter(&LastCounter);

          i64 LastCycleCount = 0;//__rdtsc();
		
          while(!p.exit)
          {
              MSG msg;
              while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
              {
                  if(msg.message == WM_QUIT)
                  {
                      p.exit = FALSE;
                  }
                  TranslateMessage(&msg);
                  DispatchMessageA(&msg);
              }
              HDC DeviceContext = GetDC(Window);

              win32_WinDim dim = win32_get_win_dim(Window);
              update(0.f);
              render();
              if (p.key_pressed[KEY_W])
                  YOffset++;
              if (p.key_pressed[KEY_S])
                  YOffset--;
              //render_weird_effect(&back_buffer,XOffset, YOffset);
			  render_main_fbo(&back_buffer);
              win32_paint_buffer_in_window(&back_buffer, DeviceContext, dim.width,dim.height, 0, 0, dim.width, dim.height);

              ReleaseDC(Window, DeviceContext);



              ++XOffset;

              LARGE_INTEGER EndCounter;
              QueryPerformanceCounter(&EndCounter);

              i64 EndCycleCount = 0;//__rdtsc();

              i64 CyclesElapsed = EndCycleCount - LastCycleCount; 
              i64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
              i32 MSPerFrame = (1000*CounterElapsed) / PerfCounterFrequency.QuadPart;
			  p.current_time += (f32)CounterElapsed / PerfCounterFrequency.QuadPart;
			  //p.current_time += 1.f/60;

              i64 FPS = PerfCounterFrequency.QuadPart / CounterElapsed;
              sprintf(window_name, "Software Rasterizer ");
              sprintf(window_name + str_size("Software Rasterizer "), itoa(FPS, window_name + str_size("Software Rasterizer "), 10));
              SetWindowTextA(Window, window_name);

              //NOTE(ilias): must open fucking devenv
              char buf[256];
              wsprintf(buf, "Miliseconds/frame = %dms, %dFPS, %d cycles/frame\n", MSPerFrame, FPS,CyclesElapsed);
              OutputDebugStringA(buf);
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
