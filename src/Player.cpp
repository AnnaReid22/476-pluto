#include "GameObject.h"
#include "Player.h"
#include "ParticleSystem.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "PhysicsObject.h"
#include "World.h"
#include "Fin.h"
#include "Camera_Follow_Rocket.h"
#include <iostream>
#define PI 3.14159265
#define MAXDOLLYFTIME 1.3
#define DISASSEMBLE_SPEED .5
#define LOSE_FINS_TIME 15//7
//#define DISASSEMBLE_ANIM 0
//#define ROTATE_FINS_ANIM 1
#include "soloud.h"
#include "soloud_wav.h"

ResourceManager* rm_ = ResourceManager::getInstance();
SoLoud::Soloud gSoloudPlayer;
SoLoud::Wav gWaveBG;
SoLoud::Wav gWaveShoot;
SoLoud::Wav gWaveLoseLife;
SoLoud::Wav gWaveDie;
SoLoud::Wav gWaveWin;
SoLoud::Wav gWaveFly;
int bg;
int win_play = 0;
int fly_play = 0;
int flying_handle;

Player::Player(GameObject* d_GameObject) : Component(d_GameObject)
{
    // Has rocket collided with planet?
    stop = false;

    // Move forward and backwards
    dollyF = false;
    prevDollyF = false;
    dollyFTime = 0;
    stopTime = 0;
    shakeTime = 0;
    collideTime = LOSE_FINS_TIME;//6.0f;
    dead = false;
    alreadyShot = true;
    posUpdate = glm::vec4(0, 0, 0, 1);
    prepareShootTime = 0;
    won = false;
    wonTimer = 0;

    // Variables used for losing fins upon collision with asteroid
    loseFinsTime = 0;

    // Variables used for obtaining a new set of fins
    getFinsTime = 0;

    // Initial num lives
    numLives = 4;

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
    rotMat = mat4(1);

    // Set original fin positions to 0
    fin1R = mat4(1);

    // Original fin positions are not set yet
    setOriginalDisassemblePositions = false;
}

/*
* Sets initial rocket position
*/
void Player::Start()
{   
    gSoloudPlayer.init();  
    // //https://www.free-stock-music.com/savfk-deep.html
    gWaveBG.load("../resources/audio/bg.wav");
    int bg = gSoloudPlayer.playBackground(gWaveBG);
    this->gameObject->transform.position = glm::vec3(0, 0, -4);
    this->gameObject->transform.scale = originalScale;
    time = time->getInstance();

  
}


/*
* Calls functions that move and rotate rocket
*/
void Player::Update()
{
    if (InputManager::getInstance()->GetKey(GLFW_KEY_P))
    {
        won = true;
        rotation = glm::vec3(0, 0, 0);
    }
    updateMoveVars();
    if (!stop)
    {
        if (loseFinsTime > 0)
        {
            LoseFin(numLives-1);
        }
        else
        {
            moveRocket();
        }
    }
    else if (collideTime > 0)
    {
        DisassembleRocket();
    }
    else if (!dead)
    {
        KillRocket();
        dead = true;
        ResourceManager::getInstance()->addNumericalValue("guiDeathTime", Time::getInstance()->getGlobalTime());

    }
    if (prepareShootTime > 0)
    {
        PrepareShoot();
    }
    else if (!alreadyShot)
    {
        Shoot();
    }
    if (shakeTime > 0)
    {
        ShakeRocket();
    }
    if (won)
    {
        WinAnimation();
    }

}

