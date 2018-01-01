#pragma once
#ifndef EULER_H
#define EULER_H

namespace gmath
{
    struct float3x3;

    struct euler
    {
        union
        {
            struct
            {
                float x, y, z;
            };

            struct
            {
                float pitch, yaw, roll;
            };
        };

        euler();
        euler(const float& x, const float& y, const float& z);
        euler(const euler& o);
        euler(const float3x3& o);

        euler& operator=(const euler& o);
    };
}

#endif