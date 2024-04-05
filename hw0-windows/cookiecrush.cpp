
#include <Windows.h>
#include <Mmsystem.h>
#include <mciapi.h>


#ifndef COOKIE_CRUSH_CPP
#define COOKIE_CRUSH_CPP

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include "util.h"
#include<fstream>
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

#define KEY_ESC 27 // A

// 20,30,30


int width = 800; int height = 600; // i have set my window size to be 800 x 600


float score = 0; // Variable For Maintaining Score 
int Time = 8; // Variable For Time Function
bool Sound_Check = true; // Enable or Disable Sound ..


enum GameState { // Use to check different states of game...
	Begin, Menu, Help, Options, Ready, Pause, End
};
GameState gamestate = Begin;

const int ncookies = 6;
enum Cookies {
	CK_BISCUIT, CK_BURGER, CK_CROISSONT, CK_CUPCAKE, CK_DONUT, CK_STAR
};
GLuint texture[ncookies];
GLuint tid[ncookies];
string tnames[] = { "biscuit.png", "burger.png", "croissont.png", "cupcake.png",
"donut.png", "star.png" };
GLuint mtid[ncookies];
int cwidth = 60, cheight = 60; // 60x60 pixels cookies width & height...

void RegisterTextures()
/*Function is used to load the textures from the files and display*/
// Please do not change it...
{
	// allocate a texture name
	glGenTextures(ncookies, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);
	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < ncookies; ++i) {

		// Read current cookie

		ifile.read((char*)&data[0], sizeof(char) * length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, cwidth, cheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawCookie(const Cookies& cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	//	glTranslatef(0, 0, 0);
	//	glRotatef(-M_PI / 2, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}


// Defining An Array For Cookies ...
int Cookies_Array_2D[100][100] = { 0 };

// Finding Rows And Columns For Cookies
int Rows = (height - 20) / cheight;
int Columns = (width - 20) / cwidth;



// Intializing Random Values To Cookies Array ...
void Making_2D_Cookies_Array()
{
	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Columns; j++)
		{
			Cookies_Array_2D[i][j] = GetRandInRange(0, 6);
		}
	}
}

// Displaying Cookies Array On The Board 
void Display_Cookies_Array()
{
	int x_axis = 10;
	int y_axis = 10;
	for (int i = 0; i < Rows; i++, y_axis += cheight)
	{
		x_axis = 10;
		for (int j = 0; j < Columns; j++, x_axis += cwidth)
		{
			DrawCookie(Cookies(Cookies_Array_2D[i][j]), x_axis, y_axis, cwidth, cheight);
		}
	}
}

// Checking Initial Combinations For 3 or More Cookies & Displaying Empty Cookies ...
void Check_Initial_Combinations()
{
	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Columns; j++)
		{
			// Horizontal Combinations
			if (Cookies_Array_2D[i][j] == Cookies_Array_2D[i][j + 1] && Cookies_Array_2D[i][j] == Cookies_Array_2D[i][j + 2])
			{

				bool Condition = true;
				int k = j + 3;
				int j_count = 0;
				do
				{
					if (Cookies_Array_2D[i][j] == Cookies_Array_2D[i][k])
					{
						Cookies_Array_2D[i][k] = 6;
						k++;
						j_count += 1;
					}
					else
					{
						Condition = false;
					}
				} while (Condition == true);

				Cookies_Array_2D[i][j] = 6; // Empty Space
				Cookies_Array_2D[i][j + 1] = 6;
				Cookies_Array_2D[i][j + 2] = 6;

				j += 2 + j_count;

				if (gamestate == Ready)
				{
					score += 3 + j_count;
					if (Sound_Check == true)
					{
						PlaySound(TEXT(".\\cookie.wav"), NULL, SND_ASYNC);
					}

				}


			}
			else
			{
				// Vertical Combinations
				if (Cookies_Array_2D[i][j] == Cookies_Array_2D[i + 1][j] && Cookies_Array_2D[i][j] == Cookies_Array_2D[i + 2][j])
				{
					bool Condition = true;
					int k = i + 3;
					int score_temp = 0;
					do
					{
						if (Cookies_Array_2D[i][j] == Cookies_Array_2D[k][j])
						{
							Cookies_Array_2D[k][j] = 6;
							k++;
							//score_temp += 1;
						}
						else
						{
							Condition = false;
						}
					} while (Condition == true);


					Cookies_Array_2D[i][j] = 6;
					Cookies_Array_2D[i + 1][j] = 6;
					Cookies_Array_2D[i + 2][j] = 6;

					if (gamestate == Ready)
					{
						if (Sound_Check == true)
						{
							PlaySound(TEXT(".\\cookie.wav"), NULL, SND_ASYNC);
						}
						score += 3 + score_temp;
					}


				}
			}
		}
	}

}