void Player::WinAnimation()
{
    wonTimer += time->getFrametime();
    if(wonTimer < 8.5)
    { 
        gameObject->transform.position += glm::vec3(0, -.05, 0);
        rotation = glm::vec3(0, 4 * wonTimer, 0);
    }
    else
    {
        Camera_Follow_Rocket* camFollowRocket = cam1->gameObject->getComponentByType<Camera_Follow_Rocket>();
        if (!(camFollowRocket->startWinAnimation))
        {
            camFollowRocket->SetUpWinAnimation();
        }
        
        gameObject->transform.position += glm::vec3(0, .2, 0);
    }
    //std::cout << "wonTimer: " << wonTimer << std::endl;
    ResourceManager* rm = ResourceManager::getInstance();
    if (wonTimer > 20 && rm->getNumericalValue("guiWinTime") < 0)
    {
        rm->addNumericalValue("guiWinTime", Time::getInstance()->getGlobalTime());
    }
    
}
// Lose fins when you crash with an asteroid 
void Player::LoseFin(int finNum)
{   
    UpdateFinFallOff(finNum);
    loseFinsTime -= 3*time->getFrametime();
    

    ParticleSystem* ps = this->gameObject->getComponentByType<ParticleSystem>();
    if (ps->gameObject->isEnabled)
    {
        ps->gameObject->Disable();
    }

    if (loseFinsTime <= 0)
    {
        Camera_Follow_Rocket* camFollowRocket = cam1->gameObject->getComponentByType<Camera_Follow_Rocket>();
        camFollowRocket->StopPanning();
        KillFin(finNum);
        ps->gameObject->Enable();
    }
}

void Player::UpdateFinFallOff(int finNum)
{
    Fin* finVars = finObjs[finNum]->getComponentByType<Fin>();
    if (!(finVars->initializedVariables))
    {
        finVars->SetFinVars();
    }
    //std::cout << "FinDirection: " << normalize(finVars->finDirection).x << ", " << normalize(finVars->finDirection).y << ", " << normalize(finVars->finDirection).z << std::endl;
    finObjs[finNum]->transform.hierarchicalTrans2 = ((float)DISASSEMBLE_SPEED * (float)(LOSE_FINS_TIME - loseFinsTime) * normalize(finVars->finDirection));// finVars->originalFinPos + ((float)DISASSEMBLE_SPEED * (float)(LOSE_FINS_TIME - loseFinsTime) * normalize(finVars->finDirection));//glm::vec3(fin1R*glm::vec4(normalize(originalFin1Pos),0)));
    // Set rotation to original rotation so that the fins don't spin as the rocket goes forward
    glm::vec3 finRot = glm::vec3((LOSE_FINS_TIME - loseFinsTime) * 10, 0.0f, 0.0f);
    glm::vec3 perpendicularVector = glm::cross(finVars->finDirection, glm::vec3(0, 1, 0));
    finObjs[finNum]->transform.hierarchicalRot1 = glm::quat(1.0, 0.0, 0.0, 0.0);
    //std::cout << "PerpendicularVector: " << normalize(perpendicularVector).x << ", " << normalize(perpendicularVector).y << ", " << normalize(perpendicularVector).z << std::endl;
    finObjs[finNum]->transform.hierarchicalTrans1 = 0.5f * normalize(perpendicularVector);
    finObjs[finNum]->transform.hierarchicalRot2 = glm::quat(glm::rotate(glm::mat4(1.f), finRot.x, normalize(finVars->finDirection)));
}

// Shake rocket along forward vecyor. Used after shooting.
void Player::ShakeRocket()
{
    float frametime = time->getFrametime();
    shakeTime -= frametime;
    this->gameObject->transform.position += this->getForward()*((float)(rand() % 7 - 3) / 20.0f);
}


float Player::hash(float n)
{
    return fract(sin(n) * 753.5453123);
}

// Noise function used to disassemble rocket
glm::vec3 Player::snoiseRotation(glm::vec3 x)
{
    glm::vec3 rotation;
    glm::vec3 p = floor(x);
    glm::vec3 f = fract(x);
    f = f * f * (3.0f - 2.0f * f);
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    rotation.x = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x), mix(hash(n + 15.0), hash(n + 300.0), f.x), f.y), mix(mix(hash(n + 78.0), hash(n + 43.0), f.x), mix(hash(n + 88.0), hash(n + 53.0), f.x), f.y), f.z);
    rotation.y = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x), mix(hash(n + 133.0), hash(n + 67.0), f.x), f.y), mix(mix(hash(n + 215.0), hash(n + 328.0), f.x), mix(hash(n + 112.0), hash(n + 14.0), f.x), f.y), f.z);
    rotation.z = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x), mix(hash(n + 99.0), hash(n + 156.0), f.x), f.y), mix(mix(hash(n + 245.0), hash(n + 114.0), f.x), mix(hash(n + 270.0), hash(n + 275.0), f.x), f.y), f.z);
    rotation.x = sin(rotation.x * 111.);
    rotation.y = sin(rotation.y * 111.);
    rotation.z = sin(rotation.z * 111.);
    return rotation;
}

