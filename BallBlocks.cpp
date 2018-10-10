#include<stdlib.h>
#include<GL/glut.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <ctime>

typedef struct {
	float x, y, z;
	float red, green, blue;
	int status;
	int onTable;

}Ball;

typedef struct {
	float red, green, blue;
	float x, y, z;
	int score;

}Hole;

typedef struct {
	float x, z;
	float w, h;
	int score;

}Block;

void init();
void cube();
void StopBalls();
void init_ball();
void init_holes();
void init_blocks();
void draw_Hole(Hole hole);
void draw_block(Block block);
void color_ball(float red, float green, float blue);


int rot = 0, rot2 = 0, rot3 = 0, flag = 0, CanSpeed = 1, IsSpeeding = 0, flag4, IsAnimatingTable = 0, arrlenght = 9; // rotations for different in game camera movements
int temp, reflex = 0, score = 0, start = 1, isVColliding, isHColliding; // reflex to check balls collision
int selected_Ball, old_Ball; // old ball to get colour when switching ball control and the current selected ball which the player is controlling
char s[10], str[100], *str2; // to print different characters on the screen using sprintf()
float aC, xC, zC, dis; // the C variables for circular calculations, dis = distance between the ball and the hole
float velocity = 15, speed = 0.005, defSpeed, rot_temp = 0; // variables for physics when a ball has collided with another ball starts a timer for speeding
int fRight[9] = { 0 }, fLeft[9] = { 0 }, fDown[9] = { 0 }, fUp[9] = { 0 }; // The flags that for each ball, initialize with 0 and 1 when the specific side has entered collision
clock_t starttime = clock(); // use the clock for game time ( 60 seconds)
clock_t endtime; // end time for the clock
double duration; // the duration of the animation
Ball balls[9];
Hole holes[5];
Block blocks[8];

void reset()  // reset all values when starting new game
{
	rot = rot2 = rot3 = 0;
	flag = IsSpeeding = IsAnimatingTable = 0;
	CanSpeed = 1; 
	reflex = 0;
	start = 1; velocity = 15; speed = 0.005; rot_temp = 0;
}
void up()   // When new game starts the table going up
{
	rot_temp += 0.03;

	if (rot_temp >= 1.0)
	{
		rot2 += 1.0;
		rot_temp = 0;
	}

	if (rot2 >= 60)
	{
		IsAnimatingTable = 1;
		starttime = clock();
	}
}
void print_score()  /// and timer
{
	sprintf(str, "Score : %d", score);
	str2 = str;
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos3f(-9, 10, 0);
	do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str2);
	while (*(++str2));

	if (IsAnimatingTable) {        /// when the table stop animating moving up starts the clock
		endtime = clock();
		duration = (endtime - starttime) / (double) CLOCKS_PER_SEC; // get the time frame
		sprintf(str, "Timer : %0.f", 60.0 - duration); // print time left on the screen
		str2 = str;
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos3f(-8.5, 11, 0);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str2);
		while (*(++str2));

		if (duration >= 60) {
			StopBalls();
			reset();
		}
	}
}

// return the horizontal side of the collision
int checkCollisionHorizontal(Ball a) {
	Block b;
	for (int k = 4; k < 8; k++) {
		b = blocks[k];
		if (a.x <= b.x + b.w &&
			a.x >= b.x &&
			a.z <= b.z + b.h &&
			a.z >= b.z - b.h) {
			printf("left\n");
			return 2;
		}

		if (a.x <= b.x &&
			a.x >= b.x - b.w &&
			a.z <= b.z + b.h &&
			a.z >= b.z - b.h) {
			printf("right\n");
			return 1;
		}
	}
}

