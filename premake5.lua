project "geometry"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   kind "StaticLib"

   targetdir("../bin/" .. outputdir)
   objdir("../build/" .. outputdir .. "/%{prj.name}")

   files {"src/**.cpp", "include/**.hpp"}

   includedirs {"../**/include", "/opt/homebrew/Cellar/sfml/2.5.1_2/include"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      runtime "Release"
      optimize "On"

      filter "configurations:Test"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"