//
// Lol Engine
//
// Copyright: (c) 2010-2012 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

//
// The Stick class
// ---------------
//

#if !defined __LOL_INPUT_STICK_H__
#define __LOL_INPUT_STICK_H__

#include "entity.h"

namespace lol
{

class StickData;

class Stick : public Entity
{
    friend class Input;

public:
    void SetAxisCount(int n);
    void SetButtonCount(int n);
    void SetAxis(int n, float val);
    void SetButton(int n, int val);
    void RemapAxis(int src, int dst);
    void RemapButton(int src, int dst);

    int GetAxisCount();
    int GetButtonCount();
    float GetAxis(int n);
    int GetButton(int n);

private:
    Stick();
    ~Stick();
    StickData *m_data;
};

} /* namespace lol */

#endif // __LOL_INPUT_STICK_H__
