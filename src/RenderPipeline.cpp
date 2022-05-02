#include "RenderPipeline.h"
#include "MeshRenderer.h"
#include "GameObject.h"

RenderPipeline::RenderPipeline(WindowManager* wm)
{
    windowManager = wm;
    rm = rm->getInstance();
}

std::vector<GameObject *> RenderPipeline::viewFrustumCull(std::vector<GameObject*> objectsToRender, Camera* cam)
{
	glm::vec4 left, right, bottom, top, near, far;
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

    near.x = composite[0][3] + composite[0][2]; 
    near.y = composite[1][3] + composite[1][2]; 
    near.z = composite[2][3] + composite[2][2];
    near.w = composite[3][3] + composite[3][2];	
    normal = glm::vec3(near.x, near.y, near.z);
    l = length(normal);
    planes[4] = near = near/l;

    far.x = composite[0][3] - composite[0][2];
    far.y = composite[1][3] - composite[1][2]; 
    far.z = composite[2][3] - composite[2][2];
    far.w = composite[3][3] - composite[3][2];
    normal = glm::vec3(far.x, far.y, far.z);
    l = length(normal);
    planes[5] = far = far/l;

    std::vector<GameObject *> objectsToDraw;
    for (int i = 0; i < objectsToRender.size(); i++)
    {
        for (int j = 0; j < 6; j++)
        {
            float radius;
            glm::vec3 center = objectsToRender[i]->getComponentByType<MeshRenderer>()->mesh->getBSphere(&radius);
            center += objectsToRender[i]->transform.position;
            float max_scale = glm::max(objectsToRender[i]->transform.scale.x, glm::max(objectsToRender[i]->transform.scale.y, objectsToRender[i]->transform.scale.z));
            radius *= max_scale;
            float dist = planes[i].x*center.x + planes[i].y*center.y + planes[i].z*center.z + planes[i].w;
            if(dist >= (-1)*radius)
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


