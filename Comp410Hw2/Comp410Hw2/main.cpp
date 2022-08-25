//
//  Display a rotating cube, revisited
//

#include "Angel.h"
;

typedef vec4  color4;
typedef vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

mat4 Model_view[9];

point4 Points[9][NumVertices] = {};
point4 Colors[9][NumVertices] = {};
point4 VerticesCube[9][8] = {};



GLfloat offsetBetweenCubes = 0.025;
GLfloat squareEdge = 0.8; // edge of one small cube
// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };



GLfloat  Initial[NumAxes] = { 30.0, -40.0, 0.0 }; // initial rotation angles

point4 distanceXToCentre[8] = {

	//front 4
	point4((-squareEdge / 2) - offsetBetweenCubes,0.0,0.0,0.0), point4(squareEdge / 2 + offsetBetweenCubes,0.0,0.0,0.0),
	point4(-squareEdge / 2 - offsetBetweenCubes,0.0,0.0,0.0), point4(squareEdge / 2 + offsetBetweenCubes,0.0,0.0,0.0),

	//back 4
	point4(-squareEdge / 2 - offsetBetweenCubes,0.0,0.0,0.0), point4(squareEdge / 2 + offsetBetweenCubes,0.0,0.0,0.0),
	point4(-squareEdge / 2 - offsetBetweenCubes,0.0,0.0,0.0), point4(squareEdge / 2 + offsetBetweenCubes,0.0,0.0,0.0),

};

point4 distanceYToCentre[8] = {

	//front 4
	point4(0.0,squareEdge / 2 + offsetBetweenCubes,0.0,0.0), point4(0.0,squareEdge / 2 + offsetBetweenCubes,0.0,0.0),
	point4(0.0,-squareEdge / 2 - offsetBetweenCubes,0.0,0.0), point4(0.0,-squareEdge / 2 - offsetBetweenCubes,0.0,0.0),

	//back 4
	point4(0.0,squareEdge / 2 + offsetBetweenCubes,0.0,0.0), point4(0.0,squareEdge / 2 + offsetBetweenCubes,0.0,0.0),
	point4(0.0,-squareEdge / 2 - offsetBetweenCubes,0.0,0.0), point4(0.0,-squareEdge / 2 - offsetBetweenCubes,0.0,0.0),
};

point4 distanceZToCentre[8] = {
	//front 4
point4(0.0,0.0,squareEdge / 2 + offsetBetweenCubes,0.0), point4(0.0,0.0,squareEdge / 2 + offsetBetweenCubes,0.0),
point4(0.0,0.0,squareEdge / 2 + offsetBetweenCubes,0.0), point4(0.0,0.0,squareEdge / 2 + offsetBetweenCubes,0.0),

//back 4
point4(0.0,0.0,-squareEdge / 2 - offsetBetweenCubes,0.0), point4(0.0,0.0,-squareEdge / 2 - offsetBetweenCubes,0.0),
point4(0.0,0.0,-squareEdge / 2 - offsetBetweenCubes,0.0), point4(0.0,0.0,-squareEdge / 2 - offsetBetweenCubes,0.0),

};


// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-squareEdge / 2, -squareEdge / 2,  squareEdge / 2, 1.0),
	point4(-squareEdge / 2,  squareEdge / 2,  squareEdge / 2, 1.0),
	point4(squareEdge / 2,  squareEdge / 2,  squareEdge / 2, 1.0),
	point4(squareEdge / 2, -squareEdge / 2,  squareEdge / 2, 1.0),
	point4(-squareEdge / 2, -squareEdge / 2, -squareEdge / 2, 1.0),
	point4(-squareEdge / 2,  squareEdge / 2, -squareEdge / 2, 1.0),
	point4(squareEdge / 2,  squareEdge / 2, -squareEdge / 2, 1.0),
	point4(squareEdge / 2, -squareEdge / 2, -squareEdge / 2, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};


// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

//which cube and faces has which colors

