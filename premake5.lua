workspace "PhysicsEngine"
	architecture "x86_64"
	startproject "PhysicsEngine"

	configurations
	{
		"Debug",
		"Release",
		"Distrib"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Eis/Eis/vendor/GLFW/include"
IncludeDir["Glad"] = "Eis/Eis/vendor/Glad/include"
IncludeDir["ImGui"] = "Eis/Eis/vendor/imgui"
IncludeDir["glm"] = "Eis/Eis/vendor/glm"
IncludeDir["stb_image"] = "Eis/Eis/vendor/stb_image"
IncludeDir["stb_image_resize"] = "Eis/Eis/vendor/stb_image_resize"
IncludeDir["stb_image_write"] = "Eis/Eis/vendor/stb_image_write"

group "Dependencies"
	include "Eis/Eis/vendor/GLFW"
	include "Eis/Eis/vendor/Glad"
	include "Eis/Eis/vendor/imgui"

group ""

project "Eis"
	location "Eis/Eis"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Eispch.h"
	pchsource "Eis/Eis/src/Eispch.cpp"

	files
	{
		"Eis/%{prj.name}/src/**.h",
		"Eis/%{prj.name}/src/**.cpp",
		"Eis/%{prj.name}/vendor/stb_image/**.h",
		"Eis/%{prj.name}/vendor/stb_image/**.cpp",
		"Eis/%{prj.name}/vendor/stb_image_resize/**.h",
		"Eis/%{prj.name}/vendor/stb_image_resize/**.cpp",
		"Eis/%{prj.name}/vendor/stb_image_write/**.h",
		"Eis/%{prj.name}/vendor/stb_image_write/**.cpp",
		"Eis/%{prj.name}/vendor/glm/glm/**.hpp",
		"Eis/%{prj.name}/vendor/glm/glm/**.inl"

	}

	includedirs
	{
		"Eis/%{prj.name}/src",
		"Eis/%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.stb_image_resize}",
		"%{IncludeDir.stb_image_write}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "EIS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "EIS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distrib" 
		defines "EIS_DISTRIB"
		runtime "Release"
		optimize "on"

project "PhysicsEngine"
	location "PhysicsEngine" 
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp"
	}

	includedirs
	{
		"Eis/Eis/src",
		"Eis/Eis/vendor/spdlog/include",
		"Eis/Eis/vendor/imgui",
		"%{IncludeDir.glm}",
		"%{prj.name}/src"
	}

	links
	{
		"Eis"
	}
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "EIS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "EIS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distrib"
		defines "EIS_DISTRIB"
		runtime "Release"
		optimize "on"