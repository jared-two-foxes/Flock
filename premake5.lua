--
--  Flock v0.0.1 build script
--

local utils   = require( 'utils' )
local desc = require( 'project' )

-----------------------------------------------------------------------------------------------------------------------------------------------------

newoption {
  trigger     = "tests",
  description = "Add's to the solution the unit test projects.",
}


-- Setup --------------------------------------------------------------------------------------------------------------------------------------------

local workspaceName = "Flock"

baseLocation     = path.getabsolute( "./" )
solutionLocation = path.getabsolute( "Projects/" .. workspaceName )

if ( false == os.isdir(solutionLocation) ) then
  os.mkdir( solutionLocation )
end



-- Workspace ----------------------------------------------------------------------------------------------------------------------------------------

workspace( workspaceName )
  configurations { "Debug", "Release" }
  architecture "x86_64"
  language "C++"
  location( solutionLocation )  
  flags { "NoEditAndContinue", "FloatFast" }
  targetdir ( path.join(baseLocation, "Bin") )

  includedirs {
    "./Source",
    "./Externals/include"
  }

  libdirs {
    "./Externals/lib"
  }

  filter "configurations:Debug"
    defines { "DEBUG", "_DEBUG" }
    targetsuffix 'd'
    symbols "On"
  
  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On" 

  filter "action:vs*"
    systemversion "10.0.14393.0"
    defines{
      "NOMINMAX",
      "WIN32",
      "_WIN32" 
    }

    buildoptions { "/EHsc" }
    flags        { "WinMain" }

  filter {}


-- Common -------------------------------------------------------------------------------------------------------------------------------------------

project "Common"  
  kind "StaticLib"
  language "C++"
  location( solutionLocation )
  targetdir ( path.join(solutionLocation, "Lib" ) )

  files {
    "Source/Common/**.h",
    "Source/Common/**.inl",
    "Source/Common/**.hpp",
    "Source/Common/**.cpp",
  }



-- Server -------------------------------------------------------------------------------------------------------------------------------------------

project "Server"  
  kind "ConsoleApp"
  language "C++"
  location( solutionLocation )

  files {
    "Source/Server/**.h",
    "Source/Server/**.inl",
    "Source/Server/**.hpp",
    "Source/Server/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );



-- ClientConsole ------------------------------------------------------------------------------------------------------------------------------------

project "ClientConsole"  
  kind "ConsoleApp"
  language "C++"
  location( solutionLocation )

  files {
    "Source/ClientConsole/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );



-- Client -----------------------------------------------------------------------------------------------------------------------------------------------

project "Client"  
  kind "WindowedApp"
  language "C++"
  location( solutionLocation )

  files {
    "Source/Client/**.h",
    "Source/Client/**.hpp",
    "Source/Client/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );