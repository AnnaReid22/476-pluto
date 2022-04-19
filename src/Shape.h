#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

class Program;

class Shape
{
public:
	Shape();//no
	virtual ~Shape();//no
	void loadMesh(const std::string& meshName, std::string* mtlName = NULL, unsigned char* (loadimage)(char const*, int*, int*, int*, int) = NULL);
	void resize();//no
	void createShape(tinyobj::shape_t & shape);//no
	void init();
	void measure();//no
	glm::vec3 getBSphere(float* radius);
	void draw(const std::shared_ptr<Program> prog) const;
	glm::vec3 min;
	glm::vec3 max;
	unsigned int* textureIDs = NULL;
	
private:

	int obj_count = 0;
	std::vector<unsigned int>* eleBuf = NULL;
	std::vector<float>* posBuf = NULL;
	std::vector<float>* norBuf = NULL;
	std::vector<float>* texBuf = NULL;
	unsigned int* materialIDs = NULL;


	unsigned int* eleBufID = 0;
	unsigned int* posBufID = 0;
	unsigned int* norBufID = 0;
	unsigned int* texBufID = 0;
	unsigned int* vaoID = 0;
// 	std::vector<unsigned int> eleBuf;
// 	std::vector<float> posBuf;
// 	std::vector<float> norBuf;
// 	std::vector<float> texBuf;
// 	unsigned eleBufID;
// 	unsigned posBufID;
// 	unsigned norBufID;
// 	unsigned texBufID;
//    unsigned vaoID;
};

#endif
