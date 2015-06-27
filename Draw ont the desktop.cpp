//htdc= GetWindowDC( GetTopWindow(hwnd) ); 


// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h> //needed to play a sound using windows also need to insert winmm.lib
					  //C:\Program Files\Microsoft Visual Studio\VC98\Lib


// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"  // class name
#define WINDOW_WIDTH  0              // size of window
#define WINDOW_HEIGHT 0

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// PROTOTYPES /////////////////////////////////////////////


// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[80];                // used to print text
HPEN white_pen; //creates a white pen
HPEN black_pen; //creates a black pen

// FUNCTIONS //////////////////////////////////////////////

//\\\\\\\\\\\\\\\\\\\\\\\\\ WinProc ///////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   //creates a handle to the device context

	//find out what the message is
	switch(msg)
	{	
	
	case WM_CREATE: //called when window is created
		{// do initialization stuff here
			
			white_pen= CreatePen(PS_SOLID,0,RGB(255,255,255));
			black_pen= CreatePen(PS_SOLID,0,RGB(0,0,0));

			return(0);
		} break;

	case WM_PAINT: //called when window needs repainting
		{//simply validate the window
		    
			//ValidateRect(hwnd, NULL);
			hdc = BeginPaint(GetDesktopWindow(),&ps);
		    EndPaint(GetDesktopWindow(),&ps);
			
			return(0);
		} break;

	case WM_DESTROY: 
		{// kill the application			
			
			//delete pens, if you dont delete all pens you'll get resource leeks
			DeleteObject(white_pen);
			DeleteObject(black_pen);

			//close the program
			PostQuitMessage(0);
			return(0);
		} break;

	default:break;

    } // end main switch

	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

//\\\\\\\\\\\\\\\\\\\\\\\\ WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{

	WNDCLASS winclass;	// this will hold the class created
	HWND	 hwnd;		// generic window handle
	MSG		 msg;		// generic message
	HDC		 hmdc;      //gets a dc for the whole screen (i think thats what its doing)
	
	float gravity= 0.2f;
	float velocity= 0.0f;
	float friction= 0.995f;
	RECT rect;
	int ball_radius= 10;
	int ball_x=500,
		ball_y= ball_radius; //balls radius because the ball is draw around a point and if didn't add the radius
	                         //you would see half the ball

	//fill in the window class stucture
	winclass.style			= CS_DBLCLKS | CS_OWNDC;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	winclass.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDR_MENU1);
	winclass.lpszClassName	= WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
							  "Drawing on the Desktop",	 // title
							  WS_MINIMIZE | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						 	  0,0,	   // x,y
							  WINDOW_WIDTH,  // width
						      WINDOW_HEIGHT, // height
							  NULL,	   // handle to parent 
							  NULL,	   // handle to menu
							  hinstance,// instance
							  NULL)))	// creation parms
	{
		MessageBox(hwnd, "Window Could not be Created", NULL, MB_OK); //NULL is default for Error
		return(0);
	}

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance      = hinstance;

	
	// enter main event loop
	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			// test if this is a quit message
			if (msg.message == WM_QUIT)
				break;
	
			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if
    
    	hmdc= GetWindowDC( GetDesktopWindow() ); //gets the handle to the desktop window
												 //which is the window tha tall other windows
												 // and icons are drawn.
		//fill in rect struct for erasing ball
		rect.left = ball_x - ball_radius;
		rect.right = ball_x + ball_radius;
		rect.top = ball_y - ball_radius;
		rect.bottom = ball_y + ball_radius;
		
		//Draw erasing ball
		SelectObject(hmdc, black_pen);
		SelectObject(hmdc, GetStockObject(BLACK_BRUSH));
		Ellipse(hmdc,rect.left, rect.top, rect.right, rect.bottom);
		
		//Now move the ball
		velocity+= gravity;
		velocity*= friction;
		ball_y+= (int) velocity;

		//Redraw the ball in its new position
		
		//fill in rect struct for drawing ball
		rect.left = ball_x - ball_radius;
		rect.right = ball_x + ball_radius;
		rect.top = ball_y - ball_radius;
		rect.bottom = ball_y + ball_radius;
		
		//Draw the ball in its new spot
		SelectObject(hmdc, white_pen);
		SelectObject(hmdc, GetStockObject(WHITE_BRUSH));
		Ellipse(hmdc,rect.left, rect.top, rect.right, rect.bottom);

		if(ball_y >= 370) //if the ball hits the ground, bounce back.
		{
			velocity= -velocity;
			ball_y= 370; //need to rest the y position because of >=, the y could be > 370
			
			//SelectObject(hmdc, GetStockObject(BLACK_BRUSH));
			//SelectObject(hmdc, black_pen);
			//Rectangle(hmdc, 500, 370, 500, 390);
		}
		//SelectObject(hmdc, GetStockObject(BLACK_BRUSH));
		//SelectObject(hmdc, black_pen);
		//Rectangle(hmdc, 0, 50, 1240, 900);

		//slow things down a bit
		Sleep(10);
		ReleaseDC(GetDesktopWindow(), hmdc);

	} // end while

	

	// return to Windows like this
	return(msg.wParam);

} // end WinMain


