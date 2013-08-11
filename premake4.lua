-- see http://glsdk.sourceforge.net/docs/html/pg_use.html

solution "glexamples"
	configurations {"Debug", "Release"}
	
	configuration {"Debug"}
		targetdir "bin/debug"
		
	configuration {"Release"}
		targetdir "bin/release"
		
	project "glexamples"
		kind "ConsoleApp"
		language "c++"
		files {"**.cpp", "**.h"} -- recursively add files

		-- ensure that paths are correct here!
		includedirs "../../glsdk/glfw/include"
		libdirs "../../glsdk/glfw/library"
		includedirs "../../glsdk/glimg/include"
		libdirs "../../glsdk/glimg/lib"
		includedirs "../../glsdk/glload/include"
		libdirs "../../glsdk/glload/lib"
		includedirs "../../glsdk/glm"
		
		configuration "windows"
			defines "WIN32"
			links {"opengl32"}
			
		configuration "Debug"
			targetsuffix "D"
			defines "_DEBUG"
			flags "Symbols"
			links {"glfwD", "glloadD", "glimgD"}
			
		configuration "Release"
			defines "NDEBUG"
			flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
			links {"glfw", "glload", "glimg"}