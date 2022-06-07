#include "Shape.h"
#include <iostream>
#include <assert.h>

#include "GLSL.h"
#include "Program.h"

using namespace std;

Shape::Shape() :
    eleBufID(0),
    posBufID(0),
    norBufID(0),
    texBufID(0),
    vaoID(0)
{
    min = glm::vec3(0);
    max = glm::vec3(0);
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string& meshName, string* mtlpath, unsigned char* (loadimage)(char const*, int*, int*, int*, int)) {
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> objMaterials;
    string errStr;
    bool rc = false;
    if (mtlpath)
        rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str(), mtlpath->c_str());
    else
        rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());


    if (!rc) {
        cerr << errStr << endl;
    }
    else if (shapes.size()) {
        obj_count = shapes.size();
        posBuf = new std::vector<float>[shapes.size()];
        norBuf = new std::vector<float>[shapes.size()];
        texBuf = new std::vector<float>[shapes.size()];
        tanBuf = new std::vector<float>[shapes.size()];
        BNBuf = new std::vector<float>[shapes.size()];
        eleBuf = new std::vector<unsigned int>[shapes.size()];

        eleBufID = new unsigned int[shapes.size()];
        posBufID = new unsigned int[shapes.size()];
        norBufID = new unsigned int[shapes.size()];
        texBufID = new unsigned int[shapes.size()];
        tanBufID = new unsigned int[shapes.size()];
        BNBufID = new unsigned int[shapes.size()];

        vaoID = new unsigned int[shapes.size()];
        materialIDs = new unsigned int[shapes.size()];

        textureIDs = new unsigned int[shapes.size()];

        glm::vec3 scale = glm::vec3(1);
        glm::vec3 shift = glm::vec3(0);
        this->max = glm::vec3(-3.4028234664e+38);
        this->min = glm::vec3(3.4028234664e+38);

        for (int i = 0; i < obj_count; i++) {
            //load textures
            textureIDs[i] = 0;
            //texture sky
            posBuf[i] = shapes[i].mesh.positions;
            norBuf[i] = shapes[i].mesh.normals;
            texBuf[i] = shapes[i].mesh.texcoords;
            eleBuf[i] = shapes[i].mesh.indices;
            if (shapes[i].mesh.material_ids.size() > 0)
                materialIDs[i] = shapes[i].mesh.material_ids[0];
            else
                materialIDs[i] = -1;

            for (int v = 0; v < posBuf[i].size() / 3; v++) {
                float x = posBuf[i][3 * v + 0];
                float y = posBuf[i][3 * v + 1];
                float z = posBuf[i][3 * v + 2];

                if (x < this->min.x) this->min.x = x;
                if (x > this->max.x) this->max.x = x;
                if (y < this->min.y) this->min.y = y;
                if (y > this->max.y) this->max.y = y;
                if (z < this->min.z) this->min.z = z;
                if (z > this->max.z) this->max.z = z;
            }
        }
    }
    //material:
    for (int i = 0; i < objMaterials.size(); i++)
        if (objMaterials[i].diffuse_texname.size() > 0) {
            char filepath[1000];
            int width, height, channels;
            string filename = objMaterials[i].ambient_texname;
            int subdir = filename.rfind("\\");
            if (subdir > 0)
                filename = filename.substr(subdir + 1);
            string str = *mtlpath + filename;
            strcpy(filepath, str.c_str());
            //stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)

            unsigned char* data = loadimage(filepath, &width, &height, &channels, 4);
            glGenTextures(1, &textureIDs[i]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            //delete[] data;
        }
}

glm::vec3 Shape::getBSphere(float* radius)
{
    *radius = this->getRadius();
    return this->getCenter();
}

