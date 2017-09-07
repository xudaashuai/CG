
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <math.h>
using namespace std;
/*
* Initialize depth buffer, projection matrix, light source, and lighting
* model.  Do not specify a material property here.
*/

// 颜色数组
int colorIndex = 0;
const int ColorCount = 7;
const GLfloat Colors[][4] = { { 0.0, 0.0, 0.0, 0.0 },{ 0.1, 0.1, 0.1, 0 },{ 0.2, 0.2, 0.2, 0 },{ 0.3, 0.3, 0.3, 0 },{ 0.4, 0.4, 0.4, 0 },{ 0.5, 0.5, 0.5, 0 },{ 0.6, 0.6, 0.6, 0 },{ 0.7, 0.7, 0.7, 0 },{ 0.8, 0.8, 0.8, 0 },{ 0.9, 0.9, 0.9, 0 } };
GLfloat curColor[4] = { 1,1,1,1 }, oldColor[4], newColor[4];
//光源位置
GLfloat position[] = { 1.0, 1.0, 5.0, 0.0 };
//窗口大小
int shape[] = { 400,400 };
GLfloat moveOffset[3], angles[3];
int mousePostion[2];
bool isRotate = false, isAnimRotate = false, isColorChange = false, isDrag = false, isZip = false;
float colorTransition = 0.0, scale = 10;
auto DispList = glGenLists(1);
// 记录读取文件
vector<array<GLfloat, 3>> points;
vector<array<int, 3>> triangles;
vector<array<int, 2>> lines;
//初始化
void init(void) {
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { .5, .5, .5, 1.0 };
	GLfloat local_view[] = { 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	glFrontFace(GL_CW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
}
//画图
void drawString(char* str) {
	int n = strlen(str);
	for (int i = 0; i < n; i++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *(str + i));
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat white[] = { 1.0,1.0,1.0,1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	char str[100];
	GLfloat p[] = { 0,0,0,0 };
	glRasterPos2i(0, 0);
	sprintf_s(str, "angles %f %f %f", angles[0], angles[1], angles[2]);
	drawString(str);
	p[1] += 0.7;
	glRasterPos3fv(p);
	sprintf_s(str, "translate %f %f %f", moveOffset[0], moveOffset[1], moveOffset[2]);
	drawString(str);
	p[1] += 0.7;
	glRasterPos3fv(p);
	sprintf_s(str, "scale %f ", scale);
	drawString(str);
	p[1] += 0.7;
	glRasterPos3fv(p);
	sprintf_s(str, "color r:%f g:%f b:%f ", curColor[0], curColor[1],curColor[2]);
	drawString(str);
	glPushMatrix();
	glTranslatef(6, 5, 0.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, curColor);
	//glutSolidTeapot(3.0);
	glRotatef(angles[0], 1.0, 0.0, 0.0);
	glRotatef(angles[1], 0, 1.0, 0.0);
	glScalef(scale, scale, scale);
	glTranslatef(moveOffset[0], -moveOffset[1], moveOffset[2]);
	//glCallList(1);
	for (auto triangle : triangles) {
		if (triangle[0] == triangle[1] || triangle[1] == triangle[2] || triangle[0] == triangle[2]) {
			continue;
		}
		glBegin(GL_LINE_LOOP);
		glVertex3d(points[triangle[0] - 1][0], points[triangle[0] - 1][1], points[triangle[0] - 1][2]);
		glVertex3d(points[triangle[1] - 1][0], points[triangle[1] - 1][1], points[triangle[1] - 1][2]);
		glVertex3d(points[triangle[2] - 1][0], points[triangle[2] - 1][1], points[triangle[2] - 1][2]);
		glEnd();
	}
	glPopMatrix();
	glFlush();
}
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(0.0, 16.0, 0.0, 16.0*(GLfloat)h / (GLfloat)w,
			-10.0, 10.0);
	else
		glOrtho(0.0, 16.0*(GLfloat)w / (GLfloat)h, 0.0, 16.0,
			-10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	shape[0] = w;
	shape[1] = h;
}
// 一些小函数
template<class T>
void colorAssignment(T target[], const T* from, int length = 4) {
	for (int i = 0; i < length; i++) {
		target[i] = from[i];
	}
}
void changeColor() {
	colorIndex += 1;
	colorIndex %= ColorCount;
	colorAssignment(newColor, Colors[colorIndex]);
	colorAssignment(oldColor, curColor);
	isColorChange = true;
}
void changeAngle(int index, float offset) {
	angles[index] += offset;
	for (int i = 0; i < 3; i++) {
		if (angles[i] >= 360) {
			angles[i] -= 360;
		} else if (angles[i] < 0) {
			angles[i] += 360;
		}
	}
}
float getRandomFloat() {
	return rand() / double(RAND_MAX);
}
void randomColor() {
	GLfloat temp[4]{ getRandomFloat(),getRandomFloat(),getRandomFloat(),0 };
	colorAssignment(newColor, temp);
	colorAssignment(oldColor, curColor);
	colorTransition = 0.0;
	isColorChange = true;
}
void setCurColor() {
	for (int i = 0; i < 4; i++) {
		curColor[i] = oldColor[i] + (newColor[i] - oldColor[i]) * colorTransition;
	}
}
void moveLight(float offset[2]) {
	position[0] += offset[0];
	position[1] += offset[1];
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}
// 网格收缩算法
void genLines() {
	lines.clear();
	for (auto&triangle : triangles) {
		for (int i = 0; i < 3; i++) {
			if (triangle[i] != triangle[(i + 1) % 3]) {
				lines.push_back({ triangle[i],triangle[(i + 1) % 3] });
			}
		}
	}
}
void genTriangles() {
	for (auto it = triangles.begin(); it != triangles.end();) {
		auto triangle = *it;
		for (int i = 0; i < triangle.size(); i++) {
			if (triangle[i] == triangle[(i + 1) % 3]) {
				it = triangles.erase(it);
				break;
			}
			if (i == triangle.size() - 1) {
				++it;
			}
		}
	}
}
float pointDistance(array<GLfloat, 3> &left, array<GLfloat, 3> &right) {
	return pow(left[0] - right[0], 2) + pow(left[1] - right[1], 2) + pow(left[2] - right[2], 2);
}
void zip(vector<array<GLfloat, 3>> &points, vector<array<int, 2>> &lines, vector<array<int, 3>> &triangles, int num) {
	for (int i = 0; i < num; i++) {
		genTriangles();
		genLines();
		if (lines.size() <= 10) {
			isZip = false;
			break;
		}
		// 计算并找出最短线段
		vector<float> lineLengths;
		int index = 0;
		for (auto &line : lines) {
			float distance = pointDistance(points[line[0] - 1], points[line[1] - 1]);
			lineLengths.push_back(distance);
			if (distance < lineLengths[index]) {
				index = lineLengths.size() - 1;
			}
		}
		for (int i = 0; i < 3; i++) {
			float	t = (points[lines[index][0] - 1][i] + points[lines[index][1] - 1][i]) / 2;
			points[lines[index][0] - 1][i] = t;
		}
		for (auto&triangle : triangles) {
			for (auto &i : triangle) {
				if (i == lines[index][1]) {
					i = lines[index][0];
				}
			}
		}
		for (auto&triangle : lines) {
			for (auto &i : triangle) {
				if (i == lines[index][1]) {
					i = lines[index][0];
				}
			}
		}
		lines.erase(lines.begin() + index);
	}
}
// 定时器实现动画
void anim(int value) {
	if (isAnimRotate) {
		changeAngle(1, 0.3);
	}
	if (isZip) {
		zip(points, lines, triangles, 1);
	}
	if (isColorChange) {
		colorTransition += 0.04;
		setCurColor();
		if (colorTransition >= 1) {
			colorAssignment(curColor, newColor);
			isColorChange = false;
			colorTransition = 0;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(20, anim, value);
}
// 事件回调
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		exit(0);
		break;
	case 27:
		exit(0);
		break;
	case 'c':
		changeColor();
		break;
	case 'x':
		randomColor();
		break;
	case '+':
		scale += 0.5;
		break;
	case '-':
		scale -= 0.5;
		break;
	case 'r':
		isAnimRotate = !isAnimRotate;
		break;
	case 't':
		isZip = !isZip;
		break;

	}
	glutPostRedisplay();
}
void specialKeyboard(int key, int x, int y) {
	if (key > 99 && key < 104) {
		float temp[2] = { (key - 101) % 2 * 0.5 ,(key - 102) % 2 * 0.5 };
		moveLight(temp);
	} else {
		cout << key << endl;
	}
	glutPostRedisplay();
}
void mouseMotion(int x, int y) {
	if (isRotate) {
		changeAngle(0, (y - mousePostion[1]) * 0.5);
		changeAngle(1, (x - mousePostion[0]) * 0.5);
	} else if (isDrag) {
		moveOffset[0] += (x - mousePostion[0]) * 1.0 / shape[0];
		moveOffset[1] += (y - mousePostion[1]) * 1.0 / shape[1];
	} else {
		return;
	}
	mousePostion[0] = x;
	mousePostion[1] = y;
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {
	mousePostion[0] = x;
	mousePostion[1] = y;
	isRotate = (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN);
	isDrag = (button == GLUT_RIGHT_BUTTON&&state == GLUT_DOWN);
}
// 文件读取
void input() {
	// 读入输入信息
	char ch;
	while (cin >> ch) {
		if (ch == 'v') {
			array<GLfloat, 3> triangle;
			cin >> triangle[0] >> triangle[1] >> triangle[2];
			points.push_back(triangle);
		} else {
			array<int, 3> point;
			cin >> point[0] >> point[1] >> point[2];
			triangles.push_back(point);
		}
	}
	// 开始记录display list
	glNewList(1, GL_COMPILE);
	cout << triangles.size() << endl;
	if (triangles.size() == 0) {
		glutSolidTeapot(0.3);
	} else {
		for (auto triangle : triangles) {
			glBegin(GL_TRIANGLES);
			glVertex3d(points[triangle[0] - 1][0], points[triangle[0] - 1][1], points[triangle[0] - 1][2]);
			glVertex3d(points[triangle[1] - 1][0], points[triangle[1] - 1][1], points[triangle[1] - 1][2]);
			glVertex3d(points[triangle[2] - 1][0], points[triangle[2] - 1][1], points[triangle[2] - 1][2]);
			glEnd();
		}
	}
	glEndList();
}
void inputFile(string filename) {
	fstream cin(filename);
	// 读入输入信息
	char ch;
	while (cin >> ch) {
		if (ch == 'v') {
			array<GLfloat, 3> triangle;
			cin >> triangle[0] >> triangle[1] >> triangle[2];
			points.push_back(triangle);
		} else {
			array<int, 3> point;
			cin >> point[0] >> point[1] >> point[2];
			triangles.push_back(point);
		}
	}
	genLines();
	// 开始记录 display list
	glNewList(1, GL_COMPILE);
	if (triangles.size() == 0) {
		glutSolidTeapot(0.3);
	} else {
		for (auto triangle : triangles) {
			glBegin(GL_TRIANGLES);
			glVertex3d(points[triangle[0] - 1][0], points[triangle[0] - 1][1], points[triangle[0] - 1][2]);
			glVertex3d(points[triangle[1] - 1][0], points[triangle[1] - 1][1], points[triangle[1] - 1][2]);
			glVertex3d(points[triangle[2] - 1][0], points[triangle[2] - 1][1], points[triangle[2] - 1][2]);
			glEnd();
		}
	}
	glEndList();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("hello world");
	init();
	glutReshapeFunc(reshape);
	input();
	// inputFile("E:\\CG\\作业0\\bunny_1k.obj");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);
	glutTimerFunc(20, anim, 1);
	glutMainLoop();
	return 0;
}
