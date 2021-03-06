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
        "src/**.cpp"
    }

    includedirs {"include"}
    libdirs {"libs"}

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
