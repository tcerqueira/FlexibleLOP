workspace "MES"
   configurations { "Debug", "Release" }

project "MES"
   kind "ConsoleApp"
   language "C++"
   targetdir "build/bin/%{cfg.buildcfg}"
   objdir "build/obj/%{cfg.buildcfg}"

   files { "src/*.cpp", "src/*.h" }
   includedirs { "include" }
   libdirs { "libs" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"