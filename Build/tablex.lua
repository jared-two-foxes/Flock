--- Extended operations on Lua tables.
--
-- See @{02-arrays.md.Useful_Operations_on_Tables|the Guide}
--
-- Dependencies: `pl.utils`, `pl.types`
-- @module pl.tablex

local getmetatable,setmetatable,require = getmetatable,setmetatable,require
local tsort,append,remove = table.sort,table.insert,table.remove
local min = math.min
local pairs,type,select,tostring = pairs,type,select,tostring


local tablex = {}

-- generally, functions that make copies of tables try to preserve the metatable.
-- However, when the source has no obvious type, then we attach appropriate metatables
-- like List, Map, etc to the result.
local function setmeta (res,tbl)
    local mt = getmetatable(tbl)
    return mt and setmetatable(res, mt) or res
end

--- make a shallow copy of a table
-- @within Copying
-- @tab t an iterable source
-- @return new table
function tablex.copy (t)
    -- assert_arg_iterable(1,t)
    local res = {}
    for k,v in pairs(t) do
        res[k] = v
    end
    return res
end

--- combine two tables, either as union or intersection. Corresponds to
-- set operations for sets () but more general. Not particularly
-- useful for list-like tables.
-- @within Merging
-- @tab t1 a table
-- @tab t2 a table
-- @bool dup true for a union, false for an intersection.
-- @usage merge({alice=23,fred=34},{bob=25,fred=34}) is {fred=34}
-- @usage merge({alice=23,fred=34},{bob=25,fred=34},true) is {bob=25,fred=34,alice=23}
-- @see tablex.index_map
function tablex.merge (t1,t2,dup)
    -- assert_arg_iterable(1,t1)
    -- assert_arg_iterable(2,t2)
    local res = {}
    for k,v in pairs(t1) do
        if dup or t2[k] then res[k] = v end
    end
    if dup then
      for k,v in pairs(t2) do
        res[k] = v
      end
    end
    return setmeta(res,t1,'Map')
end

--- the union of two map-like tables.
-- If there are duplicate keys, the second table wins.
-- @tab t1 a table
-- @tab t2 a table
-- @treturn tab
-- @see tablex.merge
function tablex.union(t1, t2)
    return tablex.merge(t1, t2, true)
end

--- the intersection of two map-like tables.
-- @tab t1 a table
-- @tab t2 a table
-- @treturn tab
-- @see tablex.merge
function tablex.intersection(t1, t2)
    return tablex.merge(t1, t2, false)
end

return tablex