// return the vertical side of the collision
int checkCollisionVertical(Ball a)
{
	Block b;
	for (int k = 4; k < 8; k++) {
		b = blocks[k];
			//check the X axis
			if (a.x <= b.x + b.w &&
				a.x >= b.x - b.w &&
				a.z <= b.z &&
				a.z >= b.z - b.h) {

				printf("up\n");
				return 4;
			}

			if (a.x <= b.x + b.w &&
				a.x >= b.x - b.w &&
				a.z <= b.z + b.h &&
				a.z >= b.z - b.h) {
				printf("down\n");
				return 3;
			}


		}
	return 0;
}
// I is the controlled ball and J is the other balls, 0.6 = radius
// checks for collision and move the balls accordingly
int move_x(int i)
{
	int j;
	for (j = 0; j<arrlenght; j++)
	{
		if (j != i && balls[j].onTable == 0) {
			if (balls[i].x >= balls[j].x - 0.6 &&
				balls[i].x <= balls[j].x + 0.6 &&
				balls[i].z >= balls[j].z - 0.6 &&
				balls[i].z <= balls[j].z + 0.6)
				return j;
		}
	}
	return -1;
}
void StopBalls()   // Stop's balls moving
{
	balls[selected_Ball].status = 1;

	for (int i = 0; i < arrlenght; i++)
	{
		if (i != selected_Ball)
			balls[i].status = 0;

		fRight[i] = 0;
		fLeft[i] = 0;
		fDown[i] = 0;
		fUp[i] = 0;
	}
}
void checkgame()  /// To Check if All the Ball's are inserted to the holes (End Game) 
{
	for (int i = 1; i < 9 && balls[i].onTable == 1; i++)
		if (i == 8)
		{
			StopBalls();
			reset();
		}

	if (score == 50) {
		StopBalls();
		reset();
	}
}
// The arrow keys
void SpecialInput(int key, int x, int y)
{
	if (key == GLUT_KEY_RIGHT) {
		fRight[selected_Ball] = 1;
		fLeft[selected_Ball] = 0;
	}
	if (key == GLUT_KEY_LEFT) {
		fLeft[selected_Ball] = 1;
		fRight[selected_Ball] = 0;
	}
	if (key == GLUT_KEY_UP) {
		fDown[selected_Ball] = 0;
		fUp[selected_Ball] = 1;
	}
	if (key == GLUT_KEY_DOWN) {
		fUp[selected_Ball] = 0;
		fDown[selected_Ball] = 1;
	}
}
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(1);

	if (key == 'n' && start == 1)
	{
		init_ball();
		start = 0;
		score = 0;
	}  ///
	if (key == 'd')
	{
		rot += 5;
		rot %= 360;
	} /// 
	if (key == 'w')
	{
		rot2 += 5;
		rot2 %= 360;
	} ///
	if (key == 's')
	{
		rot2 -= 5;
		rot2 %= 360;
	} ///
	if (key == 'b')
	{
		StopBalls();
		reset();
		start = 1;
	} ///
	if (key == 'a')
	{
		rot -= 5;
		rot %= 360;
	} ///
	if (key == 'z')
	{
		rot3 += 5;
		rot3 %= 360;
	} ///
	if (key == 'x')
	{
		rot3 -= 5;
		rot3 %= 360;
	} /// select balls by the numbers and return the other ball to old colour
	if (key >= '0' && key <= '9') {
		old_Ball = selected_Ball;
		selected_Ball = key - 48;

		balls[old_Ball].blue = balls[selected_Ball].blue;
		balls[old_Ball].green = balls[selected_Ball].green;
		balls[old_Ball].red = balls[selected_Ball].red;

		balls[selected_Ball].status = 1;
		balls[selected_Ball].blue = 1;
		balls[selected_Ball].green = 1;
		balls[selected_Ball].red = 1;
	}
	// stop all balls from moving
	if (key == '5')
	{
		StopBalls();
	} ///
	if (key == '+')   // increase ball speed
	{
		flag4 = 0;
		if (speed < 0.025) {
			for (int i = 1; i < 9; i++)
			{
				if (balls[i].status == 1)
					flag4 = 1;
			}
			if (flag4 == 0)
				speed = speed + 0.002;
		}
	} ///
	if (key == '-') // slow ball speed 
	{
		flag4 = 0;
		if (speed > 0.005) {
			for (int i = 1; i < 9; i++)
			{
				if (balls[i].status == 1)
					flag4 = 1;
			}
			if (flag4 == 0)
				speed = speed - 0.002;
		}
	} ///
	glutPostRedisplay();
}
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, -15, -30);
	glRotatef(rot, 0, 1, 0);
	glRotatef(rot2, 1, 0, 0);
	glRotatef(rot3, 0, 0, 1);
	/************************************************************/

	if (start == 1)
	{
		str2 = "Press N To Start New Game";
		glColor3f(1.0, 1.0, 0.0);
		glRasterPos3f(-8, 15, 0);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str2);
		while (*(++str2));

		sprintf(str, "Your Score : %d", score);
		str2 = str;
		glColor3f(1.0, 1.0, 0.0);
		glRasterPos3f(-6, 19, 0);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str2);
		while (*(++str2));
	}

	if (start == 0)
	{
		if (!IsAnimatingTable)
			up();

		checkgame();

		//Floor
		{
			glTranslatef(0, 4, -12);
			glColor3f(1, 1, 0.8);
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0); // normal straight up
			glVertex3f(-20, 0, -25);
			glVertex3f(20, 0, -25);
			glVertex3f(20, 0, 25);
			glVertex3f(-20, 0, 25);
			glEnd();

			glColor3f(0.2, 0.7, 0.3);
			glTranslatef(0, 8, 0);
		}
		//Table
		{
			
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0); // normal straight up
			glVertex3f(-9, 0, -12);
			glVertex3f(9, 0, -12);
			glVertex3f(9, 0, 12);
			glVertex3f(-9, 0, 12);
			glEnd();

			//Table Legs 
			glColor3f(0.2, 0.2, 0.5);

			glTranslatef(-8.5, 0, 11.5);
			cube();
			glTranslatef(0, 0, -23);
			cube();
			glTranslatef(17, 0, 0);
			cube();
			glTranslatef(0, 0, 23);
			cube();

			glTranslatef(-8.5, 0, -11.5);  // 0 point
		}
		print_score();	
		//Walls
		{		
			glColor3f(0.6, 0.6, 0.6);
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);  // normal straight up
			glVertex3f(-20, -8, -25);
			glVertex3f(20, -8, -25);
			glVertex3f(20, 20, -25);
			glVertex3f(-20, 20, -25);
			glEnd();

			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0); // normal straight up
			glVertex3f(-20, -8, -25);
			glVertex3f(-20, 20, -25);
			glVertex3f(-20, 20, 25);
			glVertex3f(-20, -8, 25);
			glEnd();

			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0); // normal straight up
			glVertex3f(20, -8, -25);
			glVertex3f(20, 20, -25);
			glVertex3f(20, 20, 25);
			glVertex3f(20, -8, 25);
			glEnd();
		}

		//Blocks
		for (int i = 0; i < 8; i++) {
			glTranslatef(blocks[i].x, 0, blocks[i].z);
			draw_block(blocks[i]);
			glTranslatef(-blocks[i].x, 0, -blocks[i].z);
		}
		
		//Holes
		for (int i = 0; i < 5; i++)
		{
			draw_Hole(holes[i]);
		}

		//Balls
		glTranslatef(0, 0.3, 0);       // y = ball's radius

		for (int i = 0; i < arrlenght; i++)
		{
			glTranslatef(balls[i].x, balls[i].y, balls[i].z);
			color_ball(balls[i].red, balls[i].green, balls[i].blue);
			glTranslatef(-balls[i].x, -balls[i].y, -balls[i].z);
		}

		glTranslatef(0, -0.3, 0);
	}

	glutSwapBuffers(); // display the output

}
void Checkflag(int i)  // to check if a ball hit a hole
{
	if (balls[i].onTable == 0) {   // to check if the ball still in the game

		for (int j = 0; j < 5; j++) {
			dis = sqrt(pow(balls[i].x - (holes[j].x), 2) + pow(balls[i].z - (holes[j].z), 2));
			if (dis < 0.9) {
				score += holes[j].score;
				if (holes[j].red == 1) {					
					balls[i].onTable = 2;
					balls[i].status = 0;
					printf("%d\n", score);
				}
				else{
					balls[i].x = 0;
					balls[i].z = -7.5;
					fRight[i] = 0;
					fLeft[i] = 0;
					fDown[i] = 0;
					fUp[i] = 0;
					printf("%d\n", score);
				}
			}
		}
	}
}
// here we check collision, which side of the collision is occurring
// the flags for the sides (fLeft, fRight, fDown, fUp) that each ball has
// we can set a reflex and move the ball accordingly
void Idle()
{
	isVColliding = isHColliding = 0;
	for (int i = 0; i < arrlenght; i++) {	
        // both functions return a number that represents the collided side (left,right) (up,down)	
		isVColliding = checkCollisionVertical(balls[i]); // check ball collided with block vertically 
		isHColliding = checkCollisionHorizontal(balls[i]); // check ball collided with block horizontally
		Checkflag(i);   // check if the ball hit a hole
		if (balls[i].onTable == 0) {            /// check of the ball still in the table
			if (fRight[i])             // each ball have her own right flags
			{
			// is the ball still in the game? 
			// if yes starts moving until it has collided with block
				if (balls[i].status == 1 && move_x(i) == -1)
					// TODO: add conditions for vertical+horizontal collision example: left & up
					if (balls[i].x >= 7.7 || isHColliding == 1) {
						fLeft[i] = 1;
						fRight[i] = 0;
					}
					else {
						balls[i].x += speed;
					}
					// if the ball is still in the game and has collided with another ball
					if (balls[i].status == 1 && move_x(i) != -1) {
						temp = move_x(i);
						balls[temp].status = 1;
						fRight[temp] = 1;
						fLeft[temp] = 0;
						reflex = 1;
						balls[i].x -= speed;
						fLeft[i] = 1;
						fRight[i] = 0;

						if (fUp[i] == 1)
							fUp[temp] = 1;
						if (fDown[i] == 1)
							fDown[temp] = 1;
					}
			}
			if (fLeft[i])
			{
				if (balls[i].status == 1 && move_x(i) == -1)
					if (balls[i].x <= -7.7 || isHColliding == 2) {
						fLeft[i] = 0;
						fRight[i] = 1;
					}
					else {
						balls[i].x -= speed;
					}
				
				if (balls[i].status == 1 && move_x(i) != -1) {
					temp = move_x(i);
					balls[temp].status = 1;
					fRight[temp] = 0;
					fLeft[temp] = 1;
					reflex = 1;
					balls[i].x += speed;
					fLeft[i] = 0;
					fRight[i] = 1;
					if (fUp[i] == 1)
						fUp[temp] = 1;
					if (fDown[i] == 1)
						fDown[temp] = 1;
				}
			}
			if (fUp[i])
			{
				if (balls[i].status == 1 && move_x(i) == -1)
					if (balls[i].z <= -10.7 || isVColliding == 3) {
						fUp[i] = 0;
						fDown[i] = 1;
					}
					else
					{
						balls[i].z -= speed;
					}
				if (balls[i].status == 1 && move_x(i) != -1) {
					temp = move_x(i);
					balls[temp].status = 1;
					fUp[temp] = 1;
					fDown[temp] = 0;
					balls[i].z += speed;
					reflex = 1;
					fDown[i] = 1;
					fUp[i] = 0;

					if (fLeft[i] == 1)
						fLeft[temp] = 1;
					if (fRight[i] == 1)
						fRight[temp] = 1;
				}
			}
			if (fDown[i])
			{
				if (balls[i].status == 1 && move_x(i) == -1)
					if (balls[i].z >= 10.7 || isVColliding == 4)
					{
						fUp[i] = 1;
						fDown[i] = 0;
					}					
					else
					{
						balls[i].z += speed;
					}
				if (balls[i].status == 1 && move_x(i) != -1) {

					temp = move_x(i);
					balls[temp].status = 1;
					fUp[temp] = 0;
					fDown[temp] = 1;
					balls[i].z -= speed;
					reflex = 1;
					fDown[i] = 0;
					fUp[i] = 1;
					if (fLeft[i] == 1)
						fLeft[temp] = 1;
					if (fRight[i] == 1)
						fRight[temp] = 1;
				}
			}		
		}
		// keep the balls on the table
		if (balls[i].onTable == 2)
		{

			balls[i].y = balls[i].y - 0.002;

			if (balls[i].y <-7.7)
				balls[i].onTable = 1;
		}
	}

	if (reflex) {            //  if a ball hit another ball reflex=1 starts to speed then stop after some time (15 frame)
		if (velocity > 0) {
			if (IsSpeeding == 0) {
				defSpeed = speed;   // saving the speed
				printf("%f\n", defSpeed);
				IsSpeeding = 1;
				printf("here\n");
			}
			velocity -= defSpeed;
			speed = speed - (0.0007*speed);
			if (CanSpeed && velocity < 10) {
				balls[selected_Ball].status = 0;
				CanSpeed = 0;
			}
		}
		else {     // stop ball's moving
			speed = defSpeed;   // return speed 
			velocity = 15;
			CanSpeed = 1;
			IsSpeeding = 0;
			StopBalls();
			reflex = 0;
		}
	}
	draw();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // RGB display, double-buffered, with Z-Buffer
	glutInitWindowSize(600, 600); // 500 x 500 pixels
	glutCreateWindow("3D");
	init();
	glutDisplayFunc(draw); // Set the display function
	glutKeyboardFunc(keyboard); // Set the keyboard function
	glutSpecialFunc(SpecialInput); //special keyboard keys
	init_ball(); // draw the balls
	init_holes(); // draw holes
	init_blocks(); // draw inside and outside walls
	glutIdleFunc(Idle);
	glutMainLoop(); // Start the main event loop

}
// Set OpenGL parameters
void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);

	// Lighting parameters

	GLfloat mat_ambdif[] = { 1.0,2.0, 0.0, 1.0 };
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 80.0 };
	GLfloat light_position[] = { 0.0, 5.0, 0.0, 1.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambdif); // set both amb and diff components
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); // set specular
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); // set shininess
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // set light "position", in this case direction
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // active material changes by glColor3f(..)

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
}
// used to draw the table legs
// TODO: switch to blocks
void cube()
{
	glBegin(GL_QUADS);
	// Front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, 0, 0.5);
	glVertex3f(-0.5, -8, 0.5);
	glVertex3f(0.5, -8, 0.5);
	glVertex3f(0.5, 0, 0.5);

	// Back
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, 0, -0.5);
	glVertex3f(0.5, 0, -0.5);
	glVertex3f(0.5, -8, -0.5);
	glVertex3f(-0.5, -8, -0.5);

	// Left side
	glNormal3f(1, 0, 0);
	glVertex3f(-0.5, 0, 0.5);
	glVertex3f(-0.5, -8, 0.5);
	glVertex3f(-0.5, -8, -0.5);
	glVertex3f(-0.5, 0, -0.5);

	// Right side
	glNormal3f(1, 0, 0);
	glVertex3f(0.5, 0, 0.5);
	glVertex3f(0.5, 0, -0.5);
	glVertex3f(0.5, -8, -0.5);
	glVertex3f(0.5, -8, 0.5);
	glEnd();
}
// draws the block on the screen
void draw_block(Block block)
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);

	//Front
	glNormal3f(0, 0, 1);
	glVertex3f(-block.w, 0, block.h);
	glVertex3f(-block.w, 0.6, block.h);
	glVertex3f(block.w, 0.6, block.h);
	glVertex3f(block.w, 0, block.h);

	// Back
	glNormal3f(0, 0, 1);
	glVertex3f(-block.w, 0, -block.h);
	glVertex3f(block.w, 0.6, -block.h);
	glVertex3f(block.w, 0.6, -block.h);
	glVertex3f(-block.w, 0, -block.h);

	// Left side
	glNormal3f(-1, 0, 0);
	glVertex3f(-block.w, 0, block.h);
	glVertex3f(-block.w, 0.6, block.h);
	glVertex3f(-block.w, 0.6, -block.h);
	glVertex3f(-block.w, 0, -block.h);

	// Right side
	glNormal3f(1, 0, 0);
	glVertex3f(block.w, 0, block.h);
	glVertex3f(block.w, 0, -block.h);
	glVertex3f(block.w, 0.6, -block.h);
	glVertex3f(block.w, 0.6, block.h);

	// Up side
	glNormal3f(1, 0, 0);
	glVertex3f(block.w, 0.6, block.h);
	glVertex3f(block.w, 0.6, -block.h);
	glVertex3f(-block.w, 0.6, -block.h);
	glVertex3f(-block.w, 0.6, block.h);

	glEnd();
}
// position the 4 surrounding walls and the 4 inside cubes 
//TODO: set the variables width and height and replace the numbers
void init_blocks() 
{
	blocks[0] = { 0.0f, -11.5f, 9.0f, 0.5f, 0};
	blocks[1] = { 0.0f, 11.5f, 9.0f, 0.5f, 0 };
	blocks[2] = { 8.5f, 0.0f, 0.5f, 12.0f, 0 };
	blocks[3] = { -8.5f, 0.0f, 0.5f, 12.0f, 0 };

	blocks[4] = { -5.0f, -7.0f, 3.5f, 1.0f, 2 }; // left up
	blocks[5] = { -5.0f,  6.5f, 3.5f, 1.0f, 2 }; // left down
	blocks[6] = { 5.0f,  6.5f, 3.5f, 1.0f, 2 }; // right down
	blocks[7] = { 0.0f, -1.0f, 3.5f, 1.0f, 2 }; // center
}

