#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "OBJ.h"
#include "PolygonMesh.h"

static int win_width = 1024;
static int win_height = 720;

int old_t = 0;

float deltaTime;

OBJ* obj = NULL;

PolygonMesh *floorMesh = NULL;

bool showFloor = true;

bool task5 = false;

bool task6 = false;

bool recording = false;

bool rotating = false;

int picCounter = 0;

void render(void);

void drawText(void);

void reshape(int w, int h);

void keyboard(unsigned char key, int mx, int my);

void idle(void);

void init(void);

void screenshot();

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("Animation Assignment");

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	init();
	glutMainLoop();
	return 0;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -80.0);
	glTranslatef(0.0, 35.0, 0.0);
	glRotatef(20.0, 1.0, 0.0, 0.0);
	glRotatef(40.0, 0.0, 1.0, 0.0);

	float size = 15.0f;

	if (floorMesh)
	{
		// Update min max
		floorMesh->updateMinMax();

		// Set the colour of the polygon mesh
		glColor3f(0.8, 0.8, 0.8);
		if (showFloor)
		{
			if(rotating)
				floorMesh->update(0.005);
			floorMesh->render();
		}
	}

	if (obj)
	{
		obj->performTask6(task6);
		obj->performTask6(task6);

		if(floorMesh)
			obj->update(0.005, *floorMesh);
		else
			obj->update(0.005);
	}

	glColor3f(1.0, 1.0, 1.0);
	drawText();

	glutSwapBuffers();

	if (recording)
	{
		screenshot();
	}
}

void drawText()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, win_width, win_height, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	std::string message = "";
	if (task5)
	{
		message = "Task5 values applied";
	}
	else
	{
		message = "Task5 values NOT applied";
	}
	glRasterPos2i(8, 20);
	for (int i = 0; i < message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	if (task6)
	{
		message = "Wind applied";
	}
	else
	{
		message = "Wind NOT applied";
	}
	glRasterPos2i(8, 40);
	for (int i = 0; i < message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	message = "Reload object if values changed";
	glRasterPos2i(8, 60);
	for (int i = 0; i < message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	glColor3f(1.0, 0.0, 0.0);
	if (recording)
	{
		message = "Recording!!!!!";
	}
	else
	{
		message = "";
	}
	glRasterPos2i(8, 80);
	for (int i = 0; i < message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / h, 1, 500);

	win_width = w;
	win_height = h;
}

void keyboard(unsigned char key, int mx, int my) 
{
	if (key == '5')
	{
		task5 = !task5;
	}

	if (key == '6')
	{
		task6 = !task6;
	}

	if (key == 'b')
	{
		showFloor = !showFloor;
	}

	if (key == 'v')
	{
		recording = !recording;
		picCounter = 0;
	}

	if (key == 'r')
	{
		rotating = !rotating;
		if (floorMesh)
			floorMesh->setRotating(rotating);
	}

	// Select obj for the floor
	if (key == 'f')
	{
		OPENFILENAME ofn;
		// an another memory buffer to contain the file name
		wchar_t szFile[256];

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"Object File (*.obj)\0*.obj\0All (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = L"Select OBJ file";
		ofn.lpstrDefExt = L"obj";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		bool readSuccess = GetOpenFileName(&ofn);
		if (readSuccess)
		{
			char buffer[256];
			wcstombs_s(NULL, buffer, sizeof(buffer), szFile, sizeof(szFile) - 1);

			// Check if selecting files is working fine
			//std::cout <<buffer << "\n";

			floorMesh = new PolygonMesh();
			bool res = floorMesh->loadOBJ(buffer);
			if (!res)
			{
				floorMesh = NULL;
				std::cout << "Cannot read obj file" << "\n";
			}
		}
	}

	if (key == 'l')
	{
		OPENFILENAME ofn;
		// an another memory buffer to contain the file name
		wchar_t szFile[256];

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"Object File (*.obj)\0*.obj\0All (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = L"Select OBJ file";
		ofn.lpstrDefExt = L"obj";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		bool readSuccess = GetOpenFileName(&ofn);
		if (readSuccess)
		{
			char buffer[256];
			wcstombs_s(NULL, buffer, sizeof(buffer), szFile, sizeof(szFile)-1);

			// Check if selecting files is working fine
			//std::cout <<buffer << "\n";

			obj = new OBJ();
			bool res = obj->loadOBJ(buffer);
			obj->performTask5(task5);
			if (!res)
			{
				obj = NULL;
				std::cout << "Cannot read obj file" << "\n";
			}
		}
	}

	if (key == 's')
	{
		OPENFILENAME ofn;

		char szFileName[256] = "";

		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"Object File (*.obj)\0*.obj\0All (*.*)\0*.*\0";
		ofn.lpstrFile = (LPWSTR)szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = (LPCWSTR)L"obj";

		GetSaveFileName(&ofn);
		wprintf(L"the path is : %s\n", ofn.lpstrFile);

		char buffer[256];
		wcstombs(buffer, ofn.lpstrFile, sizeof(buffer));

		std::ofstream outfile(buffer);

		outfile << std::setprecision(6) << std::fixed;
		outfile << "# COMP5823M Animation and Simulation Assignment 2" << "\n";

		obj->exportOBJ(outfile);
		outfile.close();
	}

	glutPostRedisplay();
}

void idle()
{
	int t;
	t = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (t - old_t) / 1000.0;
	old_t = t;

	glutPostRedisplay();
}

void init()
{
	old_t = glutGet(GLUT_ELAPSED_TIME);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void screenshot()
{
	// This is probably the easiest way to take a screen shot
	BYTE* pixels = new BYTE[3 * win_width * win_height];

	glReadPixels(0, 0, win_width, win_height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	std::string fileName = "test" + std::to_string(picCounter) + ".tga";
	std::string path = "C:/Users/Steven/source/repos/AnimationAssignment2/Output";

	std::string outputFileString = path + "/" + fileName;
	const char* outputFile = outputFileString.c_str();

	FILE* out = fopen(outputFile, "w");
	short  TGAhead[] = { 0, 2, 0, 0, 0, 0, win_width, win_height, 24 };
	fwrite(&TGAhead, sizeof(TGAhead), 1, out);
	fwrite(pixels, 3 * win_width * win_height, 1, out);
	fclose(out);
	picCounter++;
	// Clear memory
	delete pixels;
}