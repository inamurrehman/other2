
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <gl/glut.h>
#include<conio.h>
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<math.h>

using namespace cv;
using namespace std;

//Variables for OpenGl
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 15.0f;
float ly = 0.0f, y = 1.0f;
float robox = x + lx;
int mrv = 5;//mill rotation varibale

// all variables initialized to 1.0, meaning
// the triangle will initially be white
float red = 1.0f, blue = 1.0f, green = 1.0f;

// angle for rotating triangle
float angle = 0.0f;
//End Variable for OpenGl

// variables for image processing ...........
//For Green Color .......
//Of Green Below
int GreeniLowH = 63;// 59;// 42;//72;//170//
int GreeniHighH = 98;// 101;// 74;// 90;//179//
int GreeniLowS = 98;// 147;// 117;// 115;//150,,
int GreeniHighS = 255;//255
int GreeniLowV = 37;// 79;// 114;//60,
int GreeniHighV = 255;//255 
//End For Green Color....
//For Blue Color .......
//Of Blue Below
int BlueiLowH = 65;// 30;// 65;// 60;//50;//42;//59;//110;//  170// night 65
int BlueiHighH = 80;// 120;//80;// 90;//75;//101;//130;//   90;//179// night 80
int BlueiLowS = 110;// 147;// 117;// 115;//150,,
int BlueiHighS = 255;//255
int BlueiLowV = 145;// 170;//50;//  114;//60,
int BlueiHighV = 255;// 255; //155;// 
//End For Blue Color....
Mat image;
static bool NavigateEnable = 0;
bool OnceGate = 1;
bool GateFromCentralArea = 0;
bool Gate4WheelRotate = 0;
bool Detected = 0;
bool DetectedB = 0;

int posX, posY;
int bposX = 0, bposY = 0;
int PreposX, PreposY;
long int Arr[5];
int cnt = 0;
int centerX;
int centerY;
int WheelRotateAng = 0;

Mat ForGreenImg;
Mat ForBlueImg;
Mat GreenimgThresholded;
Mat BlueimgThresholded;
int Aavg;
//End for variable in image processing........