// position the 9 different balls on screen
void init_ball()
{
	balls[0] = {0.0f , 0.0f, -5.0f, 1.0,1.0,1.0, 1,0};
	////////////////////////////
	balls[1] = {0.0f, 0.0f, -3.0f, 0.0f, 0.0f, 1.0f, 0, 0};
	////////////////////////////
	balls[2] = {-1.0f, 0.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0,0};
	////////////////////////////
	balls[3] = { 1.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 0,0};
	////////////////////////////
	balls[4] = {-4.0f, 0.0f, -3.0f, 0.8f, 0.5f, 0.2f, 0,0};
	////////////////////////////
	balls[5] = {1.0, 0.0f, 5.0f, 0.2f, 0.5f, 0.86f, 0,0};
	////////////////////////////
	balls[6] = {0.0f, 0.0f, 4.0f, 0.2f, 0.7f, 0.7f, 0,0};
	////////////////////////////
	balls[7] = {-6.0f, 0.0f, 8.0f, 0.3f, 0.1f, 0.6f, 0,0};
	////////////////////////////
	balls[8] = {6.0f, 0.0f, 3.0f, 0.8f, 0.6f, 0.2f, 0,0};
}
void init_holes() {
	holes[0] = { 1.0f, 0.0f, 0.0f, 6.5f, 0.1f, 9.5f, -10 };// down right
	holes[1] = { 0.0f, 0.0f, 0.0f, -1.5f, 0.1f, 9.5f, 15 };// down left
	holes[2] = { 1.0f, 0.0f, 0.0f, -6.0f, 0.1f, 1.0f, -10 };// center left
	holes[3] = { 0.0f, 0.0f, 0.0f, 6.0f, 0.1f, 1.0f, 15 };// center right
	holes[4] = { 0.0f, 0.0f, 0.0f, 6.0f, 0.1f, -8.0f, 15 };// up right
}
void draw_Hole(Hole hole) {
	glColor3f(hole.red, hole.green, hole.blue);
	glTranslatef(hole.x, hole.y, hole.z);
	glBegin(GL_POLYGON);
	for (int i = 0; i <= 720; i++) {
		aC = 3.14 * i;
		xC = cos(aC / 360);
		zC = sin(aC / 360);
		glVertex3d(xC, 0, zC);
	}
	glEnd();
	glTranslatef(-hole.x, -hole.y, -hole.z); // 0 point
}
void color_ball(float red, float green, float blue)
{
	glColor3f(red, green, blue);
	glutSolidSphere(0.3, 32, 32);
}
