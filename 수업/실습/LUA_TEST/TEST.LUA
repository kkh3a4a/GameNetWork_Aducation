function plustwo( x )
local a;
a = 2
return x + a;
end

function plustwo_subtwo( x )
local a;
a = 2
return x + a, x - a;
end

function addnum( a, b )
return c_addnum(a,b)
end

pos_x = 6
pos_y = plustwo(pos_x)
pos_z = plustwo(pos_y)