void Shape::shift() {
    glm::vec3 center = this->getCenter();
    for (int i = 0; i < obj_count; i++) {
        for (size_t v = 0; v < posBuf[i].size() / 3; v++) {
            posBuf[i][3 * v + 0] = (posBuf[i][3 * v + 0] - center.x);
            posBuf[i][3 * v + 1] = (posBuf[i][3 * v + 1] - center.y);
            posBuf[i][3 * v + 2] = (posBuf[i][3 * v + 2] - center.z);
        }
    }
}
void Shape::resize() {
    glm::vec3 center = this->getCenter();
    float radius = this->getRadius();
    this->max -= center;
    this->min -= center;

    for (int i = 0; i < obj_count; i++) {
        for (size_t v = 0; v < posBuf[i].size() / 3; v++) {
            posBuf[i][3 * v + 0] = (posBuf[i][3 * v + 0] - center.x) / radius;
            posBuf[i][3 * v + 1] = (posBuf[i][3 * v + 1] - center.y) / radius;
            posBuf[i][3 * v + 2] = (posBuf[i][3 * v + 2] - center.z) / radius;
        }
    }
}

void Shape::ComputeTanBN() {
    int idx0, idx1, idx2;
    glm::vec3 v0, v1, v2;
    glm::vec2 t0, t1, t2;
    glm::vec3 e0, e1;
    glm::vec2 texE0, texE1;
    float weight;
    glm::vec3 Tan, biTan;

    for (int i = 0; i < obj_count; i++) {
        //bootstrap for every vertex create a tangent and biTangent
        for (size_t n = 0; n < posBuf[i].size(); n++) {
            tanBuf[i].push_back(0);
            BNBuf[i].push_back(0);
        }

        for (size_t n = 0; n < (*eleBuf).size() / 3; n++) {
            idx0 = (*eleBuf)[n * 3];
            idx1 = (*eleBuf)[n * 3 + 1];
            idx2 = (*eleBuf)[n * 3 + 2];
            v0 = glm::vec3(posBuf[i][idx0 * 3 + 0], posBuf[i][idx0 * 3 + 1], posBuf[i][idx0 * 3 + 2]);
            v1 = glm::vec3(posBuf[i][idx1 * 3 + 0], posBuf[i][idx1 * 3 + 1], posBuf[i][idx1 * 3 + 2]);
            v2 = glm::vec3(posBuf[i][idx2 * 3 + 0], posBuf[i][idx2 * 3 + 1], posBuf[i][idx2 * 3 + 2]);
            t0 = glm::vec2(texBuf[i][idx0 * 2 + 0], texBuf[i][idx0 * 2 + 1]);
            t1 = glm::vec2(texBuf[i][idx1 * 2 + 0], texBuf[i][idx1 * 2 + 1]);
            t2 = glm::vec2(texBuf[i][idx2 * 2 + 0], texBuf[i][idx2 * 2 + 1]);
            e0 = v1 - v0;
            e1 = v2 - v0;
            texE0 = t1 - t0;
            texE1 = t2 - t0;
            weight = 1.0f / (texE0.x * texE1.y - texE0.y * texE1.x);
            Tan = (e0 * texE1.y - e1 * texE0.y) * weight;
            biTan = (e1 * texE0.x - e0 * texE1.x) * weight;
            //set the tangent and biTangent for each vertex
            tanBuf[i][idx0 * 3] = Tan.x;
            tanBuf[i][idx0 * 3 + 1] = Tan.y;
            tanBuf[i][idx0 * 3 + 2] = Tan.z;
            BNBuf[i][idx0 * 3] = biTan.x;
            BNBuf[i][idx0 * 3 + 1] = biTan.y;
            BNBuf[i][idx0 * 3 + 2] = biTan.z;
            tanBuf[i][idx1 * 3] = Tan.x;
            tanBuf[i][idx1 * 3 + 1] = Tan.y;
            tanBuf[i][idx1 * 3 + 2] = Tan.z;
            BNBuf[i][idx1 * 3] = biTan.x;
            BNBuf[i][idx1 * 3 + 1] = biTan.y;
            BNBuf[i][idx1 * 3 + 2] = biTan.z;
            tanBuf[i][idx2 * 3] = Tan.x;
            tanBuf[i][idx2 * 3 + 1] = Tan.y;
            tanBuf[i][idx2 * 3 + 2] = Tan.z;
            BNBuf[i][idx2 * 3] = biTan.x;
            BNBuf[i][idx2 * 3 + 1] = biTan.y;
            BNBuf[i][idx2 * 3 + 2] = biTan.z;
        }
    }
}


