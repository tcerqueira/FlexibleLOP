-- premake5
workspace "MES"
    configurations {"Debug", "Release"}
    architecture "x86_64"
    cppdialect "C++17"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "MES"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/bin/%{outputdir}"
    objdir "build/obj/%{outputdir}"
    targetname "%{prj.name}"
    targetextension ""

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
        "libs/open62541/include",
        "libs/SQLAPI/include"
    }

    libdirs {
        "libs/open62541/build/bin",
        "libs/spdlog/build",
        "libs/SQLAPI/lib64"
    }

    links {
        "pthread",
        "boost_system:static",
        "open62541:static",
        "spdlog:static",
        "sqlapi:static",
        "dl"
    }

    filter "configurations:Debug"
        defines {"DEBUG", "OPC_LISTEN_ASYNC_MODE=0", "DB_ASYNC_MODE=0"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG", "OPC_LISTEN_ASYNC_MODE=0", "DB_ASYNC_MODE=0"}
        optimize "On"
