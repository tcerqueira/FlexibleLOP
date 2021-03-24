-- premake5
workspace "MES"
    configurations {"Debug", "Release"}
    architecture "x64"
    --system "windows"
    cppdialect "C++17"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "MES"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/bin/%{outputdir}"
    objdir "build/obj/%{outputdir}"

    files {
        "src/**.h",
        "src/**.cpp",
        "libs/pugixml/src/**.cpp"
    }

    includedirs {
        "include",
        "libs/pugixml/src",
        "libs/spdlog/include"
    }

    libdirs {
        --"libs/spdlog/build"
    }

    links {
        "pthread",
        "boost_system"
        --"spdlog"
    }

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