color4 CubeColor[9][6]{
	{
		//front left up
		color4(0.0, 1.0, 0.0, 1.0),  // green
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 0.5, 0.0, 1.0),  // orange
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(0.0, 0.0, 0.0, 1.0)   // black
	},
	{
		//front right up
		color4(0.0, 1.0, 0.0, 1.0),  // green
		color4(1.0, 0.0, 0.0, 1.0),  // red
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(0.0, 0.0, 0.0, 1.0)   // black
	},
	{
		//front left down
		color4(0.0, 1.0, 0.0, 1.0),  // green
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 0.5, 0.0, 1.0),  // orange
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 0.0, 1.0)   // yellow
	},
	{
		//front right down
		color4(0.0, 1.0, 0.0, 1.0),  // green
		color4(1.0, 0.0, 0.0, 1.0),  // red
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 0.0, 1.0)   // yellow
	},
	{
		//back left up
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 1.0, 1.0),  // blue
		color4(1.0, 0.5, 0.0, 1.0),  // orange
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(0.0, 0.0, 0.0, 1.0)   // black
	},
	{
		//back right up
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 0.0, 0.0, 1.0),  // red
		color4(0.0, 0.0, 1.0, 1.0),  // blue
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(0.0, 0.0, 0.0, 1.0)   // black
	},
	{
		//back left down
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 1.0, 1.0),  // blue
		color4(1.0, 0.5, 0.0, 1.0),  // orange
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 0.0, 1.0)   // yellow

	},
	{
		//back right down
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 0.0, 0.0, 1.0),  // red
		color4(0.0, 0.0, 1.0, 1.0),  // blue
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(0.0, 0.0, 0.0, 1.0),  // black
		color4(1.0, 1.0, 0.0, 1.0)  // yellow
	},
	{
		//extra blank
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(1.0, 1.0, 1.0, 1.0),  // white
		color4(1.0, 1.0, 1.0, 1.0)  // white
	}

};



void fillVertexArrays() {

	for (int i = 0; i < 9; i++) {

		for (int j = 0; j < 8; j++) {

			VerticesCube[i][j] = vertices[j] + distanceXToCentre[i] + distanceYToCentre[i] + distanceZToCentre[i];

		}
	}
	// last cube is going to be the enlarged version of the middle center cube
	for (int j = 0; j < 8; j++) {
		VerticesCube[8][j] = VerticesCube[4][j];
	}
}

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad(int a, int b, int c, int d, int cubeIndex, int face)
{
	// Initialize colors

	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][a]; Index++;
	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][b]; Index++;
	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][c]; Index++;
	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][a]; Index++;
	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][c]; Index++;
	Colors[cubeIndex][Index] = CubeColor[cubeIndex][face]; Points[cubeIndex][Index] = VerticesCube[cubeIndex][d]; Index++;
}


//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube(int cubeIndex)
{
	quad(1, 0, 3, 2, cubeIndex, 0);
	quad(2, 3, 7, 6, cubeIndex, 1);
	quad(4, 5, 6, 7, cubeIndex, 2);
	quad(5, 4, 0, 1, cubeIndex, 3);
	quad(6, 5, 1, 2, cubeIndex, 4);
	quad(3, 0, 4, 7, cubeIndex, 5);
	Index = 0;
}

//----------------------------------------------------------------------------




// OpenGL initialization
GLuint vaoArray[9];
GLuint buffer;
GLuint vPosition;
GLuint vColor;
const vec3 displacement(0.0, 0.0, 0.0);

void
init()
{

	fillVertexArrays();
	for (int i = 0; i < 8; i++) {
		colorcube(i);
	}

	// Create a vertex array object
	glGenVertexArrays(9, vaoArray);
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	for (int i = 0; i < 9; i++) {

		glBindVertexArray(vaoArray[i]);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * 36 + sizeof(Colors[i]), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * 36, Points[i]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * 36, sizeof(Colors[i]), Colors[i]);

		vPosition = glGetAttribLocation(program, "vPosition");

		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		vColor = glGetAttribLocation(program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * 36));

		if (i == 8) {
			Model_view[i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]);
		}
		else {
			Model_view[i] = RotateY(Initial[Yaxis]) * RotateX(Initial[Xaxis]) * Model_view[i];

		}

	}



	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	// Set current program object
	glUseProgram(program);

	// Set projection matrix
	mat4  projection;
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);

	// Set state variable "clear color" to clear buffer with.
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

//----------------------------------------------------------------------------
int asd = 0;
int bsd = 0;
void
display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//  Generate tha model-view matrix
	//const vec3 displacement( 0.0, 0.0, -5.0 ); //use this in the case of perspective projection
	for (int i = 0; i < 9; i++) {
		glBindVertexArray(vaoArray[i]);



		glUniformMatrix4fv(ModelView, 1, GL_TRUE, Model_view[i]);

		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	mat4  projection;
	projection = Ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0); // Ortho(): user-defined function in mat.h
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	glutSwapBuffers();


}

//---------------------------------------------------------------------
//
// reshape
//

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	// Set projection matrix
	mat4  projection;
	if (w <= h)
		projection = Ortho(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
			1.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
	else  projection = Ortho(-1.0* (GLfloat)w / (GLfloat)h, 1.0 *
		(GLfloat)w / (GLfloat)h, -1.0, 1.0, -1.0, 1.0);


	/* if (w <= h)
		 projection = Perspective( 45.0, (GLfloat) w / (GLfloat) h, 0.5, 6.0 );
	 else  projection = Perspective( 45.0, (GLfloat) h / (GLfloat) w, 0.5, 6.0 );*/


	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}


