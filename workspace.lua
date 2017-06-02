

local workspace = {}

workspace.name = 'Flock'
workspace.system = "premake5"

workspace.libraries = {
	common = {
	    name = 'common',
	    path = 'C:/Develop/Flock/Source/Common',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    naming = "standard",
	}
}

workspace.binaries = {
	server = {
	    name = 'server',
	    path = 'C:/Develop/Flock/Source/Server',     
	    includePath = 'C:/Develop/Flock/Source', 
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client_console = {
	    name = 'client_console',
	    path = 'C:/Develop/Flock/Source/ClientConsole',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq' },
	    naming = "standard",
	},
	client = {
	    name = 'client',
	    path = 'C:/Develop/Flock/Source/Client',      
	    includePath = 'C:/Develop/Flock/Source',
	    system = 'premake5',
	    dependencies = { 'common', 'cppzmq', 'NebulaeEngine:nebulous_alpha' },
	    naming = "standard",
	}	
}

return workspace