glm::mat4 Player::rotationMatrix(glm::vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
        0.0, 0.0, 0.0, 1.0);
}

// Make the different parts of the rocket fall apart. Used after the rocket crashes. //TODO: add random number generation
void Player::DisassembleRocket()
{ 
    if (rocketBody->deformFactor < 0.09)
    {
        rocketBody->deformFactor += 0.0001;
    }

    

    if (!setOriginalDisassemblePositions)
    {
        vec3 rotation = normalize(snoiseRotation(normalize(finObjs[0]->transform.position)));

        fin1R = rotationMatrix(rotation, 5*collideTime);
     
        originalRocketBodyPos = normalize(vec3(0, 0, -1));

        setOriginalDisassemblePositions = true;

    }
    for (int i = numLives - 2; i >= 0; i--)
    {
        UpdateFinFallOff(i);
    }
    
    rocketBody->transform.position += ((float)(time->getFrametime()) * (float)DISASSEMBLE_SPEED * originalRocketBodyPos);
    collideTime -= time->getFrametime();
    loseFinsTime -= time->getFrametime();
}

// Stop rendering the fin called finNum
void Player::KillFin(int finNum)
{
    MeshRenderer* fin_mesh = finObjs[finNum]->getComponentByType<MeshRenderer>();
    fin_mesh->Disable();
}

// Stop rendering rocket mesh
void Player::KillRocket()
{
    // Stop rendering rocket body 
    MeshRenderer* rocket_body_mesh = rocketBody->getComponentByType<MeshRenderer>();
    rocket_body_mesh->Disable();

    // Stop rendering fins
    // fin1
    MeshRenderer* fin_mesh = finObjs[0]->getComponentByType<MeshRenderer>();
    fin_mesh->Disable();

    // fin2
    fin_mesh = finObjs[1]->getComponentByType<MeshRenderer>();
    fin_mesh->Disable();

    // fin3
    fin_mesh = finObjs[2]->getComponentByType<MeshRenderer>();
    fin_mesh->Disable();


    // Activate particle systems
    GameObject* ps = (GameObject*)rm_->getOther("particle_system_death");

    ps->transform = this->gameObject->transform;
    ps->Enable();
    ParticleSystem* ps_obj = ps->getComponentByType<ParticleSystem>();
    ps_obj->start = this->gameObject->transform.position;
    //https://www.shockwave-sound.com/free-sound-effects/explosion-sounds
    gWaveDie.load("../resources/audio/die.wav");
    gSoloudPlayer.play(gWaveDie);
}

// Rocket gets fact before it shoots
void Player::PrepareShoot()
{
    this->gameObject->transform.scale.x += (2.0*(0.5-prepareShootTime));
    prepareShootTime -= time->getFrametime();
}

void Player::Shoot()
{
    alreadyShot = true;
    shakeTime = 1.0f;
    bulletCooldown = 0.5f;
    GameObject* bullet = new GameObject("lazer");
    bullet->transform.position = gameObject->transform.position - 4.5f * this->getForward();
    bullet->transform.scale = gameObject->transform.scale * 0.5f;
    bullet->transform.rotation = gameObject->transform.rotation;

    BoundingSphereCollider* bsc = bullet->addComponentOfType<BoundingSphereCollider>();
    MeshRenderer* meshRenderer = bullet->addComponentOfType<MeshRenderer>();
    ResourceManager* rm = ResourceManager::getInstance();
    meshRenderer->mesh = rm->getMesh("lazer");
    meshRenderer->material = std::make_shared<Material>();
    meshRenderer->material->t_albedo = rm->getUserTextureResource("lazerTexture");

    PhysicsObject* physicsObject = bullet->addComponentOfType<PhysicsObject>();
    physicsObject->vel = this->getForward() * -30.0f - glm::vec3(posUpdate.x, posUpdate.y, posUpdate.z);
    physicsObject->acc = glm::vec3(0.0f);

    gameObject->world->addObject(bullet);
    bsc->radius = 0.2f; 
    //https://www.soundfishing.eu/sound/laser-gun
    gWaveShoot.load("../resources/audio/shoot.mp3");
    gSoloudPlayer.play(gWaveShoot);
}


