#include "AIComponent.hpp"
#include "../util/EnumsActions.hpp"

void AIComponent::initAIComponent(float arrivalRadius, float time2arrive, float visionDistance, float time2Update)
{
    this->arrivalRadius = arrivalRadius;
    this->time2arrive = time2arrive;
    this->visionDistance = visionDistance;
    this->time2Update = time2Update;
    this->accumulatedTime = time2Update;

    this->actions = createActions();
}

std::array<std::shared_ptr<Action>, 8> AIComponent::createActions(){
    std::array<std::shared_ptr<Action>, 8> actions;

    // Crear las acciones
    auto pathFollowingAction = std::make_shared<PathFollowingAction>();
    auto overtakeAction = std::make_shared<OvertakeAction>();
    auto groundAction = std::make_shared<GroundAction>();
    auto avoidAction = std::make_shared<AvoidAction>();
    auto takeObjectAction = std::make_shared<TakeObjectAction>();
    auto useObjectBoostAction = std::make_shared<UseObjectBoostAction>();
    auto useObjectBananaAction = std::make_shared<UseObjectBananaAction>();
    auto useObjectShellAction = std::make_shared<UseObjectShellAction>();

    // Crear las consideraciones
    auto velocityConsideration = std::make_shared<VelocityConsideration>();
    auto accelerationConsideration = std::make_shared<AccelerationConsideration>();
    auto distance2WallConsideration = std::make_shared<Distance2WallConsideration>();
    auto distance2PowerupConsideration = std::make_shared<Distance2PowerupConsideration>();
    auto distance2CarConsideration = std::make_shared<Distance2CarConsideration>();
    auto distance2CarAheadConsideration = std::make_shared<Distance2CarAheadConsideration>();
    auto positionConsideration = std::make_shared<PositionConsideration>();
    auto lapConsideration = std::make_shared<LapConsideration>();
    auto powerUpConsideration = std::make_shared<PowerUpConsideration>();
    auto inversePowerUpConsideration = std::make_shared<InversePowerUpConsideration>();
    auto boostConsideration = std::make_shared<BoostConsideration>();
    auto shellConsideration = std::make_shared<ShellConsideration>();
    auto bananaConsideration = std::make_shared<BananaConsideration>();
    auto pathFollowingConsideration = std::make_shared<PathFollowingConsideration>();
    auto groundConsideration = std::make_shared<GroundConsideration>();


    // Asignar las consideraciones a las acciones
    pathFollowingAction->addConsideration(pathFollowingConsideration);

    overtakeAction->addConsideration(velocityConsideration);
    overtakeAction->addConsideration(distance2CarAheadConsideration);
    overtakeAction->addConsideration(positionConsideration);
    //overtakeAction->addConsideration(lapConsideration);
    overtakeAction->addConsideration(accelerationConsideration);
    //overtakeAction->addConsideration(boostConsideration);

    groundAction->addConsideration(groundConsideration);
    groundAction->addConsideration(positionConsideration);

    avoidAction->addConsideration(distance2WallConsideration);

    takeObjectAction->addConsideration(distance2PowerupConsideration);
    takeObjectAction->addConsideration(inversePowerUpConsideration);
    takeObjectAction->addConsideration(positionConsideration);

    useObjectBoostAction->addConsideration(powerUpConsideration);
    useObjectBoostAction->addConsideration(boostConsideration);
    useObjectBoostAction->addConsideration(distance2CarAheadConsideration);

    useObjectBananaAction->addConsideration(powerUpConsideration);
    useObjectBananaAction->addConsideration(bananaConsideration);
    useObjectBananaAction->addConsideration(distance2CarConsideration);

    useObjectShellAction->addConsideration(powerUpConsideration);
    useObjectShellAction->addConsideration(shellConsideration);
    useObjectShellAction->addConsideration(distance2CarAheadConsideration);

    // Agregar todas las acciones al vector
    actions[0] = pathFollowingAction;
    actions[1] = groundAction;
    actions[2] = takeObjectAction;
    actions[3] = avoidAction;
    actions[4] = pathFollowingAction;
    actions[5] = useObjectBananaAction;
    actions[6] = useObjectBoostAction;
    actions[7] = useObjectShellAction;

    return actions;
}


void AIComponent::clearComponent()
{

}