//----------------------------------------------------------------------------

void
idle(void)
{
	//Theta[Axis] += 2.0;

	if (Theta[Axis] > 360.0) {
		//Theta[Axis] -= 360.0;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------
int frontLeftUp = 0;
int frontRightUp = 1;
int frontLeftDown = 2;
int frontRightDown = 3;
int backLeftUp = 4;
int backRightUp = 5;
int backLeftDown = 6;
int backRightDown = 7;
int tempFace = -1;


int *frontFace[4] = { &frontLeftUp,&frontRightUp,&frontLeftDown,&frontRightDown };
int *backFace[4] = { &backLeftUp,&backRightUp,&backLeftDown,&backRightDown };
int *upFace[4] = { &backLeftUp,&backRightUp,&frontLeftUp,&frontRightUp };
int *downFace[4] = { &backLeftDown,&backRightDown,&frontLeftDown,&frontRightDown };
int *leftFace[4] = { &backLeftUp,&frontLeftUp,&backLeftDown,&frontLeftDown };
int *rightFace[4] = { &frontRightUp,&backRightUp,&frontRightDown,&backRightDown };
int *inputFace[4] = {};


void
rotate(int p) {
	int speed = 3;
	if (p == 1) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateX(-speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}
	else if (p == 2) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateY(-speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}
	else if (p == 3) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateZ(-speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}

	if (p == 4) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateX(speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}
	else if (p == 5) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateY(speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}
	else if (p == 6) {
		for (int i = 0; i < 4; i++) {
			Model_view[*inputFace[i]] = RotateY(Initial[Yaxis]) *RotateX(Initial[Xaxis]) *  RotateZ(speed) * RotateX(-Initial[Xaxis])  * RotateY(-Initial[Yaxis]) * Model_view[*inputFace[i]];
		}
	}
}
int i = 0;
void timer(int p)
{
	rotate(p);
	glutPostRedisplay();
	if (i >= 29) {
		i = 0;
		return;
	}
	if (i < 29) {
		glutTimerFunc(1, timer, p);
		i++;
	}

}
//----------------------------------------------------------------------------
int keyPressed = 0;
void
keyboard(unsigned char key, int x, int y)
{
	if (key == 27) {
		exit(0);
	}
	else if (key == 'q' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = frontFace[i];
		}

		glutTimerFunc(1, timer, 3);


		tempFace = frontLeftUp;
		frontLeftUp = frontLeftDown;
		frontLeftDown = frontRightDown;
		frontRightDown = frontRightUp;
		frontRightUp = tempFace;
		keyPressed = 0;



		keyPressed = 0;
	}
	else if (key == 'Q' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = frontFace[i];
		}

		glutTimerFunc(1, timer, 6);

		tempFace = frontLeftUp;
		frontLeftUp = frontRightUp;
		frontRightUp = frontRightDown;
		frontRightDown = frontLeftDown;
		frontLeftDown = tempFace;
		keyPressed = 0;



	}
	else if (key == 'w' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = upFace[i];
		}

		glutTimerFunc(1, timer, 2);

		tempFace = backLeftUp;
		backLeftUp = frontLeftUp;
		frontLeftUp = frontRightUp;
		frontRightUp = backRightUp;
		backRightUp = tempFace;
		keyPressed = 0;


	}
	else if (key == 'W' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = upFace[i];
		}

		glutTimerFunc(1, timer, 5);

		tempFace = backLeftUp;
		backLeftUp = backRightUp;
		backRightUp = frontRightUp;
		frontRightUp = frontLeftUp;
		frontLeftUp = tempFace;
		keyPressed = 0;
	}
	else if (key == 'e' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = backFace[i];
		}

		glutTimerFunc(1, timer, 6);

		tempFace = backRightUp;
		backRightUp = backRightDown;
		backRightDown = backLeftDown;
		backLeftDown = backLeftUp;
		backLeftUp = tempFace;
		keyPressed = 0;
	}
	else if (key == 'E' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = backFace[i];
		}

		glutTimerFunc(1, timer, 3);

		tempFace = backRightUp;
		backRightUp = backLeftUp;
		backLeftUp = backLeftDown;
		backLeftDown = backRightDown;
		backRightDown = tempFace;
		keyPressed = 0;
	}
	else if (key == 'a' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = leftFace[i];
		}

		glutTimerFunc(1, timer, 4);

		tempFace = backLeftUp;
		backLeftUp = backLeftDown;
		backLeftDown = frontLeftDown;
		frontLeftDown = frontLeftUp;
		frontLeftUp = tempFace;


		keyPressed = 0;
	}
	else if (key == 'A' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = leftFace[i];
		}

		glutTimerFunc(1, timer, 1);

		tempFace = backLeftUp;
		backLeftUp = frontLeftUp;
		frontLeftUp = frontLeftDown;
		frontLeftDown = backLeftDown;
		backLeftDown = tempFace;
		keyPressed = 0;
	}
	else if (key == 's' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = downFace[i];
		}

		glutTimerFunc(1, timer, 5);

		tempFace = frontLeftDown;
		frontLeftDown = backLeftDown;
		backLeftDown = backRightDown;
		backRightDown = frontRightDown;
		frontRightDown = tempFace;
		keyPressed = 0;
	}
	else if (key == 'S' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = downFace[i];
		}

		glutTimerFunc(1, timer, 2);

		tempFace = frontLeftDown;
		frontLeftDown = frontRightDown;
		frontRightDown = backRightDown;
		backRightDown = backLeftDown;
		backLeftDown = tempFace;
		keyPressed = 0;
	}
	else if (key == 'd' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = rightFace[i];
		}

		glutTimerFunc(1, timer, 1);

		tempFace = frontRightUp;
		frontRightUp = frontRightDown;
		frontRightDown = backRightDown;
		backRightDown = backRightUp;
		backRightUp = tempFace;
		keyPressed = 0;
	}
	else if (key == 'D' && keyPressed == 0) {
		keyPressed = 1;
		for (int i = 0; i < 4; i++) {
			inputFace[i] = rightFace[i];
		}

		glutTimerFunc(1, timer, 4);

		tempFace = frontRightUp;
		frontRightUp = backRightUp;
		backRightUp = backRightDown;
		backRightDown = frontRightDown;
		frontRightDown = tempFace;
		keyPressed = 0;
	}
	else if (key == 'i') {
		for (int i = 0; i < 8; i++) {
			printf("X: %f\n", Initial[Xaxis]);
			printf("Y: %f\n", Initial[Yaxis]);
			Model_view[i] = RotateY(-Initial[Yaxis])* RotateX(-Initial[Xaxis]) *Model_view[i];

		}
	}
	else if (key == 'h' || key == 'H') {
		printf("---------------------------------HELP----------------------------------------\n");
		printf("| WARNING, THIS GAME IS CASE SENSITIVE                                      |\n");
		printf("| If you press a LOWER case button, the face will turn CLOCKWISE            |\n");
		printf("| If you press an UPPER case button, the face will turn COUNTER CLOCKWISE   |\n");
		printf("| w: upper face                                                             |\n");
		printf("| s: lower face                                                             |\n");
		printf("| a: left face                                                              |\n");
		printf("| d: right face                                                             |\n");
		printf("| q: front face                                                             |\n");
		printf("| e: back face                                                              |\n");
		printf("| Press h for help                                                          |\n");
		printf("| Press Esc to quit                                                         |\n");
		printf("| Drag the mouse to move the cube (horizontally, to be able to keep track   |\n");
		printf("| of front and back faces                                                   |\n");
		printf("| wait for turn to finish for better experience                             |\n");
		printf("-----------------------------------------------------------------------------\n");
	}


}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

