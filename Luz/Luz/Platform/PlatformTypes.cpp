#include "stdafx.h"
#include "PlatformTypes.h"

namespace Platform
{
    ScreenPoint::ScreenPoint() 
    {

    }

    ScreenPoint::ScreenPoint(int16_t _x, int16_t _y) 
        : x(_x)
        , y(_y) 
    {
    
    }
    
    ScreenPoint::ScreenPoint(_In_reads_(2) const int16_t *pArray) 
        : x(pArray[0])
        , y(pArray[1]) 
    {
    
    }

    ScreenPoint& ScreenPoint::operator= (const ScreenPoint& Int2) 
    { 
        x = Int2.x; 
        y = Int2.y; 
        return *this; 
    }
}