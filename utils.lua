

package.path = package.path .. ";" .. os.getenv("userprofile") .. "/?.lua;"


local user_libraries = require( 'libraries' )

-- local tablex = require( 'pl.tablex' )

local utils = {}

function utils.addLibrariesToCurrentProject( dependencies )
  for i, name in pairs( dependencies ) do

    -- Grab the named library from the global libraries list.
    local library = nil
    for j, libObj in pairs( user_libraries ) do
      if name == libObj.name then
        library = libObj
      end
    end

    -- if includePath ~= nil then includedirs { includePath } end
    -- if libPath ~= nil then     libdirs { libPath } end

    if library ~= nil then     
      filter 'configurations:Release'
        links { library.library } 
      filter {}

      filter 'configurations:Debug'
        links { library.library .. "d" } 
      filter {}
    end
  end
end


return utils