int old_x = 0;
int old_y = 0;
int valid = 0;


void mouse(int button, int state, int x, int y)
{
	old_x = x;
	old_y = y;
	valid = state == GLUT_DOWN;
}

void mouseDrag(int x, int y) {
	if (valid) {
		int dx = old_x - x;
		int dy = old_y - y;

		//Initial[Xaxis] -= dy;
		Initial[Yaxis] -= dx;

		if (Initial[Xaxis] > 360.0) {
			Initial[Xaxis] -= 360.0;
		}if (Initial[Xaxis] < -360.0) {
			Initial[Xaxis] += 360.0;
		}
		if (Initial[Yaxis] > 360.0) {
			Initial[Yaxis] -= 360.0;
		}
		if (Initial[Yaxis] < -360.0) {
			Initial[Yaxis] += 360.0;
		}
		for (int i = 0; i < 8; i++)
			Model_view[i] = RotateY(-dx) * Model_view[i];
	}
	glutPostRedisplay();

	old_x = x;
	old_y = y;
}



//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Color Cube");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display); // set display callback function
	glutReshapeFunc(reshape);
	//glutIdleFunc( idle );
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);
	glutKeyboardFunc(keyboard);

	glutTimerFunc(5, timer, 0);

	glutMainLoop();
	return 0;
}
