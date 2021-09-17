#!/usr/bin/env lua
-- MoonCCD example: hello.lua
local ccd = require("moonccd")

-- Support function for box objects:
local function support(obj, dir)
   -- Assume obj is a user defined table containing info about the object,
   -- in this case a box: obj = { pos, quat, x, y, z } where pos is the
   -- position, quat is the rotation, and x, y, z are the dimensions.
   -- Apply the rotation on direction vector:
   local qinv = ccd.qinvert(obj.quat)
   local dir = ccd.vrotate(dir, qinv)
   -- Compute the support point in the specified direction:
   local v = { 0.5*ccd.sign(dir[1])*obj.x,
               0.5*ccd.sign(dir[2])*obj.y,
               0.5*ccd.sign(dir[3])*obj.z }
   -- Transform support point according to the rotation of the object and return it
   v = ccd.vrotate(v, obj.quat)
   v = ccd.vadd(v, obj.pos)
   return v
end

-- Create two box objects:
local box1 = { pos={-5, 0, 0}, quat={1, 0, 0, 0}, x=1, y=2, z=1 }
local box2 = { pos={ 0, 0, 0}, quat={1, 0, 0, 0}, x=2, y=1, z=2 }

-- Create and initialize the ccd parameters:
local ccdpar = ccd.new({
   support1 = support,   -- support function for first object
   support2 = support,   -- support function for second object
   max_iterations = 100, -- maximum number of iterations
})

for i = 0, 99 do
   local intersect = ccd.gjk_intersect(ccdpar, box1, box2)
   -- now intersect is true if the two boxes intersect, false otherwise
   -- print(i, intersect, box1.pos[1], box2.pos[1])
   if i < 35 or i > 65 then
      assert(not intersect)
   elseif i~=35 and i~=65 then
      assert(intersect)
   end
   -- move first box along the x axis:
   box1.pos[1] = box1.pos[1] + 0.1
end

