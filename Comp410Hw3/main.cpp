//
//  Display a rotating cube, revisited
// Yiðithan Þanlýk 64117 HW3 COMP410

#include "Angel.h"
//sphere numbers
const int NumObjects = 4;
const int NumTimesToSubdivide = 4;
const int NumTrianglesSphere = 4096 *4;	// (4 faces)^(NumTimesToSubdivide + 1) 4096 
//vertice array
const int NumVertices[NumObjects] = { 36, 3 * NumTrianglesSphere, 4922 };
//triangle numbers
int NumTrianglesBunny=9840;
int NumVerticesBunny=4922;

typedef vec4  color4;
typedef vec4  point4;

// timing related variables
GLfloat currTime = 0;
const GLfloat deltaTime = .033;

// physics //
	// initial values for simulation
const vec3 ini_vel = vec3(0.8, 0.0, 0.0);
;
vec3 vel = ini_vel;
const vec3 acc = vec3(0.0, -2, 0.0);
// velocity multiplier for each collision 
const GLfloat dampFactor = 0.9;

// bounds for side collision and resizing
typedef struct bounds {
	GLfloat xMin;
	GLfloat xMax;
	GLfloat yMin;
	GLfloat yMax;
} Bounds;
Bounds bounds = { -3, 3, -3, 3 };
// scale of the model 
const GLfloat defaultModelScale = 0.20;
// global variable to control scale for different models
GLfloat modelScale = defaultModelScale;
//const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)


point4 spherePoints[3 * NumTrianglesSphere];
vec3 sphereNormals[3 * NumTrianglesSphere];
vec2 sphereTexture[3 * NumTrianglesSphere];
//point4 trianglesBunny[9840];


// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.1, -0.1,  0.1, 0.1),
    point4( -0.1,  0.1,  0.1, 0.1 ),
    point4(  0.1,  0.1,  0.1, 0.1 ),
    point4(  0.1, -0.1,  0.1, 0.1 ),
    point4( -0.1, -0.1, -0.1, 0.1 ),
    point4( -0.1,  0.1, -0.1, 0.1 ),
    point4(  0.1,  0.1, -0.1, 0.1 ),
    point4(  0.1, -0.1, -0.1, 0.1 )
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 2.0 ),  // black
    color4( 1.0, 0.0, 0.0, 2.0 ),  // red
    color4( 1.0, 1.0, 0.0, 2.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 2.0 ),  // green
    color4( 0.0, 0.0, 1.0, 2.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 2.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 2.0 ),  // white
    color4( 0.0, 1.0, 1.0, 2.0 )   // cyan
};

//initial positions
float xinit = -3.0;
float yinit = 3.0;

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
//displacement vector
vec3 displacement(xinit, yinit, -10.0);
// Model-view and projection matrices uniform location
GLuint  ModelView, Projection, Color;
//the speed lost from bouncing on ground.
const float resist = 0.009;
//bottom of window
float limit = -3.0;
// Create a vertex array object
GLuint vao;
//keeps the current object as int
GLuint object=1;
//keeps the color object as int
GLuint color;
//GLuint programs[3];
bool phongFlag = false; //enable phong shading
GLuint PhongFlag; // phong shading flag uniform location
bool gouraudFlag = false; 
GLuint GouraudFlag;
bool modifiedFlag = true; 
GLuint ModifiedFlag;

bool textureFlag = false; //enable texture mapping
GLuint  TextureFlag; // texture flag uniform location

color4 ambient_product,diffuse_product,specular_product;
float  material_shininess;

color4 light_ambient(0.2, 0.2, 0.2, 1.0); // L_a
color4 light_diffuse(1.0, 1.0, 1.0, 1.0); // L_d
color4 light_specular(1.0, 1.0, 1.0, 1.0); // L_s

color4 material_ambient(1.0, 0.0, 1.0, 1.0); // k_a
color4 material_diffuse(1.0, 0.8, 0.0, 1.0); // k_d
color4 material_specular(1.0, 0.8, 0.0, 1.0); // k_s

