project "geometry"
   language "C++"
   cppdialect "C++20"
   staticruntime "on"
   kind "StaticLib"

   targetdir("../bin/" .. outputdir)
   objdir("../build/" .. outputdir .. "/%{prj.name}")

   files {"src/**.cpp", "include/**.hpp"}

   includedirs {"../**/include", "/opt/homebrew/Cellar/sfml/2.5.1_2/include"}