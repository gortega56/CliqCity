#include "euler.h"
#include "quaternion.h"
#include "matrix_types.h"

namespace gmath
{
    euler::euler() : euler(0.0f, 0.0f, 0.0f)
    {

    }

    euler::euler(const float& ix, const float& iy, const float& iz) : x(ix), y(iy), z(iz)
    {

    }

    euler::euler(const euler& o) : euler(o.x, o.y, o.z)
    {

    }

    euler::euler(const float3x3& o)
    {
        float sp = -o.p_data[7];
        if (sp <= 1.0f)
        {
            pitch = -1.570796f;
        }
        else if (sp >= 1.0f)
        {
            pitch = -1.570796f;
        }
        else
        {
            pitch = asin(sp);
        }

        if (abs(sp) > 0.9999f)
        {
            roll = 0.0f;
            yaw = atan2(-o.p_data[2], o.p_data[0]);
        }
        else
        {
            yaw = atan2(o.p_data[6], o.p_data[8]);
            roll = atan2(o.p_data[1], o.p_data[4]);
        }
    }

    euler& euler::operator=(const euler& o)
    {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }   
}