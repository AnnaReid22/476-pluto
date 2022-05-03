#include "RenderPipeline.h"
#include "MeshRenderer.h"
#include "GameObject.h"


float max_(float a, float b, float c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

RenderPipeline::RenderPipeline(WindowManager* wm)
{
    windowManager = wm;
    rm = rm->getInstance();
}

std::vector<GameObject *> RenderPipeline::viewFrustumCull(std::vector<GameObject*> objectsToRender, Camera* cam)
{
	glm::vec4 left, right, bottom, top, near_, far_;
	glm::vec4 planes[6];

    glm::mat4 view = cam->getCameraViewMatrix();
    glm::mat4 projection = cam->getCameraProjectionMatrix();

    glm::mat4 composite = projection * view;
    glm::vec3 normal;
    float l;

    left.x = composite[0][3] + composite[0][0];
    left.y = composite[1][3] + composite[1][0];
    left.z = composite[2][3] + composite[2][0];
    left.w = composite[3][3] + composite[3][0];
    normal = glm::vec3(left.x, left.y, left.z);
    l = length(normal);
    planes[0] = left = left/l;

    right.x = composite[0][3] - composite[0][0];
    right.y = composite[1][3] - composite[1][0];
    right.z = composite[2][3] - composite[2][0];
    right.w = composite[3][3] - composite[3][0];		
    normal = glm::vec3(right.x, right.y, right.z);
    l = length(normal);
    planes[1] = right = right/l;

    bottom.x = composite[0][3] + composite[0][1];
    bottom.y = composite[1][3] + composite[1][1];
    bottom.z = composite[2][3] + composite[2][1];
    bottom.w = composite[3][3] + composite[3][1];
    normal = glm::vec3(bottom.x, bottom.y, bottom.z);
    l = length(normal);
    planes[2] = bottom = bottom/l;

    top.x = composite[0][3] - composite[0][1]; 
    top.y = composite[1][3] - composite[1][1];
    top.z = composite[2][3] - composite[2][1];
    top.w = composite[3][3] - composite[3][1];	
    normal = glm::vec3(top.x, top.y, top.z);
    l = length(normal);
    planes[3] = top = top/l;

    near_.x = composite[0][3] + composite[0][2]; 
    near_.y = composite[1][3] + composite[1][2]; 
    near_.z = composite[2][3] + composite[2][2];
    near_.w = composite[3][3] + composite[3][2];	
    normal = glm::vec3(near_.x, near_.y, near_.z);
    l = length(normal);
    planes[4] = near_ = near_/l;

    far_.x = composite[0][3] - composite[0][2];
    far_.y = composite[1][3] - composite[1][2]; 
    far_.z = composite[2][3] - composite[2][2];
    far_.w = composite[3][3] - composite[3][2];
    normal = glm::vec3(far_.x, far_.y, far_.z);
    l = length(normal);
    planes[5] = far_ = far_/l;

    std::vector<GameObject *> objectsToDraw;
    for (int i = 0; i < objectsToRender.size(); i++)
    {
        for (int j = 0; j < 6; j++)
        {
            std::shared_ptr<Shape> mesh = objectsToRender[i]->getComponentByType<MeshRenderer>()->mesh;
            glm::vec3 center = mesh->center;
            std::cout << "centerx" << center.x << std::endl;
            float rad = mesh->radius;
            std::cout << "rad" << rad << std::endl;
            center += objectsToRender[i]->transform.position;
            float max_scale = max_(objectsToRender[i]->transform.scale.x, objectsToRender[i]->transform.scale.y, objectsToRender[i]->transform.scale.z);
            rad *= max_scale;
            float dist = planes[i].x*center.x + planes[i].y*center.y + planes[i].z*center.z + planes[i].w;
            if(dist >= (-1)*rad)
            {
                objectsToDraw.push_back(objectsToRender[i]);
            }
        }
    }
    return objectsToDraw;
}


void RenderPipeline::addRenderPass(std::shared_ptr<IShader> pass)
{
    renderPasses.push_back(pass);
    pass->init();
}

void RenderPipeline::executePipeline()
{
    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");
    renderables = viewFrustumCull(renderables, cam);

    rm->addOther("renderables", &renderables);

    for (std::shared_ptr<IShader> pass : renderPasses)
        pass->execute(windowManager);

    //simple render pass that will combine all the textures
    //outputPass->execute(windowManager);

    return;
}