void Shape::init(bool norMap) {
    for (int i = 0; i < obj_count; i++) {


        // Initialize the vertex array object
        glGenVertexArrays(1, &vaoID[i]);
        glBindVertexArray(vaoID[i]);

        // Send the position array to the GPU
        glGenBuffers(1, &posBufID[i]);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
        glBufferData(GL_ARRAY_BUFFER, posBuf[i].size() * sizeof(float), posBuf[i].data(), GL_STATIC_DRAW);

        // Send the normal array to the GPU
        if (norBuf[i].empty()) {
            norBufID[i] = 0;
        }
        else {
            glGenBuffers(1, &norBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, norBuf[i].size() * sizeof(float), norBuf[i].data(), GL_STATIC_DRAW);
        }

        // Send the texture array to the GPU
        if (texBuf[i].empty()) {
            texBufID[i] = 0;
        }
        else {
            glGenBuffers(1, &texBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, texBuf[i].size() * sizeof(float), texBuf[i].data(), GL_STATIC_DRAW);
        }

        if (norMap) {
            ComputeTanBN();
            glGenBuffers(1, &tanBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, tanBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, tanBuf[i].size() * sizeof(float), tanBuf[i].data(), GL_STATIC_DRAW);
            glGenBuffers(1, &BNBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, BNBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, BNBuf[i].size() * sizeof(float), BNBuf[i].data(), GL_STATIC_DRAW);
        }

        // Send the element array to the GPU
        glGenBuffers(1, &eleBufID[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf[i].size() * sizeof(unsigned int), eleBuf[i].data(), GL_STATIC_DRAW);

        // Unbind the arrays
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        assert(glGetError() == GL_NO_ERROR);
    }
}

void Shape::draw(const shared_ptr<Program> prog, bool norMap) const {
    for (int i = 0; i < obj_count; i++) {
        int h_pos, h_nor, h_tex;
        h_pos = h_nor = h_tex = -1;

        glBindVertexArray(vaoID[i]);
        // Bind position buffer
        h_pos = prog->getAttribute("vertPos");
        GLSL::enableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

        // Bind normal buffer
        h_nor = prog->getAttribute("vertNor");
        if (h_nor != -1 && norBufID[i] != 0) {
            GLSL::enableVertexAttribArray(h_nor);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
            glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
        }

        if (texBufID[i] != 0) {
            // Bind texcoords buffer
            h_tex = prog->getAttribute("vertTex");
            if (h_tex != -1 && texBufID[i] != 0) {
                GLSL::enableVertexAttribArray(h_tex);
                glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
                glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
            }
        }

        int h_tan = -1, h_bn = -1;
        if (norMap) {
            h_tan = prog->getAttribute("vertTan");
            if (h_tan != -1 && tanBufID != 0) {
                GLSL::enableVertexAttribArray(h_tan);
                glBindBuffer(GL_ARRAY_BUFFER, tanBufID[i]);
                glVertexAttribPointer(h_tan, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
            }
            h_bn = prog->getAttribute("vertBN");
            if (h_bn != -1 && BNBufID != 0) {
                GLSL::enableVertexAttribArray(h_bn);
                glBindBuffer(GL_ARRAY_BUFFER, BNBufID[i]);
                glVertexAttribPointer(h_bn, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
            }
        }

        // Bind element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);

        //texture


        int textureindex = materialIDs[i];
        if (textureindex >= 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[textureindex]);
        }
        // Draw
        glDrawElements(GL_TRIANGLES, (int)eleBuf[i].size(), GL_UNSIGNED_INT, (const void*)0);

        // Disable and unbind
        if (h_tex != -1) {
            GLSL::disableVertexAttribArray(h_tex);
        }
        if (h_nor != -1) {
            GLSL::disableVertexAttribArray(h_nor);
        }
        if (h_tan != -1) {
            GLSL::disableVertexAttribArray(h_tan);
            GLSL::disableVertexAttribArray(h_bn);
        }
        GLSL::disableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}