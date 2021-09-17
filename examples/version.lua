#!/usr/bin/env lua
-- MoonCCD example: versio.lua

local ccd = require("moonccd")
local glmath = require("moonglmath")
print(ccd._VERSION)
print(ccd._LIBCCD_VERSION)
--print("REAL_MAX", ccd.REAL_MAX)
--print("EPS", ccd.EPS)