// Dropping Cookies From Above After Crushing Combinations
void Dropping_Cookies()
{
	for (int j = 0; j < Columns; j++)
	{
		for (int i = 0; i < Rows; i++)
		{
			if (Cookies_Array_2D[i][j] == 6)
			{
				int k = i;
				for (; k < Rows - 1; k++)
				{
					int Temp = Cookies_Array_2D[k][j];
					Cookies_Array_2D[k][j] = Cookies_Array_2D[k + 1][j];
					Cookies_Array_2D[k + 1][j] = Temp;
				}
				Cookies_Array_2D[k][j] = GetRandInRange(0, 6);
			}
		}
	}
}



// Functions For Front Page
void Front_Cookies()
{
	int x_axis = 10;
	int y_axis = 10;
	for (int i = 0; i < Rows; i++, y_axis += cheight)
	{
		x_axis = 10;
		for (int j = 0; j < Columns; j++, x_axis += cwidth)
		{
			if ((i < 2 || i > Rows - 3) || (j < 4 || j > Columns - 5))
			{
				DrawCookie(Cookies(GetRandInRange(0, 5)), x_axis, y_axis, cwidth, cheight);
			}
		}
	}
}

// Rotating Front Page Cookies 
void Rotate_Horizontal_Front_Cookies()
{

	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Columns; j++)
		{
			if ((i < 2 || i > Rows - 3) || (j < 4 || j > Columns - 5))
			{
				int Temp = Cookies_Array_2D[i][j];
				Cookies_Array_2D[i][j] = Cookies_Array_2D[i][j + 1];
				Cookies_Array_2D[i][j + 1] = Temp;
			}
		}
	}
}


