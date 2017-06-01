--
--  Nebulae v0.0.1 build script
--
  package.path = package.path .. ";" .. os.getenv("userprofile") .. "/?.lua;"


  _ROOT_DIR = _SCRIPT_DIR


---
--  Load the workspace description file.
---
  local desc = require( 'workspace' )
  local user_libraries = require( 'libraries' )


--- 
--  
---

  local function boost_combine_components( a, b, ... )
    local out = a .. "-" .. b 
    if select('#',...) > 0 then
      local args = {...}
      for i = 1,#args do
        if args[i] ~= nil and args[i] ~= "" then
          out = out .. "-" .. args[i]
        end
      end
    end

    return out
  end


--- 
--  
---

  local function addLibrariesToCurrentProject( dependencies )
    if dependencies then
      for _, name in pairs( dependencies ) do

        -- Attempt to grab the named library from the global libraries list.
        local library = nil
        for id, libObj in pairs( user_libraries ) do
          if name == id then
            library = libObj
          end
        end

        -- If found assume that it was build with "Build" and link it appropriately.
        if library ~= nil then
          if library.includePath ~= nil then
            includedirs { path.join( _ROOT_DIR, "_external", "include", library.includePath ) } 
          elseif library.system == "boost" then
            includedirs { path.join( _ROOT_DIR, "_external", "include", library.name .. "-" .. library.version ) } 
          end

          if library.libPath ~= nil then
            includedirs { path.join( _ROOT_DIR, library.libPath ) } 
          end

          -- Add the libraries.
          if library.naming ~= nil and library.naming ~= "none" then     
            local libraryDebugName = library.name
            local libraryReleaseName = library.name

            if library.naming == "standard" then
              libraryDebugName = libraryDebugName .. "d.lib"
              libraryReleaseName = libraryReleaseName .. ".lib"
            elseif library.naming == "versioned" then
              libraryDebugName = libraryDebugName .. library.version .. "d.lib"
              libraryReleaseName = libraryReleaseName .. library.version .. ".lib"
            elseif library.naming == "win32" then
              libraryDebugName = libraryDebugName .. "32.lib"
              libraryReleaseName = libraryReleaseName .. "32.lib"
            elseif library.naming == "architecture" then
              libraryDebugName = libraryDebugName .. "64.lib"
              libraryReleaseName = libraryReleaseName .. "64.lib"
            elseif library.naming == "boost" then
              local toolset = "v141"
              local threading = "mt"
              local runtime = "" --"s"
              local debug = "gd"
              local version = "4_2_3"

              libraryDebugName = boost_combine_components( 
                library.name, toolset, threading, runtime .. debug,
                version ) .. ".lib"

              libraryReleaseName = boost_combine_components( 
                library.name, toolset, threading, runtime,
                version ) .. ".lib"
            end
            
            filter 'configurations:Release'
              links { libraryReleaseName } 
            filter {}

            filter 'configurations:Debug'
              links { libraryDebugName } 
            filter {} 
          end
        
        -- Assume that the library is build as part of this project and attempt to link it that way.
        else
          library = desc.libraries[ name ]
          links{ name }
        end

        -- Either way we will need to recurse if a library was found.
        if library ~= nil then
          addLibrariesToCurrentProject( library.dependencies )
        end 
      end
    end
  end


--- 
--  
---

  local function createProject( t, details )
    project( details.name )
    
    kind( t )
    
    architecture "x86_64"
    
    language "C++"
    
    location( path.join( os.getcwd(), "_build", desc.name, details.name ) )
    
    files {  
      path.join( details.path, "**.cpp" ), 
      path.join( details.path, "**.hpp" ),
      path.join( details.path, "**.h" )
    }
    
    includedirs {
      details.path
    }
    
    if details.defines then
      for _, n in ipairs( details.defines ) do
        defines { n }
      end
    end

    addLibrariesToCurrentProject( details.dependencies );
  end


--- 
--  Create the workspace objects and setup defaults.
---

  workspace( desc.name )
    configurations { "Debug", "Release" }
    architecture "x86_64"
    language "C++"
    location( path.join( os.getcwd(), "_build", desc.name  ) )  
    flags { "FloatFast" }

    defines {
      "PREMAKE5",
      "NEBULAE_INCLUDES_PNG"
    }

    includedirs {
      "./",
      "./_external/include"
    }

    libdirs {
      "./_external/lib"
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
        "WIN32",
        "_WIN32" 
      }

      buildoptions { "/EHsc", "/MP" }
      --flags        { 'FatalWarnings' }

    filter {}



---
--  Create the libraries projects based upon the description file.
---

  for _, details in pairs( desc.libraries ) do
    createProject( "StaticLib", details )
  end



--- 
--  Create the application projects
---
  for _, details in pairs( desc.binaries ) do
    createProject( "ConsoleApp", details )
  end