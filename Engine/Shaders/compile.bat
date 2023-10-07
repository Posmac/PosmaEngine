@ECHO OFF

D:/VulkanSDK/1.3.216.0/Bin/glslc.exe -fshader-stage=vert triangle.vert.glsl -o triangle.vert.txt
D:/VulkanSDK/1.3.216.0/Bin/glslc.exe -fshader-stage=frag triangle.frag.glsl -o triangle.frag.txt

D:/VulkanSDK/1.3.216.0/Bin/glslc.exe -fshader-stage=vert shadow2D.vert.glsl -o shadow2D.vert.txt