// Function For Changing Background Colour .
int r = 0, g = 0, b = 0;
void Background_Colour(int r = 0, int g = 0, int b = 0)
{
	glClearColor(r/*Red Component*/, g/*Green Component*/,
		b/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors
}


void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0

	Background_Colour();

	//drawing commands or functions here...

	if (gamestate == Begin)
	{

		Check_Initial_Combinations();
		Dropping_Cookies();

		DrawString(10, 10, width, height + 5, "Loading in " + Num2Str(Time) + " secs", colors[RED]);
		DrawString(10, height - 30, width, height + 5, "Loading Textures..... ", colors[RED]);
		DrawString(10, height - 60, width, height + 5, "Loading Detail..... ", colors[RED]);
		DrawString(10, height - 90, width, height + 5, "Loading Canvas..... ", colors[RED]);
		DrawString(10, height - 120, width, height + 5, "Loading Cookies..... ", colors[RED]);
		DrawString(10, height - 150, width, height + 5, "Checking Errors..... ", colors[RED]);
		DrawString(10, height - 180, width, height + 5, "Compiling cookiecrush.cpp ..... ", colors[RED]);
		DrawString(10, height - 210, width, height + 5, "Linking # Files..... ", colors[RED]);
		DrawString(10, height - 240, width, height + 5, "Executing CookieCrush.exe ..... ", colors[RED]);
		DrawString(10, height - 270, width, height + 5, "Enjoy..... ", colors[RED]);
		DrawString(10, height - 300, width, height + 5, "Support The Developers ..... ", colors[RED]);
		DrawString(10, height - 330, width, height + 5, "Denevo Protected ", colors[RED]);
		DrawString(10, height - 360, width, height + 5, "Copyright All Rights Reserved..... ", colors[RED]);
	}

	if (gamestate == Menu)
	{
		Front_Cookies();

		DrawString((width / 2) - 100, height - 30, width, height + 5, "COOKIE CRUSH ", colors[RED]);
		DrawString((width - 100), height - 30, width, height + 5, "Ver 1.01 ", colors[RED]);
		DrawString((width / 2) - 80, (height / 2) + 70, width, height + 5, "START GAME ", colors[RED]);
		DrawString((width / 2) - 80, (height / 2), width, height + 5, "OPTIONS ", colors[RED]);
		DrawString((width / 2) - 80, (height / 2) - 70, width, height + 5, "HELP ", colors[RED]);
		DrawString((width / 2) - 80, (height / 2) - 140, width, height + 5, "EXIT ", colors[RED]);

	}

	if (gamestate == Help)
	{
		Front_Cookies();
		DrawString((width / 2) - 100, height - 30, width, height + 5, "COOKIE CRUSH ", colors[RED]);
		DrawString((width - 100), height - 30, width, height + 5, "Ver 1.01 ", colors[RED]);

		DrawString((width / 2) - 200, (height / 2) + 100, width, height + 5, "Swap Vertical Or Horizontal Cookies ", colors[RED]);
		DrawString((width / 2) - 200, (height / 2) + 60, width, height + 5, "To Make Combinations Of 3 or More. ", colors[RED]);
		DrawString((width / 2) - 200, (height / 2) - 20, width, height + 5, "Each Cookie Crushed Awards 1 Point. ", colors[RED]);
		DrawString((width / 2) - 200, (height / 2) - 100, width, height + 5, "ENJOY !!  ", colors[RED]);
		DrawString((width / 2) - 40, (height / 2) - 180, width, height + 5, "BACK", colors[WHITE_SMOKE]);
	}


	if (gamestate == Options)
	{
		Front_Cookies();
		DrawString((width / 2) - 100, height - 30, width, height + 5, "COOKIE CRUSH ", colors[RED]);
		DrawString((width - 100), height - 30, width, height + 5, "Ver 1.01 ", colors[RED]);
		DrawString((width / 2) - 40, (height / 2) - 180, width, height + 5, "BACK", colors[CHOCOLATE]);

		DrawString((width / 2) - 135, (height / 2) + 120, width, height + 5, "BACKGROUND COLOUR", colors[WHITE_SMOKE]);

		DrawString((width / 2) - 160, (height / 2) + 60, width, height + 5, "WHITE", colors[RED]);
		DrawString((width / 2) - 40, (height / 2) + 60, width, height + 5, "BLACK", colors[RED]);
		DrawString((width / 2) + 80, (height / 2) + 60, width, height + 5, "BLUE", colors[RED]);
		DrawString((width / 2) - 120, (height / 2), width, height + 5, "GREEN", colors[RED]);
		DrawString((width / 2), (height / 2), width, height + 5, "YELLOW", colors[RED]);

		DrawString((width / 2) - 50, (height / 2) - 80, width, height + 5, "SOUND", colors[WHITE_SMOKE]);

		DrawString((width / 2) - 140, (height / 2) - 120, width, height + 5, "ON", colors[RED]);
		DrawString((width / 2) + 80, (height / 2) - 120, width, height + 5, "OFF", colors[RED]);
	}

	if (gamestate == Ready)
	{
		Background_Colour(r, g, b);
		Display_Cookies_Array(); // Placing Cookies On Board Using Cookies Array	
		DrawString(40, height - 30, width, height + 5, "Score " + Num2Str(score), colors[RED]);
		DrawString((width / 2) - 80, height - 30, width, height + 5, "Time : " + Num2Str(Time) + " secs", colors[RED]);
		DrawString((width / 2) + 160, height - 30, width, height + 5, "PAUSE ", colors[RED]);
		DrawString((width - 80), height - 30, width, height + 5, "EXIT ", colors[RED]);
		Check_Initial_Combinations(); // Checking Intial Combinations For 3 or More Cookies	
		Dropping_Cookies();

	}

	if (gamestate == Pause)
	{

		DrawString(40, height - 30, width, height + 5, "Score " + Num2Str(score), colors[RED]);
		DrawString((width / 2) - 80, height - 30, width, height + 5, "Time : " + Num2Str(Time) + " secs", colors[RED]);
		DrawString((width / 2) - 80, height / 2, width, height + 5, "GAME PAUSED ", colors[RED]);
		DrawString((width / 2) - 180, (height / 2) - 50, width, height + 5, "CLICK ANYWHERE TO CONTINUE ", colors[RED]);
	}

	if (gamestate == End)
	{
		DrawString((width / 2) - 50, (height / 2) + 20, width, height + 5, "Game Over", colors[RED]);
		DrawString((width / 2) - 50, (height / 2) - 50, width, height + 5, "Score : " + Num2Str(score), colors[RED]);
		DrawString((50), (50), width, height + 5, "Press Esc To Exit", colors[RED]);
	}


	// do not modify below this
	DrawString(width, height, width, height, "", colors[WHITE_SMOKE]);
	glutSwapBuffers(); // do not modify this line..

}
/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...

	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {

	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {

	cout << "Current Mouse Coordinates X=" << x << " Y= " << height - y << endl;
	glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

int Selected_Cookie;
int Selected_Cookie_ith_index;
int Selected_Cookie_jth_index;

int Swapping_Cookie;
int Swapping_Cookie_ith_index;
int Swapping_Cookie_jth_index;

int ith_index;
int jth_index;

int click_count = 0;

void Swap_Function(int& Selected_Cookie, int i_1, int j_1, int& Swapping_Cookie, int i_2, int j_2)
{
	int Temp = Selected_Cookie;
	Selected_Cookie = Swapping_Cookie;
	Swapping_Cookie = Temp;

	Cookies_Array_2D[i_1][j_1] = Selected_Cookie;
	Cookies_Array_2D[i_2][j_2] = Swapping_Cookie;

	if (((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 - 1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 + 1])) &&
		((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 - 1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 - 2])) &&
		((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 + 1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1][j_1 + 2])) &&

		((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 - 1][j_1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 + 1][j_1])) &&
		((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 - 1][j_1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 - 2][j_1])) &&
		((Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 + 1][j_1]) || (Cookies_Array_2D[i_1][j_1] != Cookies_Array_2D[i_1 + 2][j_1])) &&

		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 - 1]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 + 1])) &&
		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 - 1]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 - 2])) &&
		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 + 1]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2][j_2 + 2])) &&

		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 - 1][j_2]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 + 1][j_2])) &&
		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 - 1][j_2]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 - 2][j_2])) &&
		((Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 + 1][j_2]) || (Cookies_Array_2D[i_2][j_2] != Cookies_Array_2D[i_2 + 2][j_2])))
	{
		int Temp = Selected_Cookie;
		Selected_Cookie = Swapping_Cookie;
		Swapping_Cookie = Temp;

		Cookies_Array_2D[i_1][j_1] = Selected_Cookie;
		Cookies_Array_2D[i_2][j_2] = Swapping_Cookie;
	}
}
void MouseClicked(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{

		if (state == GLUT_DOWN && gamestate == Menu)
		{
			cout << "Left Mouse Button Pressed at Coordinates X=" << x << " Y= "
				<< height - y << endl;

			// Start Game ..
			if (((x >= (width / 2) - 80) && (x <= (width / 2) - 80 + 160)) && ((height - y) >= (height / 2) + 70) && ((height - y) <= (height / 2) + 70 + 20))
			{
				gamestate = Ready;
			}

			// Options ..
			if (((x >= (width / 2) - 80) && (x <= (width / 2) - 80 + 100)) && ((height - y) >= (height / 2)) && ((height - y) <= (height / 2) + 20))
			{
				gamestate = Options;
			}

			// Help
			if (((x >= (width / 2) - 80) && (x <= (width / 2) - 80 + 60)) && ((height - y) >= (height / 2) - 70) && ((height - y) <= (height / 2) - 70 + 20))
			{
				gamestate = Help;
			}

			// Exit ...
			if (((x >= (width / 2) - 80) && (x <= (width / 2) - 80 + 55)) && ((height - y) >= (height / 2) - 140) && ((height - y) <= (height / 2) - 140 + 20))
			{
				exit(1);

			}
		}
	}

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_DOWN && gamestate == Help)
		{
			cout << "Left Mouse Button Pressed at Coordinates X=" << x << " Y= "
				<< height - y << endl;

			// Back Button
			if (((x >= (width / 2) - 40) && (x <= (width / 2) - 40 + 70)) && ((height - y) >= (height / 2) - 180) && ((height - y) <= (height / 2) - 180 + 20))
			{
				gamestate = Menu;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_DOWN && gamestate == Options)
		{
			cout << "Left Mouse Button Pressed at Coordinates X=" << x << " Y= "
				<< height - y << endl;

			//White Colour ..
			if (((x >= (width / 2) - 160) && (x <= (width / 2) - 160 + 85)) && ((height - y) >= (height / 2) + 60) && ((height - y) <= (height / 2) + 60 + 20))
			{
				r = 1;
				g = 1;
				b = 1;
			}

			//Black Colour ..
			if (((x >= (width / 2) - 40) && (x <= (width / 2) - 40 + 85)) && ((height - y) >= (height / 2) + 60) && ((height - y) <= (height / 2) + 60 + 20))
			{
				r = 0;
				g = 0;
				b = 0;
			}

			//Blue Colour ..
			if (((x >= (width / 2) + 80) && (x <= (width / 2) + 80 + 65)) && ((height - y) >= (height / 2) + 60) && ((height - y) <= (height / 2) + 60 + 20))
			{
				r = 0;
				g = 1;
				b = 1;
			}

			//Green Colour ..
			if (((x >= (width / 2) - 120) && (x <= (width / 2) - 120 + 85)) && ((height - y) >= (height / 2)) && ((height - y) <= (height / 2) + 20))
			{
				r = 0;
				g = 1;
				b = 0;
			}

			//Yellow Colour ..
			if (((x >= (width / 2)) && (x <= (width / 2) + 100)) && ((height - y) >= (height / 2)) && ((height - y) <= (height / 2) + 20))
			{
				Background_Colour(1, 1, 0);
				r = 1;
				g = 1;
				b = 0;
			}

			// Back Button ..
			if (((x >= (width / 2) - 40) && (x <= (width / 2) - 40 + 70)) && ((height - y) >= (height / 2) - 180) && ((height - y) <= (height / 2) - 180 + 20))
			{
				gamestate = Menu;
			}

			// Sound ON ..
			if (((x >= (width / 2) - 140) && (x <= (width / 2) - 140 + 40)) && ((height - y) >= (height / 2) - 120) && ((height - y) <= (height / 2) - 120 + 20))
			{
				Sound_Check = true;
			}

			// Sound OFF ..
			if (((x >= (width / 2) + 80) && (x <= (width / 2) + 80 + 50)) && ((height - y) >= (height / 2) - 120) && ((height - y) <= (height / 2) - 120 + 20))
			{
				Sound_Check = false;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_DOWN && gamestate == Pause)
		{
			cout << "Left Mouse Button Pressed at Coordinates X=" << x << " Y= "
				<< height - y << endl;

			// Resuming Game
			if ((x >= 0 && x <= width) && (height - y >= 0 && height - y <= height))
			{
				gamestate = Ready;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_DOWN && gamestate == Ready)
		{
			cout << "Left Mouse Button Pressed at Coordinates X=" << x << " Y= "
				<< height - y << endl;

			// Dealing Cookies ..

			if ((x >= 10 && x <= (width - 10)) && ((height - y) >= 10 && (height - y) <= (height - 50)))
			{
				ith_index = (height - y) / 60;
				jth_index = x / 60;
				cout << ith_index << " " << jth_index << endl;

				if (click_count % 2 == 0)
				{
					Selected_Cookie = Cookies_Array_2D[ith_index][jth_index];
					Selected_Cookie_ith_index = ith_index;
					Selected_Cookie_jth_index = jth_index;
				}
				else
				{
					Swapping_Cookie = Cookies_Array_2D[ith_index][jth_index];
					Swapping_Cookie_ith_index = ith_index;
					Swapping_Cookie_jth_index = jth_index;

					if (
						((Selected_Cookie_ith_index - Swapping_Cookie_ith_index == 1) && (Selected_Cookie_jth_index == Swapping_Cookie_jth_index)) ||
						((Swapping_Cookie_ith_index - Selected_Cookie_ith_index == 1) && (Selected_Cookie_jth_index == Swapping_Cookie_jth_index)) ||
						((Selected_Cookie_jth_index - Swapping_Cookie_jth_index == 1) && (Selected_Cookie_ith_index == Swapping_Cookie_ith_index)) ||
						((Swapping_Cookie_jth_index - Selected_Cookie_jth_index == 1) && (Selected_Cookie_ith_index == Swapping_Cookie_ith_index))
						)
					{

						Swap_Function(Selected_Cookie, Selected_Cookie_ith_index, Selected_Cookie_jth_index, Swapping_Cookie, Swapping_Cookie_ith_index, Swapping_Cookie_jth_index);
					}

				}
				click_count += 1;
			}

			// Pause ..
			if (((x >= (width / 2) + 160) && (x <= (width / 2) + 160 + 85)) && (((height - y) >= height - 30) && ((height - y) <= height - 30 + 20)))
			{
				gamestate = Pause;
			}

			// Exit ..
			if (((x >= (width - 80)) && (x <= (width - 80 + 60))) && (((height - y) >= height - 30) && ((height - y) <= height - 30 + 20)))
			{
				gamestate = Menu;
				score = 0;
				Time = 120;
			}

		}
	}


	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{

	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
}

/*
* This function is called after every 1000.0 milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */



float dt = 0, lastframe = 0;
void Timer(int m) {
	dt = (m - lastframe) / 1000.0;
	lastframe = m;

	// dt is time elapsed between two frames..
	glutPostRedisplay();

	// implement your functionality here
	if (gamestate == Begin)
	{
		Time -= dt * 1000;
		if (Time == 0)
		{
			Time = 120;
			gamestate = Menu;
		}
	}
	if (gamestate == Ready)
	{
		Time -= dt * 1000;
		if (Time == 0)
		{
			gamestate = End;
		}
	}

	glutTimerFunc(1000, Timer, m + 1);


}

// Function to handle window resize event
void ReshapeFunction(int w, int h) {
	// Ignore resize attempts
	glutReshapeWindow(width, height);
}

/*
* our gateway main function
* */



int main(int argc, char* argv[]) {


	InitRandomizer(); // seed the random number generator...
	//code here for filling the canvas with different Cookies.

	Making_2D_Cookies_Array();

	glutInit(&argc, argv); // initialize the graphics library...
	SetCanvasSize(width, height); // set the number of pixels...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(0, 80); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("Ahsan_Abbas's Cookie Crush"); // set the title of our game window

	// Load the textures from the file..;
	RegisterTextures();
	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.

	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutReshapeFunc(ReshapeFunction); // tell library which function to call for resizing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);

	//// This function tells the library to call our Timer function after 1000.0 milliseconds...
	glutTimerFunc(1000, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* */