/*
* Rocket stops moving when it collides with a planet
*/
void Player::OnCollide(GameObject* other) 
{
    if (other->name == "asteroid")
    {
        if (numLives > 0 && loseFinsTime <=-3)
        {
            loseFinsTime = LOSE_FINS_TIME;
            numLives--;

            Camera_Follow_Rocket* camFollowRocket = cam1->gameObject->getComponentByType<Camera_Follow_Rocket>();
            camFollowRocket->SetUpLoseFin();
           
            //https://www.shockwave-sound.com/free-sound-effects/explosion-sounds
            gWaveLoseLife.load("../resources/audio/losefin.wav");
            gSoloudPlayer.play(gWaveLoseLife);
        }
        else if(numLives <= 0 && !stop)
        {
            stop = true;
            rocketBody->deform = true;
            //https://www.shockwave-sound.com/free-sound-effects/explosion-sounds
            gWaveDie.load("../resources/audio/losefin.wav");
            gSoloudPlayer.play(gWaveDie);
        }
    }
    if (other->tag == "planet" && other->name != "pluto")
    {
        if (loseFinsTime <= 0)
        {
            loseFinsTime = LOSE_FINS_TIME;
            rocketBody->deform = true;
            Camera_Follow_Rocket* camFollowRocket = cam1->gameObject->getComponentByType<Camera_Follow_Rocket>();
            camFollowRocket->SetUpLoseFin();//Be careful with the wing index you are passing in here
             //https://www.shockwave-sound.com/free-sound-effects/explosion-sounds
            gWaveDie.load("../resources/audio/losefin.wav");
            gSoloudPlayer.play(gWaveDie);
        }
        ParticleSystem* ps = this->gameObject->getComponentByType<ParticleSystem>();
      
        stop = true;
       
        
    }
    if (other->name == "pluto")
    {
        won = true;
        this->gameObject->getComponentByType<ParticleSystem>()->Enable();
        
        //stop = true; 
        rotation = glm::vec3(0, 0, 0);

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
        
        if(win_play == 0)
        {
            // https://www.soundboard.com/sb/sound/1001390
            gWaveWin.load("../resources/audio/win.mp3");
            gSoloudPlayer.play(gWaveWin);
            win_play = 1;
        }

    }
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
    //Time* time = time->getInstance();
    if (dollyF)// && dollyFTime < MAXDOLLYFTIME)
    {
        this->gameObject->getComponentByType<ParticleSystem>()->Enable();
        if(fly_play == 0)
        {
            // https://soundbible.com/tags-rocket.html
            gWaveFly.load("../resources/audio/fly.mp3");
            flying_handle = gSoloudPlayer.play(gWaveFly);
            int flying_handle;
            fly_play = 1;
        }
        dollyFTime += time->getFrametime();
    }
    else if(!dollyF)
    {
        if(!won)
            this->gameObject->getComponentByType<ParticleSystem>()->Disable();
        fly_play = 0;
        gSoloudPlayer.stop(flying_handle);
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
}

/*
* Calculates the rocket's position matrix and rotation quaternion and 
* updates the rocket's transform.
*/
void Player::moveRocket()
{
    loseFinsTime -= time->getFrametime();
    //Time* time = time->getInstance();
    float frametime = time->getFrametime();

    // Determine direction of rocket movement
    glm::vec3 rocketMove = glm::vec3(0.0f, 0.0f, 0.0f);
   
    float t = 0;
    if (dollyFTime > 0)
    {
        t = dollyFTime < MAXDOLLYFTIME ? dollyFTime : MAXDOLLYFTIME;
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
  
    //ParticleSystem* ps = this->gameObject->getComponentByType<ParticleSystem>();
    if (dollyF)
    {
        rocketMove.y += frametime * speed;
        /*if (!(ps->gameObject->isEnabled))
        {
            ps->gameObject->Enable();
        }*/

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
    rotMat = glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1, 0, 0));


    // Adjust movement based on the rocket's rotation
    posUpdate = rotMat * glm::vec4(rocketMove, 0); //glm::vec4(0, -10, 0, 0) if you want constant forward movement
    
    // Store fwd vector of the rocket
    fwd = normalize(rotMat * glm::vec4(0, -1, 0, 0));
     
    // Update the Rocket's Transform position matrix and rotation quaternion
    this->gameObject->transform.position -= glm::vec3(posUpdate.x, posUpdate.y, posUpdate.z);
    this->gameObject->transform.rotation = glm::quat(rotMat);;//glm::quat(rotMat); // glm::quat(1.0, 0.0, 0.0, 0.0);


    // Fin animation
    finObjs[0]->transform.position = glm::vec3(0.4, -0.78, 0.15);
    finObjs[0]->transform.rotation = glm::vec3(0.0f, glm::radians(-130.0f), 0.0f);//glm::radians(10.0f));
    finObjs[0]->transform.scale = glm::vec3(0.5, 0.5, 0.5);

    finObjs[1]->transform.position = glm::vec3(-0.4, -0.78, 0.15);
    finObjs[1]->transform.rotation = glm::vec3(0.0f, glm::radians(130.0f), 0.0f);//glm::radians(-10.0f));
    finObjs[1]->transform.scale = glm::vec3(0.5, 0.5, 0.5);

    finObjs[2]->transform.position = glm::vec3(0.0, -0.7, -0.4);
    finObjs[2]->transform.rotation = glm::vec3(0.0f, glm::radians(0.0f), glm::radians(0.0f));
    finObjs[2]->transform.scale = glm::vec3(0.5, 0.5, 0.5);

 

    if (dollyFTime > 0)
    {
        glm::vec3 finRot = glm::vec3(0, dollyFTime*4, 0.0f);
        //std::cout << "dollyFTime: " << dollyFTime << std::endl;
        finObjs[0]->transform.hierarchicalRot1 = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        finObjs[0]->transform.position = glm::vec3(0.5, -0.78, 0.20);
        finObjs[1]->transform.hierarchicalRot1 = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        finObjs[1]->transform.position = glm::vec3(-0.5, -0.78, 0.20);
        finObjs[2]->transform.hierarchicalRot1 = glm::quat(glm::rotate(glm::mat4(1.f), finRot.y, glm::vec3(0, 1, 0)));
        finObjs[2]->transform.position = glm::vec3(0.0, -0.7, -0.5);
        //t = dollyFTime;
    }
    

    //fin1->transform.position = glm::vec3(1, 0, 1);
    //std::cout << "fin1: " << fin1 << std::endl;
    //std::cout << "player: " << this->gameObject << std::endl;
    glm::mat4 MPlayer = this->gameObject->transform.genModelMatrix();
    glm::mat4 MFin = this->gameObject->transform.genModelMatrix();
    

   

    bulletCooldown -= Time::getInstance()->getFrametime();
    if (InputManager::getInstance()->GetKey(GLFW_KEY_SPACE) && bulletCooldown <= 0)
    {
        prepareShootTime = 0.5f;
        alreadyShot = false;
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


// Returns the vector that points to the rocket's right side
glm::vec3 Player::getRightVector()
{
    return glm::vec3(normalize(rotMat * glm::vec4(1, 0, 0, 0)));
}

// Returns the vector that points to the rocket's up
glm::vec3 Player::getUpVector()
{
    return glm::vec3(normalize(rotMat * glm::vec4(0, 0, 1, 0)));
}

