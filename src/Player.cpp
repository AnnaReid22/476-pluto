#include "GameObject.h"
#include "Player.h"
#include <iostream>

void Player::Start()
{
}

void Player::Update()
{
}


void Player::OnCollide(GameObject* other) 
{

    if (other->tag == "planet")
    {
        Camera* cam = this->gameObject->getComponentByType<Camera>();
        cam->stop = true;
    }
}
