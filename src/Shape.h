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
		void resize(); //also centeres
		void shift();
		void createShape(tinyobj::shape_t& shape);//no
		void init(bool norMap = false);
		glm::vec3 getBSphere(float* radius);
		void draw(const std::shared_ptr<Program> prog, bool norMap = false) const;
		unsigned int* textureIDs = NULL;
		glm::vec3 getCenter() { return (this->max + this->min) * 0.5f; }
		float getRadius() { return glm::length(this->max - this->min) * 0.5f; }
		void ComputeTanBN();


private:

		glm::vec3 min;
		glm::vec3 max;

		int obj_count = 0;
		std::vector<unsigned int>* eleBuf = NULL;
		std::vector<float>* posBuf = NULL;
		std::vector<float>* norBuf = NULL;
		std::vector<float>* texBuf = NULL;
		std::vector<float>* tanBuf = NULL;
		std::vector<float>* BNBuf = NULL;
		unsigned int* materialIDs = NULL;


		unsigned int* eleBufID = 0;
		unsigned int* posBufID = 0;
		unsigned int* norBufID = 0;
		unsigned int* texBufID = 0;
		unsigned int* tanBufID = 0;
		unsigned int* BNBufID = 0;
		unsigned int* vaoID = 0;
};

#endif
