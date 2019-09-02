#pragma once
#ifndef GRAPHICSIMGUI_H
#define GRAPHICSIMGUI_H

namespace Graphics
{
    class CommandStream;

    LUZ_API bool Initialize_Imgui(const unsigned int nFramesInFlight);
    
    LUZ_API void Update_Imgui();
    
    LUZ_API void Render_Imgui(CommandStream* pCommandStream);

    LUZ_API void Shutdown_Imgui();
}

#endif