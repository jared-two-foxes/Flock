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

newoption {
  trigger     = "install",
  description = "Add's to the solution the unit test projects.",
}



-- Setup --------------------------------------------------------------------------------------------------------------------------------------------

baseLocation     = path.getabsolute( "./" )
solutionLocation = path.getabsolute( "Build/Flock" )
projectLocation  = path.join( solutionLocation, "Projects" )

if ( false == os.isdir(solutionLocation) ) then
  os.mkdir( solutionLocation )
end




-- Workspace ----------------------------------------------------------------------------------------------------------------------------------------

workspace "Flock"
  configurations { "Debug", "Release" }
  architecture "x86_64"
  language "C++"
  location( solutionLocation )  
  flags { "NoEditAndContinue", "FloatFast" }
  targetdir ( path.join(baseLocation, "Bin") )

  includedirs {
    "./",
    "./include"
  }

  libdirs {
    "./lib"
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
  location( projectLocation )
  targetdir ( path.join(baseLocation, "Lib" ) )

  files {
    "Common/**.h",
    "Common/**.inl",
    "Common/**.hpp",
    "Common/**.cpp",
  }



-- Server -------------------------------------------------------------------------------------------------------------------------------------------

project "Server"  
  kind "ConsoleApp"
  language "C++"
  location( projectLocation )

  files {
    "Server/**.h",
    "Server/**.inl",
    "Server/**.hpp",
    "Server/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );



-- ClientConsole ------------------------------------------------------------------------------------------------------------------------------------

project "ClientConsole"  
  kind "ConsoleApp"
  language "C++"
  location( projectLocation )

  files {
    "ClientConsole/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );



-- Client -----------------------------------------------------------------------------------------------------------------------------------------------

project "Client"  
  kind "WindowedApp"
  language "C++"
  location( projectLocation )

  files {
    "Client/**.h",
    "Client/**.hpp",
    "Client/**.cpp"
  }

  links {
    "Common",
  }

  utils.addLibrariesToCurrentProject( desc.dependencies );