point4 light_position(-0.7, 0.7, 0.4, 0.0);
bool movingLight = false;

GLuint AmbientProduct,DiffuseProduct,SpecularProduct;
GLuint LightPosition, Shininess;

GLubyte* basketball_texels;
GLubyte* earth_texels;
//GLubyte* image;

//GLubyte* images[]; //0-basketball 1-earth
GLuint textures[2];
//----------------------------------------------------------------------------
// quad genera+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++tes two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
vec2  calculateuv(const point4& a) {
	
	double u = acos(a.x)/(2 * M_PI);
	double v = acos(a.y / sin(2 * M_PI* u)) / (2 * M_PI);

	return vec2(u, v);
}
	
void
triangle(const point4& a, const point4& b, const point4& c)
{
	vec3  normal = normalize(cross(b - a, c - b));

	sphereNormals[Index] = normal;  spherePoints[Index] = a; sphereTexture[Index] = calculateuv(a); Index++;
	sphereNormals[Index] = normal;  spherePoints[Index] = b; sphereTexture[Index] = calculateuv(b); Index++;
	sphereNormals[Index] = normal;  spherePoints[Index] = c; sphereTexture[Index] = calculateuv(c);  Index++;
}


//----------------------------------------------------------------------
point4
unit(const point4& p)
{
	float len = p.x*p.x + p.y*p.y + p.z*p.z;
	point4 t;
	if (len > DivideByZeroTolerance) {
		t = p / sqrt(len);
		t.w = 1.0;
	}
	return t;
}

void
divide_triangle(const point4& a, const point4& b,
	const point4& c, int count)
{
	if (count > 0) {
		point4 v1 = unit(a + b);
		point4 v2 = unit(a + c);
		point4 v3 = unit(b + c);
		divide_triangle(a, v1, v2, count - 1);
		divide_triangle(c, v2, v3, count - 1);
		divide_triangle(b, v3, v1, count - 1);
		divide_triangle(v1, v3, v2, count - 1);
	}
	else {
		triangle(a, b, c);
	}
}

void
tetrahedron(int count)
{
	point4 v[4] = {
	vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 0.942809, -0.333333, 1.0),
	vec4(-0.816497, -0.471405, -0.333333, 1.0),
	vec4(0.816497, -0.471405, -0.333333, 1.0)
	};
	divide_triangle(v[0], v[1], v[2], count);
	divide_triangle(v[3], v[2], v[1], count);
	divide_triangle(v[0], v[3], v[1], count);
	divide_triangle(v[0], v[2], v[3], count);
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------------

GLubyte
* readPPMFile(char* fileName)
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		printf("Can't open the file !\n");
	}
	int k, n, m;
	char c;
	int i;
	int red, green, blue;
	char line[150];


	char ch[2];   // 2 char table
	int dummy = 0;


	// read the first 3 char of the file
	fread(ch, sizeof(char), 2, file);
	// Check the file is an PPM file
	if (ch[0] != 'P' || ch[1] != '3') {
		printf("Not a ppm file.");
		exit(1);
	}
	//remove unneccesary characters #
	fscanf(file, "%c", &c);

	while (c == '#') {
		fscanf(file, "%[^\n] ", ch);
		printf("%s\n", ch);
		fscanf(file, "%c", &c);
	}
	ungetc(c, file);
	fscanf(file, "%d %d %d", &n, &m, &k);
	
	int nm = n * m;

	GLubyte* image;
	image = (GLubyte*)malloc(3 * sizeof(GLubyte) * nm);

	for (i = nm; i > 0; i--) {

		fscanf(file, "%d %d %d", &red, &green, &blue);

		image[3 * nm - 3 * i] = red;
		image[3 * nm - 3 * i + 1] = green;
		image[3 * nm - 3 * i + 2] = blue;

	}
		fclose(file);
	return image;
}

// generate 12 triangles: 36 vertices and 36 colors

