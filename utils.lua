

package.path = package.path .. ";" .. os.getenv("userprofile") .. "/?.lua;"

local user_libraries = require( 'libraries' )
local utils = {}

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

function utils.addLibrariesToCurrentProject( dependencies )
  for _, name in pairs( dependencies ) do

    -- Grab the named library from the global libraries list.
    local library = nil
    for _, libObj in pairs( user_libraries ) do
      if name == libObj.name then
        library = libObj
      end
    end

    -- if includePath ~= nil then includedirs { includePath } end
    -- if libPath ~= nil then     libdirs { libPath } end

    if library ~= nil then

      -- Add the libraries.
      if library.naming ~= "none" then     
        local libraryDebugName = library.name
        local libraryReleaseName = library.name

        if library.naming == "standard" then
          libraryDebugName = libraryDebugName .. "d.lib"
          libraryReleaseName = libraryReleaseName .. ".lib"
        elseif library.naming == "boost" then
          local toolset = "v141"
          local threading = "mt"
          local runtime = "" --"s"
          local debug = "gd"
          local version = "4_2_3"

          libraryDebugName = boost_combine_components( 
            library.name, toolset, threading, runtime .. debug,
            library.version ) .. ".lib"

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

      -- Recurse if requried
      if library.dependencies then
        utils.addLibrariesToCurrentProject( library.dependencies )
      end
    end

  end
end


return utils