#include "GameObject.h"
#include "Player.h"
#include "ParticleSystem.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "PhysicsObject.h"
#include "World.h"
#include <iostream>
#define PI 3.14159265
#define MAXDOLLYFTIME 1.3

ResourceManager* rm_ = ResourceManager::getInstance();

Player::Player(GameObject* d_GameObject) : Component(d_GameObject)
{
    // Has rocket collided with planet?
    stop = false;

    // Move forward and backwards
    dollyB = false;
    dollyF = false;
    prevDollyF = false;
    dollyFTime = 0;
    stopTime = 0;

    // Speed for any of the above movements
    speed = -7.0f;

    // Rocket rotation sensitivity
    sensitivity = 0.1;

    // Change in mouse position - used to calculate rocket rotation
    deltaXPos = deltaYPos = 0; 

    // Whether the initial mouse position has been set
    initMousePosSet = false;

    // Initial and current X and Y mouse positions
    initXPos = initYPos = currentXPos = currentYPos = 0;

    // Initial rocket rotation. A rotation in x is a rotation about the x-axis
    rotation = glm::vec3(-PI / 2.0f, 0, 0);

    // Instance of InputManager
    input = input->getInstance();

    // Store the original transform information
    originalScale = glm::vec3(1.0f, 1.0f, 1.0f);

    //std::cout << "Original Transform: " << originalTransform.x << ", " << originalTransform.y << ", " << originalTransform.z << std::endl;

}

/*
* Sets initial rocket position
*/
void Player::Start()
{
    this->gameObject->transform.position = glm::vec3(0, 0, -4);
    this->gameObject->transform.scale = originalScale;
    //fin1->transform.scale = glm::vec3(0.4, 0.4, 0.4);
}

/*
* Calls functions that move and rotate rocket
*/
void Player::Update()
{
    updateMoveVars();
    if (!stop)
    {
        moveRocket();
    }

}


/*
* Rocket stops moving when it collides with a planet
*/
void Player::OnCollide(GameObject* other) 
{
    if (other->tag == "planet" && other->name != "pluto" || other->name == "asteroid")
    {
        stop = true;
        MeshRenderer* rocket_mesh = this->gameObject->getComponentByType<MeshRenderer>();
        rocket_mesh->Disable();
        
        GameObject* ps = (GameObject*)rm_->getOther("particle_system_death");
        
        ps->transform = this->gameObject->transform;
        ps->Enable();
        ParticleSystem* ps_obj = ps->getComponentByType<ParticleSystem>();
        ps_obj->start = this->gameObject->transform.position;

        std::cout << "You crashed :(" << std::endl;
    }
    if (other->name == "pluto")
    {
        stop = true;
        
        GameObject* ps1 = (GameObject*)rm_->getOther("particle_system_c1");
        ps1->transform = this->gameObject->transform;
        ps1->Enable();
        ParticleSystem* ps_obj1 = ps1->getComponentByType<ParticleSystem>();
        ps_obj1->start = this->gameObject->transform.position + vec3(1.5f, 0.0f, 0.0f);

        GameObject* ps2 = (GameObject*)rm_->getOther("particle_system_c2");
        ps2->transform = this->gameObject->transform;
        ps2->Enable();
        ParticleSystem* ps_obj2 = ps2->getComponentByType<ParticleSystem>();
        ps_obj2->start = this->gameObject->transform.position + vec3(-1.5f, 0.0f, 0.0f);

        GameObject* ps3 = (GameObject*)rm_->getOther("particle_system_c3");
        ps3->transform = this->gameObject->transform;
        ps3->Enable();
        ParticleSystem* ps_obj3 = ps3->getComponentByType<ParticleSystem>();
        ps_obj3->start = this->gameObject->transform.position + vec3(0.0f, 1.0f, 0.0f);

        GameObject* ps4 = (GameObject*)rm_->getOther("particle_system_c4");
        ps4->transform = this->gameObject->transform;
        ps4->Enable();
        ParticleSystem* ps_obj4 = ps4->getComponentByType<ParticleSystem>();
        ps_obj4->start = this->gameObject->transform.position + vec3(0.7f, -1.0f, 0.0f);

        GameObject* ps5 = (GameObject*)rm_->getOther("particle_system_c5");
        ps5->transform = this->gameObject->transform;
        ps5->Enable();
        ParticleSystem* ps_obj5 = ps5->getComponentByType<ParticleSystem>();
        ps_obj5->start = this->gameObject->transform.position + vec3(-0.7f, -1.0f, 0.0f);

        std::cout << "YOU WON!!" << std::endl;
    }
    //stop = false;
}


