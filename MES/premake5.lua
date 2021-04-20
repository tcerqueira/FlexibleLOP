-- premake5
workspace "MES"
    configurations {"Debug", "Release"}
    -- architecture "x64"
    --system "windows"
    cppdialect "C++17"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "MES"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/bin/%{outputdir}"
    objdir "build/obj/%{outputdir}"
    targetname "%{prj.name}"

    files {
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
        "libs/pugixml/src/**.cpp"
    }

    includedirs {
        "include",
        "src",
        "libs/pugixml/src",
        "libs/spdlog/include",
        "libs/open62541/include"
    }

    libdirs {
        "libs/open62541/build/bin"
    }

    links {
        "pthread",
        "boost_system:static",
        "open62541:static"
        -- "pqxx", "pq"
    }

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
