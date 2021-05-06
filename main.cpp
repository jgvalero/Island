/*
Description: This program generates an island based on user input
Usage: g++ -g -Wall main.cpp -o island	--> ./island	--> Follow prompted instructions
*/

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

// Declare functions
int** makeParticleMap(int width, int height, int minX, int maxX, int minY, int maxY, int numParticles, int maxLife);
bool moveExists(int** map, int width, int height, int x, int y);
int findMax(int** map, int width, int height);
void normalizeMap(int** map, int width, int height, int maxVal);

int main() {
	// Declare Variables
	int width, height;
	int x_min, x_max, y_min, y_max;
	int particles;
	int maxlife;
	int waterline;
	srand((unsigned int)time(NULL));

	ofstream myfile ("island.txt");

	// Input width and height
	cout << "Enter width: ";
	cin >> width;
	cout << "Enter height: ";
	cin >> height;

	// Input x and y min and max
	cout << "Enter x minimum: ";
	cin >> x_min;
	cout << "Enter x maximum: ";
	cin >> x_max;
	cout << "Enter y minimum: ";
	cin >> y_min;
	cout << "Enter y maximum: ";
	cin >> y_max;

	// Check for errors
	if (x_min < 0 && y_min < 0) {
		cout << "Error: x_min and/or y_min need to be greater than 0" << endl;
		exit(-1);
	}
	else if (x_max <= x_min) {
		cout << "Error: x_max has to be greater than x_min" << endl;
		exit(-1);
	}
	else if (y_max <= y_min) {
		cout << "Error: y_max has to be greater than y_min" << endl;
		exit(-1);
	}
	else if (x_max >= width && x_max >= height && y_max >= width && y_max >= height) {
		cout << "Error: x_max and/or y_max has to be less than width and/or height" << endl;
		exit(-1);
	}

	// Input x and y min and max
	cout << "Enter number of particles to drop: ";
	cin >> particles;
	cout << "Enter max-life of a particle: ";
	cin >> maxlife;
	cout << "Enter water-line (between 40 and 200): ";
	cin >> waterline;
	if (!(waterline >= 40 && waterline <= 200)) {
		cout << "Error: waterline not in specified range" << endl;
		exit(-1);
	}

	// Create a rough island from function
	int** islandRough = makeParticleMap(width, height, x_min, x_max, y_min, y_max, particles, maxlife);

	// Find maximum value of array
	int max = findMax(islandRough, width, height);

	// Normalize island
	normalizeMap(islandRough, width, height, max);

	// Create another 2D array for polished island
	char** islandPolished;
	islandPolished = new char*[height];
	for(int i = 0; i < height; i++) {
		islandPolished[i] = new char[width];
	} 

	// Create polished island
	int landzone = 255 - waterline;
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			// Deep Water
			if (islandRough[i][j] < (waterline/2)) {
				islandPolished[i][j] = '#';
			}
			// Shallow Water
			else if (islandRough[i][j] > (waterline/2) && islandRough[i][j] <= waterline) {
				islandPolished[i][j] = '~';
			}
			// Island
			else {
				// Coast/Beach
				if (islandRough[i][j] < (waterline + (0.15 * landzone))) {
					islandPolished[i][j] = '.';
				}
				// Plains/Grass
				else if (islandRough[i][j] >= (waterline + (0.15 * landzone)) && islandRough[i][j] < (waterline + (0.40 * landzone))) {
					islandPolished[i][j] = '-';
				}
				// Forests
				else if (islandRough[i][j] >= (waterline + (0.40 * landzone)) && islandRough[i][j] < (waterline + (0.80 * landzone))) {
					islandPolished[i][j] = '*';
				}
				// Mountains
				else {
					islandPolished[i][j] = '^';
				}
			}
		}
	}

	// Print and output polished island
	cout << endl << endl << endl;
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			myfile << islandPolished[i][j] << "\t";
			cout << islandPolished[i][j] << "\t";
		}
		myfile << endl;
		cout << endl;
	}

	// Close file and free memory
	myfile.close();
	free (islandPolished);
	free (islandRough);
	return 0;
}

int** makeParticleMap(int width, int height, int minX, int maxX, int minY, int maxY, int numParticles, int maxLife) {
	// Create dynamic array
	int** array;
	array = new int*[height];
	for(int i = 0; i < height; i++) {
		array[i] = new int[width];
	} 

	// Fill dynamic array with 0's
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			array[i][j] = 0;
		}
	}

	// Output map and window range
	cout << endl;
	cout << "Map Range: X = 0 - " << width - 1 << ", Y = 0 - " << height - 1 << endl;
	cout << "Window Range: X = 0 - " << maxX - 1 << ", Y = 0 - " << maxY - 1 << endl;

	// For Loop for number of particles
	for (int iParticles = 0; iParticles < numParticles; iParticles++) {
		
		// Generate random X and Y to drop particle
		int particleX = rand() % (maxX-minX + 1) + minX;
		particleX = particleX - 1;
		int particleY = rand() % (maxY-minY + 1) + minY;
		particleY = particleY - 1;

		// Drop location of particle
		// cout << "Particle " << iParticles + 1 << " dropped at: X = " << particleX << ", Y = " << particleY << endl;
		array[particleY][particleX] = array[particleY][particleX] + 1;

		// For Loop for life of particle
		for (int iLife = 0; iLife < maxLife; iLife++) {
			// cout << "Life: " << iLife + 1 << endl;
			if (moveExists(array, width, height, particleX, particleY) == true) {
				int moves[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
				bool loop = true;
				while (loop == true) {
					int randMovesIndex = rand() % 8;
					int moveX = particleX + moves[randMovesIndex][0];
					int moveY = particleY + moves[randMovesIndex][1];
					if (moveX >= 0 && moveY >= 0 && moveX < width && moveY < height) {
						if (array[moveY][moveX] <= array[particleY][particleX]) {
							particleX = moveX;
							particleY = moveY;
							array[particleY][particleX] = array[particleY][particleX] + 1;
							// cout << "Next Move: " << moveX << ", " << moveY << endl;
							loop = false;
						}
					}
				}
			} else {
				// cout << "No available spaces" << endl;
			}
		}
	}

	return array;
}

bool moveExists(int** map, int width, int height, int x, int y) {
	// 8 possible moves
	int moves[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
	// Check for possible move and return true if it finds at least one
	for (int i = 0; i < 8; i++) {
		int moveX = x + moves[i][0];
		int moveY = y + moves[i][1];
		if (moveX >= 0 && moveY >= 0 && moveX < width && moveY < height) {
			if (map[moveY][moveX] <= map[y][x]) {
				return true;
			}
		}
	}
	return false;
}

int findMax(int** map, int width, int height) {
	int maxValue = 0;
	// Loop through 2d array, replacing current maxValue with new if it finds a replacement
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (map[i][j] > maxValue) {
				maxValue = map[i][j];
			}
		}
	}
	return maxValue;
}

void normalizeMap(int** map, int width, int height, int maxVal) {
	// Loop through map and normalize it by dividing each value by the maximum and multiplying by 255
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			map[i][j] = (map[i][j] / (double) maxVal) * 255;
		}
	}
}