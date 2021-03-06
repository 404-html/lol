//
//  Lol Engine — Unit tests
//
//  Copyright © 2010—2015 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#include <lol/engine-internal.h>

#include <lolunit.h>

namespace lol
{

lolunit_declare_fixture(box_test)
{
    lolunit_declare_test(box2d_intersection)
    {
        box2 b1(vec2(0.f, 0.f), vec2(10.f, 10.f));
        box2 b2(vec2(5.f, 8.f), vec2(8.f, 12.f));
        box2 b3(vec2(5.f, 11.f), vec2(8.f, 13.f));

        lolunit_assert_equal(true, TestAABBVsAABB(b1, b2));
        lolunit_assert_equal(false, TestAABBVsAABB(b1, b3));

        box2 b4(vec2(96.f, 33.f), vec2(144.f, 129.f));
        box2 b5(vec2(264.f, 91.f), vec2(244.f, 71.f));

        lolunit_assert_equal(false, TestAABBVsAABB(b4, b5));
    }

    lolunit_declare_test(box2d_move)
    {
        box2 b1(vec2(0.f, 0.f), vec2(1.f, 1.f));
        box2 b2(vec2(2.f, 2.f), vec2(3.f, 3.f));

        b1 += vec2(0.6f, 0.6f);
        lolunit_assert_equal(false, TestAABBVsAABB(b1, b2));

        b1 += vec2(0.6f, 0.6f);
        lolunit_assert_equal(true, TestAABBVsAABB(b1, b2));

        b1 -= vec2(0.0f, 0.6f);
        lolunit_assert_equal(false, TestAABBVsAABB(b1, b2));
    }
};

} /* namespace lol */

