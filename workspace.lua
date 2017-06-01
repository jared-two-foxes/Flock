

local workspace = {}

workspace.name = 'Flock'
workspace.generator = "premake5"

workspace.libraries = {
	common = {
	    name = 'common',
	    path = 'C:/Develop/Flock/Common',      
	    system = 'premake5',
	    naming = "standard",
	}
}

workspace.binaries = {
	server = {
	    name = 'server',
	    path = 'C:/Develop/Flock/Server',      
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client_console = {
	    name = 'client_console',
	    path = 'C:/Develop/Flock/ClientConsole',      
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	-- client = {
	--     name = 'client',
	--     path = 'C:/Develop/Flock/Client',      
	--     system = 'premake5',
	--     dependencies = { 'common', 'cppzmq', 'nebulous_alpha' },
	--     naming = "standard",
	-- }	
}

return workspace