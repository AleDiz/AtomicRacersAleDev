#pragma once

#ifndef ENUMACTIONS_H
#define ENUMACTIONS_H

enum GameActions{
    ACTION_ACCELERATE   = 1 << 0,
    ACTION_BRAKE        = 1 << 1,
    ACTION_LEFT         = 1 << 2,
    ACTION_RIGHT        = 1 << 3,
    ACTION_POWERUP      = 1 << 4,
    ACTION_DRIFT        = 1 << 5,
    ACTION_BACKCAM      = 1 << 6,
    ACTION_PAUSE        = 1 << 7,
};

#endif