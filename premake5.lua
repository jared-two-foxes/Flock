--
--  Nebulae v0.0.1 build script
--
  package.path = package.path .. ";" .. os.getenv("userprofile") .. "/?.lua;"


  _ROOT_DIR = _SCRIPT_DIR


  newoption {
    trigger     = "outdir",
    value       = "path",
    description = "Output directory for the compiled executable"
  }


---
--  Load the workspace description file.
---
  local desc = require( 'workspace' )
  local user_libraries = require( 'libraries' )



  local builder = {}

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

  function builder.setupProject( prj, wksp )

    -- Link in include paths, if required.
    if prj.includePath ~= nil then
      includedirs { 
        path.join( 
          _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ),
          "../..", "_external", "include", prj.includePath 
        )
      } 
    elseif prj.system == "boost" then
      includedirs { 
        path.join( 
          _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ),
          "../..", "_external", "include", prj.name .. "-" .. prj.version 
        ) 
      } 
    end

    -- Link the libraries
    if prj.naming ~= nil and prj.naming ~= "none" then     
      local libraryDebugName = prj.name
      local libraryReleaseName = prj.name

      if wksp == desc then
        -- do nothing!  We want to link based on the project name without any qualitification
      elseif prj.naming == "standard" then
        libraryDebugName = libraryDebugName .. "d.lib"
        libraryReleaseName = libraryReleaseName .. ".lib"  
      elseif prj.naming == "versioned" then
        libraryDebugName = libraryDebugName .. prj.version .. "d.lib"
        libraryReleaseName = libraryReleaseName .. prj.version .. ".lib"
      elseif prj.naming == "win32" then
        libraryDebugName = libraryDebugName .. "32.lib"
        libraryReleaseName = libraryReleaseName .. "32.lib"
      elseif prj.naming == "architecture" then
        libraryDebugName = libraryDebugName .. "64.lib"
        libraryReleaseName = libraryReleaseName .. "64.lib"
      elseif prj.naming == "boost" then
        local toolset = "v141"
        local threading = "mt"
        local runtime = "" --"s"
        local debug = "gd"
        local version = prj.version

        libraryDebugName = boost_combine_components( 
          prj.name, toolset, threading, runtime .. debug,
          version ) .. ".lib"

        libraryReleaseName = boost_combine_components( 
          prj.name, toolset, threading, runtime,
          version ) .. ".lib"
      end
  
      filter 'configurations:Release'
        links { libraryReleaseName } 
      filter {}

      filter 'configurations:Debug'
        links { libraryDebugName } 
      filter {} 
    end

    -- include any specific libraries from this project.
    builder.setupDependencies( prj.dependencies, wksp )    
  end


--- 
--  
---

  function builder.setupFramework( frameworkName, wksp )

    -- Check if there is a framework specified for this dependency
    local libraryName = frameworkName
    local parts = string.explode( frameworkName, ":" )
    if table.getn( parts ) > 1 then
      frameworkName = parts[1]
      libraryName = parts[2] -- could use better processing here for sure,
    end

    -- Attempt to grab the named framework from the global libraries list.
    local framework = nil
    for id, obj in pairs( user_libraries ) do
      if frameworkName == id then
        framework = obj
      end
    end

    -- Check if the requested framework is actually a project from this workspace
    if desc.projects[ frameworkName ] ~= nil then
      framework = desc.projects[ frameworkName ]
      builder.setupProject( desc.projects[ frameworkName ], desc )

    -- Still unable to find the requested framework, it could be a project from the most recent 
    -- included workspace/framework.
    elseif wksp ~= nil and wksp.projects ~= nil and wksp.projects[ frameworkName ] ~= nil then
      builder.setupProject( wksp.projects[ frameworkName ], wksp )
      --framework = wksp

    -- Else maybe it's a global project.
    elseif framework ~= nil then
      -- Check if there is a workspace file at the 'path' location which may hold more project 
      -- information
      if framework.path and os.isfile( path.join( framework.path, path.appendExtension( "workspace", ".lua" ) ) ) then
        local wksp = require( path.join( framework.path, "workspace" ) )
        framework = table.merge( framework, wksp )
      end

      -- Check for libraries in the framework to be included.
      if framework.projects ~= nil then
        -- if no library name is specified then include all libraries else only include the specified
        -- library.
        for _, lib in pairs( framework.projects ) do 
          if table.getn( parts ) == 1 or lib.name == libraryName then
            if lib.type == "StaticLib" then
              builder.setupProject( lib, framework )
            end
          end
        end

      -- if there are no libraries registered then attempt to include the framework as if it was a 
      -- library
      else
        builder.setupProject( framework, nil )

      end

    -- Unable to find the requested framework anywhere, log error message and continue.
    else
      premake.error( "Unable to find requested library, %s", frameworkName )

    end

  end


--- 
--  
---

  function builder.setupDependencies( dependencies, wksp )
    if dependencies then
      for _, name in pairs( dependencies ) do
        builder.setupFramework( name, wksp )
      end
    end
  end


--- 
--  
---

  function builder.createProject( details )
    project( details.name )
    
    kind( details.type )
    
    if details.type == "WindowedApp" then
      flags( "WinMain" )
    end
    
    architecture "x86_64"
    
    language "C++"
    
    location( 
      path.join( 
        _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ), 
        details.name 
      ) 
    )
    
    files {  
      path.join( details.path, "**.cpp" ), 
      path.join( details.path, "**.hpp" ),
      path.join( details.path, "**.h" )
    }

    includedirs {
      details.path,
      path.join( os.getcwd(), details.includePath ),
      path.join( 
        _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ), 
        "../..", "_external", "include" 
      )
    }
    
    libdirs {
      path.join( 
        _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ), 
        "../..", "_external", "lib" 
      )
    }

    if details.defines then
      for _, n in ipairs( details.defines ) do
        defines { n }
      end
    end

    builder.setupDependencies( details.dependencies );
  end





--- 
--  Create the workspace objects and setup defaults.
---

  workspace( desc.name )
    configurations { "Debug", "Release" }
    architecture "x86_64"
    language "C++"
    location( _OPTIONS["outdir"] or path.join( os.getcwd(), "_build", desc.name ) )  
    flags { "FloatFast" }

    defines {
      "PREMAKE5",
      "NEBULAE_INCLUDES_PNG"
    }

    includedirs {
      os.getcwd()
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

  for _, details in pairs( desc.projects ) do
    builder.createProject( details )
  end