/*
* Updates variables used for rocket movement and rotation
*/
void Player::updateMoveVars()
{
    double xpos, ypos;
    
    input->GetMousePos(&xpos, &ypos);

    // Initialize mouse position 
    if (!initMousePosSet)
    {
        initMousePosSet = true;
        initXPos = xpos;
        initYPos = ypos;
    }

    // Update current mouse position
    currentXPos = xpos;
    currentYPos = ypos; 

    // Change in mouse position since last call
    deltaXPos = currentXPos - initXPos;
    deltaYPos = currentYPos - initYPos;

    // Update previous mouse position
    initXPos = currentXPos;
    initYPos = currentYPos;
    
    // Update movement variables

    dollyF = input->GetKey(GLFW_KEY_W);
    Time* time = time->getInstance();
    if (dollyF)// && dollyFTime < MAXDOLLYFTIME)
    {
        dollyFTime += time->getFrametime();
    }
    else if(!dollyF)
    {
        if (prevDollyF)
        {
            stopTime = 1.3;
        }
        else if (stopTime > 0)
        {
            stopTime -= time->getFrametime();
        }
        dollyFTime = 0;

    }
    prevDollyF = dollyF;
    //dollyB = input->GetKey(GLFW_KEY_S);
}

/*
* Calculates the rocket's position matrix and rotation quaternion and 
* updates the rocket's transform.
*/
void Player::moveRocket()
{
    Time* time = time->getInstance();
    float frametime = time->getFrametime();

    // Determine direction of rocket movement
    glm::vec3 rocketMove = glm::vec3(0.0f, 0.0f, 0.0f);
   
    float t;
    if (dollyFTime > 0)
    {
        t = min(dollyFTime, MAXDOLLYFTIME);
    }
    else
    {
        t = stopTime;
    }
    //float t = dollyFTime;
    if (t > 1.2)
    {
        t = 1.2;
    }
    float widthChange = 2 - (3 * pow((t - 0.52), 2) + 0.2);

    if (t > 1.033)
    {
        t = 1.033;
    }

    float squashCalculation = 3 * pow((t - 0.52), 2) + 0.2;
    this->gameObject->transform.scale.x = widthChange;
    this->gameObject->transform.scale.z = widthChange;
    this->gameObject->transform.scale.y = squashCalculation;
   // std::cout << this->gameObject->transform.scale.x << ", " << this->gameObject->transform.scale.y << ", " << this->gameObject->transform.scale.z << std::endl;
  

    if (dollyF)
    {
        rocketMove.y += frametime * speed;
        
    }
    if (dollyB)
    {
        rocketMove.y -= frametime * speed;
    }

    // Update rocket rotation about the y-axis
    rotation.y += deltaXPos * frametime * sensitivity;
    
    // Update rocket rotation about the x-axis
    rotation.x += deltaYPos * frametime * sensitivity;
    
    // Limit rotation about the x-axis to prevent camera from flipping
    if (rotation.x > glm::radians(-10.0))
    {
        rotation.x = glm::radians(-10.0);
    }
    if (rotation.x < glm::radians(-170.0))
    {
        rotation.x = glm::radians(-170.0);
    }


    // Used to calculate position
    //glm::mat4 rotMat = glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.f), (float) - PI / 2.0f, glm::vec3(1, 0, 0));
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1, 0, 0));


    // Adjust movement based on the rocket's rotation
    glm::vec4 posUpdate = rotMat * glm::vec4(rocketMove, 0); //glm::vec4(0, -10, 0, 0) if you want constant forward movement
    
    // Store fwd vector of the rocket
    fwd = normalize(rotMat * glm::vec4(0, -1, 0, 0));

    // Update the Rocket's Transform position matrix and rotation quaternion
    this->gameObject->transform.position -= glm::vec3(posUpdate.x, posUpdate.y, posUpdate.z);
    this->gameObject->transform.rotation = glm::quat(1.0, 0.0, 0.0, 0.0);//glm::quat(rotMat); // glm::quat(1.0, 0.0, 0.0, 0.0);


    // Fin animation
    fin1->transform.position = glm::vec3(0.4, -0.78, 0.15);
    fin1->transform.rotation = glm::vec3(0.0f, glm::radians(-130.0f), 0.0f);//glm::radians(10.0f));
    fin1->transform.scale = glm::vec3(0.5, 0.5, 0.5);

    fin2->transform.position = glm::vec3(-0.4, -0.78, 0.15);
    fin2->transform.rotation = glm::vec3(0.0f, glm::radians(130.0f), 0.0f);//glm::radians(-10.0f));
    fin2->transform.scale = glm::vec3(0.5, 0.5, 0.5);

    fin3->transform.position = glm::vec3(0.0, -0.7, -0.4);
    fin3->transform.rotation = glm::vec3(0.0f, glm::radians(0.0f), glm::radians(0.0f));
    fin3->transform.scale = glm::vec3(0.5, 0.5, 0.5);

    if (dollyFTime > 0)
    {
        glm::vec3 finRot = glm::vec3(0, dollyFTime*4, 0.0f);
        std::cout << "dollyFTime: " << dollyFTime << std::endl;
        fin1->transform.hierarchicalRot = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        fin1->transform.position = glm::vec3(0.5, -0.78, 0.20);
        fin2->transform.hierarchicalRot = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        fin2->transform.position = glm::vec3(-0.5, -0.78, 0.20);
        fin3->transform.hierarchicalRot = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        fin3->transform.position = glm::vec3(0.0, -0.7, -0.5);
        //t = dollyFTime;
    }
    

    //fin1->transform.position = glm::vec3(1, 0, 1);
    //std::cout << "fin1: " << fin1 << std::endl;
    //std::cout << "player: " << this->gameObject << std::endl;
    glm::mat4 MPlayer = this->gameObject->transform.genModelMatrix();
    glm::mat4 MFin = this->gameObject->transform.genModelMatrix();
    

   

    bulletCooldown -= Time::getInstance()->getFrametime();
    if (InputManager::getInstance()->GetKey(GLFW_KEY_E) && bulletCooldown <= 0)
    {
        bulletCooldown = 0.5f;
        GameObject* bullet = new GameObject("bullet");
        bullet->transform.position = gameObject->transform.position - this->getForward();
        bullet->transform.scale = gameObject->transform.scale;
        bullet->transform.rotation = gameObject->transform.rotation;
        BoundingSphereCollider* bsc = bullet->addComponentOfType<BoundingSphereCollider>();
        MeshRenderer* meshRenderer = bullet->addComponentOfType<MeshRenderer>();
        ResourceManager* rm = ResourceManager::getInstance();
        meshRenderer->mesh = rm->getMesh("skybox");
        std::shared_ptr<Material> mat = std::make_shared<Material>();
        mat->t_albedo = rm->getUserTextureResource("particleTexture");
        meshRenderer->material = mat;
        PhysicsObject* physicsObject = bullet->addComponentOfType<PhysicsObject>();
        physicsObject->vel = this->getForward() * speed + this->getForward() * -10.0f - glm::vec3(posUpdate.x, posUpdate.y, posUpdate.z);
        physicsObject->acc = glm::vec3(0.0f);

        gameObject->world->addObject(bullet);
    }
}


// Returns rocket's position
glm::vec3 Player::getPosition()
{
    return this->gameObject->transform.position;
}

// Returns rocket's fwd vector
glm::vec3 Player::getForward()
{
    return fwd;
}

// Returns rocket's position
glm::quat Player::getRotation()
{
    return this->gameObject->transform.rotation;
}


// Returns the rocket's x rotation
float Player::getXRotation()
{
    return rotation.x;
}

