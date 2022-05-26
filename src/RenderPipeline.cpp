#include "RenderPipeline.h"
#include "MeshRenderer.h"
#include "GameObject.h"

RenderPipeline::RenderPipeline(WindowManager* wm)
{
    windowManager = wm;
    rm = rm->getInstance();
}

std::vector<GameObject*> RenderPipeline::viewFrustumCull(std::vector<GameObject*> objectsToRender, Camera* cam)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 PV = glm::perspective(glm::radians(50.0f), aspect, 0.01f, 1000.0f) * cam->getCameraViewMatrix();// cam->getCameraProjectionMatrix() * cam->getCameraViewMatrix();
     
    glm::vec4 left =   glm::vec4(PV[0][3] + PV[0][0], PV[1][3] + PV[1][0], PV[2][3] + PV[2][0], PV[3][3] + PV[3][0]);
    glm::vec4 right =  glm::vec4(PV[0][3] - PV[0][0], PV[1][3] - PV[1][0], PV[2][3] - PV[2][0], PV[3][3] - PV[3][0]);
    glm::vec4 bottom = glm::vec4(PV[0][3] + PV[0][1], PV[1][3] + PV[1][1], PV[2][3] + PV[2][1], PV[3][3] + PV[3][1]);
    glm::vec4 top =    glm::vec4(PV[0][3] - PV[0][1], PV[1][3] - PV[1][1], PV[2][3] - PV[2][1], PV[3][3] - PV[3][1]);
    glm::vec4 near_ =  glm::vec4(PV[0][3] + PV[0][2], PV[1][3] + PV[1][2], PV[2][3] + PV[2][2], PV[3][3] + PV[3][2]);
    glm::vec4 far_ =   glm::vec4(PV[0][3] - PV[0][2], PV[1][3] - PV[1][2], PV[2][3] - PV[2][2], PV[3][3] - PV[3][2]);

    left /= length(glm::vec3(left.x, left.y, left.z));
    right /= length(glm::vec3(right.x, right.y, right.z));
    bottom /= length(glm::vec3(bottom.x, bottom.y, bottom.z));
    top /= length(glm::vec3(top.x, top.y, top.z));
    near_ /= length(glm::vec3(near_.x, near_.y, near_.z));
    far_ /= length(glm::vec3(far_.x, far_.y, far_.z));

    glm::vec4 planes[6] = { left, right, bottom, top, near_, far_ };

    std::vector<GameObject*> objectsToDraw;
    for (int i = 0; i < objectsToRender.size(); i++)
    {
        // Ensure all parts of the rocket are rendered
        if (objectsToRender[i]->name == "rocketBody" || objectsToRender[i]->name == "fin1" || objectsToRender[i]->name == "fin2" || objectsToRender[i]->name == "fin3")
        {
            objectsToDraw.push_back(objectsToRender[i]);
        }
        else
        {
            std::shared_ptr<Shape> mesh = objectsToRender[i]->getComponentByType<MeshRenderer>()->mesh;
            glm::vec3 center = objectsToRender[i]->transform.position + mesh->getCenter();
            float radius = mesh->getRadius() * (glm::max)(objectsToRender[i]->transform.scale.x, (glm::max)(objectsToRender[i]->transform.scale.y, objectsToRender[i]->transform.scale.z));
            //std::cout << "center: " << objectsToRender[i]->name << " " << center.x << ", " << center.y << ", " << center.z << std::endl;
            bool cullable = false;
            for (int j = 0; j < 6; j++)
            {
                float dist = planes[j].x * center.x + planes[j].y * center.y + planes[j].z * center.z + planes[j].w;
                if (dist + radius < 0)
                {
                    cullable = true;
                    break;
                }
            }
            if (!cullable) {
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


