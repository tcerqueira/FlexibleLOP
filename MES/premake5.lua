-- premake5
workspace "MES"
    configurations {"Debug", "Release"}
    architecture "x64"
    system "windows"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "MES"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/bin/%{outputdir}"
    objdir "build/obj/%{outputdir}"

    files {
        "src/**.h",
        "src/**.cpp",
        "libs/pugixml-1.11/src/*.cpp"
    }

    includedirs {
        "include",
        "libs/pugixml-1.11/include"
    }

    libdirs {}
    links {}

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