cv::VideoCapture cap(0);
//....bool data
void reset()
{
	lx = 0.0f, lz = -1.0f;
	// XZ position of the camera
	x = 0.0f, z = 15.0f;
	ly = 0.0f, y = 1.0f;
	robox = x + lx;
	mrv = 5;//mill rotation varibale

	// all variables initialized to 1.0, meaning
	// the triangle will initially be white
	red = 1.0f, blue = 1.0f, green = 1.0f;

	// angle for rotating triangle
	angle = 0.0f;
	//End Variable for OpenGl

	Mat image;
	NavigateEnable = 0;
	OnceGate = 1;
	GateFromCentralArea = 0;
	Gate4WheelRotate = 0;
	posX = 0, posY = 0;
	PreposX = 0, PreposY = 0;
	//Arr[5];
	cnt = 0;
	centerX = 0;
	centerY = 0;
	WheelRotateAng = 0;
	//Mat ForGreenImg;
	//Mat GreenimgThresholded;
	Aavg = 0;
	//dArea = 0;
}
void detectImg(cv::Mat scene)
{
	float fraction = 0.1f;// for camera ratio
	Mat src;
	src = scene;
	//imshow()
	imshow("poll", scene);

	ForGreenImg = scene;
	cvtColor(ForGreenImg, ForGreenImg, COLOR_BGR2HSV);
	blur(ForGreenImg, ForGreenImg, Size(10, 10));
	cv::inRange(ForGreenImg, Scalar(GreeniLowH, GreeniLowS, GreeniLowV), Scalar(GreeniHighH, GreeniHighS, GreeniHighV), GreenimgThresholded);//Green (50 - rng, 70, 70), cv::Scalar(70 + rng, 255, 255)
	// N Lab (60 - rng, 100, 100), cv::Scalar(80 + rng, 255, 255)
	erode(GreenimgThresholded, GreenimgThresholded, getStructuringElement(2, Size(10, 10)));
	dilate(GreenimgThresholded, GreenimgThresholded, getStructuringElement(2, Size(10, 10)));

	imshow("For Green Thresholded", GreenimgThresholded);




	/////////////////////////////////////////////
	ForBlueImg = scene;
	cvtColor(ForBlueImg, ForBlueImg, COLOR_BGR2HSV);
	blur(ForBlueImg, ForBlueImg, Size(10, 10));
	cv::inRange(ForBlueImg, Scalar(BlueiLowH, BlueiLowS, BlueiLowV), Scalar(BlueiHighH, BlueiHighS, BlueiHighV), BlueimgThresholded);//Green (50 - rng, 70, 70), cv::Scalar(70 + rng, 255, 255)
	// N Lab (60 - rng, 100, 100), cv::Scalar(80 + rng, 255, 255)
	erode(BlueimgThresholded, BlueimgThresholded, getStructuringElement(2, Size(10, 10)));
	dilate(BlueimgThresholded, BlueimgThresholded, getStructuringElement(2, Size(10, 10)));

	imshow("For  Blue Thresholded", BlueimgThresholded);
	Moments oMoments1 = moments(BlueimgThresholded);

	double dM011 = oMoments1.m01;
	double dM101 = oMoments1.m10;
	double dArea1 = oMoments1.m00;
	if (dArea1 > 625 && dM101 > 8 && dM011 > 8)//of Index green
	{
		//IndexVisible = 1;
		bposX = dM101 / dArea1;
		bposY = dM011 / dArea1;
		if (dArea1 > Aavg - 30000)
			DetectedB = 1;
		//cout << "\nx of blue=" << bposX << " and y of blue=" << bposY<< "\n\n\n ";
		//Beep(900, 500);
	}
	////////////////////////////////////////////


	Moments oMoments = moments(GreenimgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	if (dArea > 625 && dM10 > 8 && dM01 > 8)//of Index green
	{
		//IndexVisible = 1;
		posX = dM10 / dArea;
		posY = dM01 / dArea;
		if (dArea > Aavg - 30000)
			Detected = 1;
		//cout << "\nx of green=" << posX << " and y of green= " << posY;



		if (posX - 20 > 2 && posY - 20>2)
		{
			if (cnt <= 5)
			{
				Arr[cnt] = dArea;
				cnt = cnt + 1;
				centerX = posX;
				centerY = posY;
				//Beep(900, 500);
			}
			else if (OnceGate == 1)
			{
				Aavg = (Arr[0] + Arr[1] + Arr[2] + Arr[3] + Arr[4]) / 5;
				OnceGate = 0;
				GateFromCentralArea = 1;
			}
			//////////////////////////////////////////////////////////// 
			//if (Detected == 1 && DetectedB == 1 && abs(posX - bposX)>=60  ) ////      Z IN
			//{
			//	//Beep(900, 500);
			//	//Beep(900, 500);
			//	x -= lx * 2 * fraction;
			//	z -= lz * 2 * fraction;
			//	Gate4WheelRotate = 1;
			//	cout << "\n" << posX << "\t" << bposX << "\t" << abs(posX - bposX) << "\n";
			//	
			//}
			if (Detected == 1 && DetectedB == 1 && abs(bposX - posX) <= 30) ////      Z OUT
			{
				//Beep(900, 500);
				x += lx * 2 * fraction;
				z += lz * 2 * fraction;
				Gate4WheelRotate = 1;
				cout << "\n" << bposX << "\n";

			}


			//			if (Detected == 1 && DetectedB == 1 && posY <120 && bposY >250 && abs(posX - bposX)>100) ////      RIGHT
			//			{
			//				angle += 0.01f;
			//				lx = sin(angle);
			//				lz = -cos(angle);
			//
			//			}
			//			if (Detected == 1 && DetectedB == 1 && bposY <150 && posY >250 && abs(posX - bposX)>100) //////////// LIFT
			//			{  
			//
			//		angle -= 0.01f;
			//		lx = sin(angle);
			//		lz = -cos(angle);
			//
			//
			//			}
			////			//////////////////////////////////////////////////////////// 
			//			if (Detected == 1 && DetectedB == 1 && posY <150 && bposY<150 && abs(posX - bposX)>100) //////////  UP
			//			{
			//				y = y + 0.1;
			//				Gate4WheelRotate = 0;
			//
			//				//Beep(900, 500);
			//				/*x += lx * 2 * fraction;
			//				z += lz * 2 * fraction;
			//				Gate4WheelRotate = 1;*/
			//				//cout << "YOU ENTERED \n";
			//				//cout << "\n   /* y of green=" << posY << " \t " << bposY;
			//			//	cout << "\n x of green=" << posX << " and x of blue= " << bposX;
			//			}
			////
			////
			////			//////////////////////////////////////////////////////////////////
			////
			////			//if ((Detected == 1) && (DetectedB == 1) && dArea > Aavg + 170000 && GateFromCentralArea == 1 && !(posY + 100<centerY) && !(posY - 100>centerY) && !(posX + 150<centerX) && !(posX - 150>centerX))
			////			//{
			////
			////			//	x += lx * 2 * fraction;
			////			//	z += lz * 2 * fraction;
			////			//	Gate4WheelRotate = 1;
			////			//	//Beep(300, 200);
			////			//}
			////////////////////////////////////////    EDITED
			////		
			//			if (Detected == 1 && DetectedB == 1 && posY > 250 && bposY > 250 && abs(posX - bposX) > 100)////////////// DOWN
			//			{
			//
			//				if (!(y <= 1))
			//				{
			//
			//					y = y - 0.1;
			//					//cout << "\n....y=" << y;
			//					Gate4WheelRotate = 0;
			//
			//				}
			//			}
			//			//if (Detected == 1 && DetectedB == 1 && posY >200 && bposY>200 /*&& posX<100 && bposX>300*/)
			//{
			//Beep(500, 200);
			//	}
			//}
			//if ((Detected == 1) && dArea < Aavg - 40000 && GateFromCentralArea == 1 && !(posY + 100<centerY) && !(posY - 100>centerY) && !(posX + 150<centerX) && !(posX - 150>centerX))
			//{
			//	x -= lx * 2 * fraction;
			//	z -= lz * 2 * fraction;oooooooooooooooooo
			//	Gate4WheelRotate = 1;
			//}//end if(dAvrea<avg) for going away frm screen
			///////////////////////////// 
			//if ((Detected == 1) && posY + 100<centerY && GateFromCentralArea == 1 && !(dArea < Aavg - 40000) && !(dArea > Aavg + 170000) && !(posX + 150<centerX))
			//{
			//y = y + 0.1;
			//Gate4WheelRotate = 0;
			////////////////centerY = y;
			//}
			//if ((Detected == 1) && posY - 120>centerY && GateFromCentralArea == 1 && !(dArea < Aavg - 40000) && !(dArea > Aavg + 170000) && !(posX + 80<centerX))
			//{
			//if (!(y <= 1))
			//{
			//	y = y - 0.1;
			//	//cout << "\n....y=" << y;
			//	Gate4WheelRotate = 0;
			//	//Beep(500, 200);
			//}
			//}

			if ((Detected == 1) && posX + 150<centerX && GateFromCentralArea == 1 && !(dArea < Aavg - 50000) && !(dArea > Aavg + 170000) && !(posY + 100<centerY))
			{

				/*angle += 0.01f;
				lx = sin(angle);
				lz = -cos(angle);*/
				////////robox = robox + 0.02;

			}
			if ((Detected == 1) && posX - 150>centerX && GateFromCentralArea == 1 && !(dArea < Aavg - 50000) && !(dArea > Aavg + 170000) && !(posY + 100<centerY))
			{

				//*angle -= 0.01f;
				//lx = sin(angle);
				//lz = -cos(angle);*/
				/////////////robox = robox - 0.02;
				/////////////robox = robox + 0.8;

			}

		}// End if (posX...)
		else
			Gate4WheelRotate = 0;

	}//End if(Area...)
	else
	{
		Detected = 0;
		NavigateEnable = 0;
		Gate4WheelRotate = 0;
		posX = 0;
		posY = 0;
		dArea = 0;
		DetectedB = 0;
		bposX = 0;
		bposY = 0;

	}



}
void wmill()
{
	glPushMatrix();

	glTranslatef(4.5, 0, -25);
	glColor3f(0.51, 0.5, 0.1);
	glBegin(GL_TRIANGLES);//
	//glTexCoord2f(0.0, 0.0);
	glVertex3f(2, 0, 5);
	//glTexCoord2f(1.0, 0);
	glVertex3f(6, 0, 5);
	//glTexCoord2f(1.0, 1);
	glVertex3f(4, 8, 5);
	glEnd();
	glTranslatef(5.1, 9.4, 0.4);
	glColor3f(1, 0.4, 0.4);
	glutSolidSphere(0.9, 18, 9.6);//for head
	glPushMatrix();//moving the wing of mill
	//glColor3f(0, 1, 0);
	glRotatef(mrv, 0, 0, 1);//for movingn the fan of mill
	glScalef(3, 0.3, 0);
	glutSolidCube(2);
	mrv = mrv + 15;
	glPopMatrix();
	glPopMatrix();
	//glPopMatrix();
}//End of Wmill

void plant()
{//glPushMatrix();
	glLineWidth(4);
	glPushMatrix();
	glTranslatef(8, 1, -10);
	glColor3f(0.2, 0.5, 0.2);
	glScalef(0.5, 0.4, 0.5);

	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	glVertex3f(-2, 1, 0);
	glVertex3f(-1, 2, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(1, 2, 0);
	glVertex3f(2, 1, 0);
	// glVertex3f(0,-1.5,);
	glEnd();
	glPopMatrix();
	glTranslatef(0, 1.8, 0);
	glScalef(0.6, 0.6, 0.6);
	glBegin(GL_LINE_STRIP);
	glVertex3f(-2, 1, 0);
	glVertex3f(-1, 2, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(1, 2, 0);
	glVertex3f(2, 1, 0);
	// glVertex3f(0,-1.5,);
	glEnd();
	glTranslatef(0, 2.5, 0);
	glScalef(0.6, 0.6, 0.6);
	glBegin(GL_LINE_STRIP);
	glVertex3f(-2, 1, 0);
	glVertex3f(-1, 2, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(1, 2, 0);
	glVertex3f(2, 1, 0);
	// glVertex3f(0,-1.5,);
	glEnd();
	//glTranslatef(0,1.8,0);
	//glScalef(0.6,0.6,0.6);
	glColor3f(1, 1, 0);

	glBegin(GL_LINES);
	glVertex3f(0, 4, 0);
	glVertex3f(0, -15, 0);
	// glVertex3f(0,-1.5,);
	glEnd();
	//glPopMatrix();
	glPopMatrix();
}

void changeSize(int w, int h)
{


	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();


	glViewport(0, 0, w, h);


	gluPerspective(45.0f, ratio, 0.1f, 100.0f);


	glMatrixMode(GL_MODELVIEW);
}

void drawTree()
{
	glPushMatrix();
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(0, 0.8, 0);

	//glTranslatef(0.0f, 0.75f, 0.0f);
	glTranslatef(0.0f, 3.6f, 0.0f);
	//glutSolidSphere(0.75f, 20, 20);
	glBegin(GL_TRIANGLES);
	glVertex3f(-4, 2, 0);
	glVertex3f(-5, -2, 0);
	glVertex3f(-3, -2, 0);
	glEnd();
	glPushMatrix();
	glTranslatef(-3.9, -2.4, 0.1);
	glColor3f(0.6, 0, 0);
	glScalef(0.4, 1.0, 0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

}
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.50f, 0.50f, 0.90f, 0.0f); // clear to black

	glLoadIdentity();

	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);


	glColor3f(0.7f, 0.9f, 0.4f);                 // Ground
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();



	glColor3f(1, 0, 0.2);
	glPushMatrix();
	//glRotatef(2*angle, 0, 1, 0);
	glTranslatef(x + lx, y + ly, (z + lz) + 0.1);
	//glRotatef(angle+10, 0, 1, 0);
	glScalef(0.15, 0.1, 0.1);
	glutSolidCube(0.2);                             // Robot
	glColor3f(0.1, 0.1, 0.2);
	glTranslatef(0.122, -0.08, 0);
	//glRotatef(90, 0, 1, 0);

	if (Gate4WheelRotate == 1)
	{
		WheelRotateAng = WheelRotateAng + 40;
		glRotatef(WheelRotateAng, 1, 0, 0);
	}
	glScalef(0.2, 0.7, 0.1);
	glutSolidCube(0.2);                             // Robot wheel right
	glTranslatef(-1.22, 0.0, 0);
	glutSolidCube(0.2);                             //Robot wheel lift
	//glutWireSphere(0.1, 8, 2);
	//glBegin(GL_LINES);//
	//glVertex3f(0.1, -0.1, 0);
	//glVertex3f(0.4, -0.18, 0);
	//glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8.5, 0, -6);
	//wmill();
	//glPopMatrix();
	glPushMatrix();
	for (int r = 2; r <= 4; r++)
	{

		glTranslatef(r, 0, 0);
		plant();
		glPushMatrix();
		glTranslatef(-2, 0, r + 10);//Frot
		plant();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-5, 0, -(r + 10));
		plant();
		glPopMatrix();
		glTranslatef(-10 + r, 0, -(r + 20));
		plant();
		glPopMatrix();

	}//End outer loop
	glPopMatrix();
	//glPopMatrix();

////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

	for (int i = -2; i < 1; i++)
	{
		for (int j = -2; j < 1; j++)
		{
			glPushMatrix();
			glTranslatef(i*10.0, 0, j * 10.0);
			drawTree();
			glPopMatrix();
		}//End inner loop
	}

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(-1, 2.5, 10);
	glBegin(GL_QUADS);
	glVertex3f(-3.0f, 0.0f, -7.0f);
	glVertex3f(-3.0f, 0.0f, 7.0f);
	glVertex3f(9.0f, 0.0f, 7.0f);
	glVertex3f(9.0f, 0.0f, -7.0f);
	glEnd();
	glTranslatef(-2, -2, 5.1);
	glColor3f(0.1, 0.1, 0.1);
	glScalef(0.1, 2, 0.1);
	glutSolidCube(2);

	glTranslatef(87, 0, 0.0);
	glutSolidCube(2);
	glTranslatef(0, 0, -70.0);
	glutSolidCube(2);
	glTranslatef(-80, 0, 0.0);
	glutSolidCube(2);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5f, 0.2f, 0.2f);
	glTranslatef(-1, 2.5, 10);
	glBegin(GL_QUADS);
	glVertex3f(0.0f, -2.4f, -55.0f);
	glVertex3f(0.0f, -2.4f, 25.0f);
	glVertex3f(5.0f, -2.4f, 25.0f);
	glVertex3f(5.0f, -2.4f, -55.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(5, 0.1, 25);
	//glRotatef(65, 0, 1, 0);
	glScalef(0.0037, 0.0022, 0.0022);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '-');
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '>');
	glPopMatrix();
	glPushMatrix();
	glTranslatef(9, 0.1, 20);
	glScalef(0.0037, 0.0022, 0.0022);
	//glRotatef(45, 1, 0, 0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '^');
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(-2.5, 0.3, 25);
	//glRotatef(65, 0, 1, 0);
	glScalef(0.0037, 0.0022, 0.0022);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '<');
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '-');
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-9, 0.1, 5);
	glScalef(0.0037, 0.0022, 0.0022);
	//glRotatef(45, 1, 0, 0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '^');
	glPopMatrix();

	if (Detected == 1)
	{
		glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(5, 1, 25);
		//glRotatef(65, 0, 1, 0);
		glScalef(0.0037, 0.0022, 0.0022);

		glPopMatrix();
	}
	glutSwapBuffers();

	glutPostRedisplay();
	Gate4WheelRotate = 0;
}

void idle()
{

	cap.retrieve(image);
	bool bSuccess = cap.read(image);
	detectImg(image);


}
void keyboard(unsigned char key, int xx, int yy)
{
	float fraction = 0.1f;// for camera ratio
	if (key == 'r' || key == ' ')
	{
		//x += lx * 2 * fraction;//for going inside screen
		//z += lz * 2 * fraction;
		reset();
		Beep(500, 200);
	}
	if (key == 'n')
	{
		x -= lx * fraction;//for going inside screen
		z -= lz * fraction;
		//reset();
		//Beep(500, 900);
	}
}
int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);		// Setup display mode to double buffer and RGB color
	//glutInitWindowSize(640, 480);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(1400, 900);						// Set the screen size
	glutCreateWindow("Opengl Window");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	//glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glEnable(GL_DEPTH_TEST);
	// start GUI loop
	glutMainLoop();
	return 0;

}
//99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999