void
sphere() {
	tetrahedron(NumTimesToSubdivide);
}

void
fillTexels()
{
	char basketballFile[] = "basketball.ppm";
	char earthFile[] = "earth.ppm";
	basketball_texels = readPPMFile(basketballFile);
	earth_texels = readPPMFile(earthFile);
}
 

void
info(void)
{
	printf("Press q to exit\n");
	printf("Press i to reset the pose\n");
	printf("Press r to give random velocity\n");
	printf("Right click for main menu\n");
	printf("Press h to print this help menu\n");
}

// OpenGL initialization
void
init()
{	
	info();
	sphere();
	fillTexels();
	glGenVertexArrays(1, &vao);
	// Load shaders
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePoints)));

	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, basketball_texels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, earth_texels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Initial Texture
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	// set up vertex arrays
	glBindVertexArray(vao);
	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spherePoints) + sizeof(sphereNormals) + sizeof(sphereTexture) , NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spherePoints), spherePoints);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spherePoints), sizeof(sphereNormals), sphereNormals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spherePoints) + sizeof(sphereNormals), sizeof(sphereTexture), sphereTexture);

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePoints)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(spherePoints) + sizeof(sphereNormals)));


	
	material_shininess = 150.0;
	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;

	AmbientProduct = glGetUniformLocation(program, "AmbientProduct");
	DiffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
	SpecularProduct = glGetUniformLocation(program, "SpecularProduct");
	LightPosition = glGetUniformLocation(program, "LightPosition");
	Shininess = glGetUniformLocation(program, "Shininess");

	glUniform4fv(AmbientProduct,1,ambient_product);
	glUniform4fv(DiffuseProduct,1, diffuse_product);
	glUniform4fv(SpecularProduct,1, specular_product);
	glUniform4fv(LightPosition,1, light_position);
	glUniform1f(Shininess,material_shininess);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	Color = glGetUniformLocation(program, "Color");

	PhongFlag = glGetUniformLocation(program, "PhongFlag");
	glUniform1i(PhongFlag, phongFlag);
	GouraudFlag = glGetUniformLocation(program, "GouraudFlag");
	glUniform1i(GouraudFlag, gouraudFlag);
	ModifiedFlag = glGetUniformLocation(program, "ModifiedFlag");
	glUniform1i(ModifiedFlag, modifiedFlag);
	TextureFlag = glGetUniformLocation(program, "TextureFlag");
	glUniform1i(TextureFlag, textureFlag);
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}
//----------------------------------------------------------------------------



void
display( void )
{	
	glBindVertexArray(vao);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 model_view = Scale(modelScale) * Translate(displacement) *
		RotateX(Theta[Xaxis]);
		
	if (movingLight) {
		glUniform4fv(LightPosition, 1, model_view * light_position);
	}

	glUniform4fv(Color, 1, vertex_colors[color]);
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

    glDrawArrays( GL_TRIANGLES, 0, NumVertices[object] );
	
    glutSwapBuffers();
	
}

//---------------------------------------------------------------------
//
// reshape
//

void reshape( int w, int h )
{	
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;

	mat4 projection;
	projection = Perspective(45.0, aspect, 0.5, 6.0);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

//----------------------------------------------------------------------------



void
keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' || key == 'q')
		exit(0);
	else if (key == 'H' || key == 'h')
		info();
	else if (key == 'I' || key == 'i') {
		displacement = vec3(bounds.xMin, bounds.yMax, -10.0);
		vel = ini_vel;
	}
	else if (key == 'r' || key == 'R') {
		vel.x = (GLfloat) 2.0 * (((rand() % 1000) / 1000.0) - 0.5);
		vel.y += (GLfloat) 2.0 * (rand() % 1000 / 1000.0);
	}
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y )
{

}

//----------------------------------------------------------------------------


