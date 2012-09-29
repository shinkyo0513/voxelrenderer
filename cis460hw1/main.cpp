/**
 * An example program that creates a 24-bit bitmap file that is 800 x 600 and makes a blue/red checkerboard pattern.
 * Uses EasyBMP
 *
 * Cory Boatright
 * University of Pennsylvania, Fall 2011
 **/

#include "WriteBMP.h"

bool* WriteBMP::entered = new bool(false);
vec3* WriteBMP::eye = 0;
vec3* WriteBMP::camDirection = 0;
vec3* WriteBMP::upVec = 0;
vec3* WriteBMP::finalColor = 0;
vec3* WriteBMP::bgColor = 0;
vec3* WriteBMP::matColor = 0;
vec3* WriteBMP::ray = 0;
vec3* WriteBMP::lightPos = 0;
vec3* WriteBMP::lightCol = 0;
vec3* WriteBMP::A = 0;
Voxel* WriteBMP::allVoxels = 0;
float* WriteBMP::voxSize = 0;
float* WriteBMP::stepSize = 0;
float* WriteBMP::fovy = 0;
float* WriteBMP::fovx = 0;
float* WriteBMP::numVoxX = 0;
float* WriteBMP::numVoxY = 0;
float* WriteBMP::numVoxZ = 0;
float* WriteBMP::resoX = 0;
float* WriteBMP::resoY = 0;
float* WriteBMP::dens = new float(0.0f);
vec3* WriteBMP::lrPos = new vec3(0.0f);
float* WriteBMP::Q = new float(1.0f);
vec3* WriteBMP::finalLight = new vec3(0.0f);
float* WriteBMP::lightDens = new float(0.0f);
vec3* WriteBMP::origin = 0;

int main(int argc, char** argv) {

	ifstream config;
	config.open(argv[1]);
	string reader;
	
	float temp;

	getline(config, reader);
	static_cast<istringstream>(reader.substr(5, reader.size() - 1)) >> temp;
	WriteBMP::voxSize = new float(temp);

	getline(config, reader);
	static_cast<istringstream>(reader.substr(5, reader.size() - 1)) >> temp;
	WriteBMP::stepSize = new float(temp);

	getline(config, reader);
	WriteBMP::origin = WriteBMP::getVec(reader);

	getline(config, reader);	
	vector<int>* numVox = WriteBMP::getInts(reader);
	WriteBMP::numVoxX = new float((*numVox)[0]);
	WriteBMP::numVoxY = new float((*numVox)[1]);
	WriteBMP::numVoxZ = new float((*numVox)[2]);
	numVox->clear();
	delete numVox;

	getline(config, reader);
	WriteBMP::bgColor = WriteBMP::getVec(reader);
	
	getline(config, reader);
	WriteBMP::matColor = WriteBMP::getVec(reader);

	getline(config, reader);
	string filename = reader.substr(5, reader.size() - 1);

	getline(config, reader);
	vector<int>* resolution = WriteBMP::getInts(reader);
	WriteBMP::resoX = new float((*resolution)[0]);
	WriteBMP::resoY = new float((*resolution)[1]);
	resolution->clear();
	delete resolution;

	getline(config, reader);
	WriteBMP::eye = WriteBMP::getVec(reader);

	getline(config, reader);
	WriteBMP::camDirection = WriteBMP::getVec(reader);

	getline(config, reader);
	WriteBMP::upVec = WriteBMP::getVec(reader);

	getline(config, reader);
	static_cast<istringstream>(reader.substr(5, reader.size() - 1)) >> temp;
	WriteBMP::fovy = new float(temp);

	getline(config, reader);
	WriteBMP::lightPos = WriteBMP::getVec(reader);

	getline(config, reader);
	WriteBMP::lightCol = WriteBMP::getVec(reader);

	vec3 A = cross(*WriteBMP::camDirection, *WriteBMP::upVec);
	vec3 B = cross(A, *WriteBMP::camDirection);
	vec3 M = *WriteBMP::camDirection + *WriteBMP::eye;
	vec3 V = (B * length(*WriteBMP::camDirection) * tan(radians(*WriteBMP::fovy)))/ length(B);
	//float fovx = atan(length(V) * (*WriteBMP::resoX/ *WriteBMP::resoY)/length(M));
	vec3 H = V * (*WriteBMP::resoX/ *WriteBMP::resoY);
	H.x = H.y;
	H.y = 0.0f;
	H.z = 0.0f;
	//((A * length(*WriteBMP::camDirection) * (length(V) * (*WriteBMP::resoX/ *WriteBMP::resoY)/length(M)))/ length(A));

	WriteBMP::allVoxels = new Voxel[(*WriteBMP::numVoxX * *WriteBMP::numVoxY * *WriteBMP::numVoxZ)];
	Voxel* vox;
	float tempDens;
	int voxInd = 0;
	while (getline(config, reader)) {
		static_cast<istringstream>(reader) >> tempDens;
		vox = new Voxel();
		*vox->density = tempDens;
		WriteBMP::allVoxels[voxInd] = *vox;
		//delete vox;
		voxInd++;
	}

	unsigned int width = static_cast<int>(*WriteBMP::resoX);
	unsigned int height = static_cast<int>(*WriteBMP::resoY);
	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);
	vec3 P;
	vec3* E = new vec3(WriteBMP::eye->x, WriteBMP::eye->y, WriteBMP::eye->z);
	vec3* color;
	cout << "Beginning ray march" << endl;
	for(unsigned int x = 0; x < width; x++) {
		for(unsigned int y = 0; y < height; y++) {
			E->x = WriteBMP::eye->x;
			E->y = WriteBMP::eye->y;
			E->z = WriteBMP::eye->z;
			P = WriteBMP::mapPoint(x, height - y - 1, M, H, V);
			if (!WriteBMP::intersectsGrid(*E, P - *E)) {
				output(x, y)->Red = 255 * WriteBMP::bgColor->x;
				output(x, y)->Green = 255 * WriteBMP::bgColor->y;
				output(x, y)->Blue = 255 * WriteBMP::bgColor->z;
				continue;
			}
			color = WriteBMP::rayMarch(E, normalize(P - *E), 1.0f);
			*WriteBMP::entered = false;
			output(x, y)->Red = 255 * color->x;
			output(x, y)->Green = 255 * color->y;
			output(x, y)->Blue = 255 * color->z;
			delete color;
			color = 0;
		}
		cout << "finished scanline: " << x << endl;
	}
	output.WriteToFile(filename.c_str()); 
	delete [] WriteBMP::allVoxels;
	return 0;
}