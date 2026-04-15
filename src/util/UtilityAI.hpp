#pragma once

#ifndef UTILITYAI_H
#define UTILITYAI_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

enum class SB{
    PATHFOLLOWING,
    OVERTAKE,
    GROUNDACTION,
    AVOID,
    TAKEOBJECT,
    USEOBJECT
};

#include "UsingTypes.hpp"

// Clase base Consideration
class Consideration {
protected:
    std::string name;
    float value;

public:
    Consideration(const std::string& name, float value);
    virtual float scoreConsideration(const E& v) const = 0;
    
    std::string getName() const;
    float getValue() const;

    virtual ~Consideration() = default;
};

// Clase Action
class Action {
private:
    std::string name;
    float score;
    std::vector<std::shared_ptr<Consideration>> considerations;

public:

    Action(const std::string& name);

    void addConsideration(std::shared_ptr<Consideration> consideration);
    float scoreAction(const E& v);

    float getScore() const;
    std::string getName() const;

    virtual void execute(E& v) = 0; // Método puramente virtual
    virtual ~Action() = default;
};

// Considerations
class VelocityConsideration : public Consideration {
public:
    VelocityConsideration();
    float scoreConsideration(const E& v) const override;
};

class AccelerationConsideration : public Consideration {
public:
    AccelerationConsideration();
    float scoreConsideration(const E& v) const override;
};

class Distance2WallConsideration : public Consideration {
public:
    Distance2WallConsideration();
    float scoreConsideration(const E& v) const override;
};

class Distance2PowerupConsideration : public Consideration {
public:
    Distance2PowerupConsideration();
    float scoreConsideration(const E& v) const override;
};

class Distance2CarConsideration : public Consideration {
public:
    Distance2CarConsideration();
    float scoreConsideration(const E& v) const override;
};

class Distance2CarAheadConsideration : public Consideration {
public:
    Distance2CarAheadConsideration();
    float scoreConsideration(const E& v) const override;
};

class PositionConsideration : public Consideration {
public:
    PositionConsideration();
    float scoreConsideration(const E& v) const override;
};

class LapConsideration : public Consideration {
public:
    LapConsideration();
    float scoreConsideration(const E& v) const override;
};

class PowerUpConsideration : public Consideration {
public:
    PowerUpConsideration();
    float scoreConsideration(const E& v) const override;
};

class InversePowerUpConsideration : public Consideration {
public:
    InversePowerUpConsideration();
    float scoreConsideration(const E& v) const override;
};

class BoostConsideration : public Consideration {
public:
    BoostConsideration();
    float scoreConsideration(const E& v) const override;
};

class ShellConsideration : public Consideration {
public:
    ShellConsideration();
    float scoreConsideration(const E& v) const override;
};

class BananaConsideration : public Consideration {
public:
    BananaConsideration();
    float scoreConsideration(const E& v) const override;
};

class PathFollowingConsideration : public Consideration {
public:
    PathFollowingConsideration();
    float scoreConsideration(const E& v) const override;
};

class GroundConsideration : public Consideration {
public:
    GroundConsideration();
    float scoreConsideration(const E& v) const override;
};

// Actions ------------------------------
class PathFollowingAction : public Action {
public:
    PathFollowingAction();
    void execute(E& v) override;
};

class OvertakeAction : public Action {
public:
    OvertakeAction();
    void execute(E& v) override;
};

class GroundAction : public Action {
public:
    GroundAction();
    void execute(E& v) override;
};

class AvoidAction : public Action {
public:
    AvoidAction();
    void execute(E& v) override;
};

class TakeObjectAction : public Action {
public:
    TakeObjectAction();
    void execute(E& v) override;
};

class UseObjectBoostAction : public Action {
public:
    UseObjectBoostAction();
    void execute(E& v) override;
};

class UseObjectBananaAction : public Action {
public:
    UseObjectBananaAction();
    void execute(E& v) override;
};

class UseObjectShellAction : public Action {
public:
    UseObjectShellAction();
    void execute(E& v) override;
};
 

#endif