void timer(int p) {
	currTime += deltaTime;

	// basic forward Euler integration 
	vel += acc * deltaTime;
	displacement += vel * deltaTime;

	// bounds checking and bounce with damping
	if (displacement.y < bounds.yMin + modelScale) {
		displacement.y = bounds.yMin + modelScale;
		vel.y *= -1;
		vel *= dampFactor;
	}
	else if (displacement.y > bounds.yMax - modelScale) {
		displacement.y = bounds.yMax - modelScale;
		vel.y *= -1;
		vel *= dampFactor;
	}

	if (displacement.x < bounds.xMin + modelScale) {
		displacement.x = bounds.xMin + modelScale;
		vel.x *= -1;
		vel *= dampFactor;
	}
	else if (displacement.x > bounds.xMax - modelScale) {
		displacement.x = bounds.xMax - modelScale;
		vel.x *= -1;
		vel *= dampFactor;
	}

	glutPostRedisplay();

	glutTimerFunc(20, timer, 0);
}


//----------------------------------------------------------------------------
void colorMenuCallback(int id)
{
	if (id == 0)
	{
		color = 0;
	}
	if (id==1)
	{
		color = 1;
	}
	if (id == 2)
	{
		color = 2;
	}
	if (id == 3)
	{
		color = 3;
	}
	if (id == 4)
	{
		color = 4;
	}
	if (id == 5)
	{
		color = 5;
	}
	if (id == 6)
	{
		color = 6;
	}
	if (id == 7)
	{
		color = 7;
	}
}

void mainMenu(int id)
{
}

void shade_setting_menu(int id)
{
}
void drawingMenuCallback(int id)
{
	if (id == 0) {
		phongFlag = false;
		gouraudFlag = false;
		modifiedFlag = false;
		textureFlag = false;
		glUniform1i(PhongFlag, phongFlag);
		glUniform1i(GouraudFlag, gouraudFlag);
		glUniform1i(ModifiedFlag, modifiedFlag);
		glUniform1i(TextureFlag, textureFlag);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
	}
}

void materialMenuCallback(int id)
{
	if (id == 0) {
		material_shininess = 150.0;
		glUniform1f(Shininess, material_shininess);
		glutPostRedisplay();
	}
	if (id == 1) {
		material_shininess = 7.0;
		glUniform1f(Shininess, material_shininess);
		glutPostRedisplay();
	}
}

void lightMenuCallback(int id)
{
	if (id == 0) {
		movingLight = false;
		glUniform4fv(LightPosition, 1, light_position);
		glutPostRedisplay();
	}
	if (id == 1) {
		movingLight = true;
		glutPostRedisplay();
	}
}
void shadowMenuCallback(int id) {
	if (id == 0) {

	}if (id == 1) {

	}
}

void lightingMenuCallback(int id)
{
	if (id == 0)
	{
		specular_product = light_specular * material_specular;
		glUniform4fv(SpecularProduct, 1, specular_product);
		glutPostRedisplay();
	}
	if (id == 1)
	{
		diffuse_product = light_diffuse * material_diffuse;
		glUniform4fv(DiffuseProduct, 1, diffuse_product);
		glutPostRedisplay();
	}
	if (id == 2)
	{
		ambient_product = light_ambient * material_ambient;
		glUniform4fv(AmbientProduct, 1, ambient_product);
		glutPostRedisplay();
	}
	if (id == 3)
	{
		specular_product = (0, 0, 0, 0);
		glUniform4fv(SpecularProduct, 1, specular_product);
		glutPostRedisplay();
	}
	if (id == 4)
	{
		diffuse_product = (0, 0, 0, 0);
		glUniform4fv(DiffuseProduct, 1, diffuse_product);
		glutPostRedisplay();
	}
	if (id == 5)
	{
		ambient_product = (0, 0, 0, 0);
		glUniform4fv(AmbientProduct, 1, ambient_product);
		glutPostRedisplay();
	}
}

