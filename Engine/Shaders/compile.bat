@ECHO OFF

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=vert composite.vert.glsl -o composite.vert.spirv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=frag composite.frag.glsl -o composite.frag.spirv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=vert shadow2D.vert.glsl -o shadow2D.vert.spirv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=vert gbuffer.vert.glsl -o gbuffer.vert.spirv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=frag gbuffer.frag.glsl -o gbuffer.frag.spirv