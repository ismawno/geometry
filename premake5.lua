project "shapes-2D"
   language "C++"
   cppdialect "C++20"
   staticruntime "on"
   kind "StaticLib"

   targetdir("../bin/" .. outputdir)
   objdir("../build/" .. outputdir .. "/%{prj.name}")

   files {"src/**.cpp", "include/**.hpp"}

   includedirs "../**/include"