void textureMenuCallback(int id)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	phongFlag = false;
	gouraudFlag = false;
	modifiedFlag = false;
	textureFlag = true;
	glUniform1i(PhongFlag, phongFlag);
	glUniform1i(GouraudFlag, gouraudFlag);
	glUniform1i(ModifiedFlag, modifiedFlag);
	glUniform1i(TextureFlag, textureFlag);
	if(id==0)
	{
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glutPostRedisplay();
	}
	if(id==1)
	{
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glutPostRedisplay();	
	}
}
void shadingMenuCallback(int id)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (id == 0) {
		phongFlag = true;
		gouraudFlag = false;
		modifiedFlag = false;
		textureFlag = false;
		glUniform1i(PhongFlag, phongFlag);
		glUniform1i(GouraudFlag, gouraudFlag);
		glUniform1i(ModifiedFlag, modifiedFlag);
		glUniform1i(TextureFlag, textureFlag);
		glutPostRedisplay();
	}
	if (id == 1) {
		phongFlag = false;
		gouraudFlag = true;
		modifiedFlag = false;
		textureFlag = false;
		glUniform1i(PhongFlag, phongFlag);
		glUniform1i(GouraudFlag, gouraudFlag);
		glUniform1i(ModifiedFlag, modifiedFlag);
		glUniform1i(TextureFlag, textureFlag);
		glutPostRedisplay();
	}
	if (id == 2) {
		phongFlag = false;
		gouraudFlag = false;
		modifiedFlag = true;
		textureFlag = false;
		glUniform1i(PhongFlag, phongFlag);
		glUniform1i(GouraudFlag, gouraudFlag);
		glUniform1i(ModifiedFlag, modifiedFlag);
		glUniform1i(TextureFlag, textureFlag);
		glutPostRedisplay();
	}

}

////// menu callbacks //////

// setting up the right click menu
void initMenus() {

	//menus
	int texture = glutCreateMenu(textureMenuCallback);
	glutAddMenuEntry("Basketball", 0);
	glutAddMenuEntry("Earth", 1);
	int shading = glutCreateMenu(shadingMenuCallback);
	glutAddMenuEntry("Phong Shading", 0);
	glutAddMenuEntry("Gouraud Shading", 1);
	glutAddMenuEntry("Modified Phong Shading", 2);

	int display = glutCreateMenu(drawingMenuCallback);
	glutAddMenuEntry("Wireframe", 0);
	glutAddSubMenu("Shading", shading);
	glutAddSubMenu("Texture", texture);
	int color = glutCreateMenu(colorMenuCallback);
	glutAddMenuEntry("Black", 0);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Yellow", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("White", 6);
	glutAddMenuEntry("Cyan", 7);
	int components = glutCreateMenu(lightingMenuCallback);
	glutAddMenuEntry("Specular On", 0);
	glutAddMenuEntry("Diffuse On", 1);
	glutAddMenuEntry("Ambient On", 2);
	glutAddMenuEntry("Specular Off", 3);
	glutAddMenuEntry("Diffuse Off", 4);
	glutAddMenuEntry("Ambient Off", 5);
	int light = glutCreateMenu(lightMenuCallback);
	glutAddMenuEntry("Fixed Light", 0);
	glutAddMenuEntry("Moving Light", 1);
	int material = glutCreateMenu(materialMenuCallback);
	glutAddMenuEntry("Metal", 0);
	glutAddMenuEntry("Plastic", 1);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("Components", components);
	glutAddSubMenu("Light Source", light);
	glutAddSubMenu("Material", material);
	glutAddSubMenu("Display Mode", display);
	glutAddSubMenu("Colors", color);



	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode(  GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutInitWindowPosition( 50, 50 );
    glutCreateWindow( "Homework 3" );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    glewExperimental = GL_TRUE;
    glewInit();

    init();
    
    glutDisplayFunc( display ); // set display callback function
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutKeyboardFunc(keyboard);
	
	initMenus();

	//-------------------------------------

    glutTimerFunc(10,timer,0);
    
    glutMainLoop();
    return 0;
}
